package inotify

import (
	"errors"
	"log/slog"
	"os"
	"strings"
	"syscall"
	"testing"
	"unsafe"

	"github.com/google/uuid"
	"github.com/stretchr/testify/require"
	"github.com/stretchr/testify/suite"
	"golang.org/x/sys/unix"
)

type Watch struct {
	events chan<- Event
	logger *slog.Logger

	inotify *os.File
	buf *inotifyBuffer
	nameByWatchDescriptor map[int] string
}

type inotifyBuffer struct {
	data []byte
	offset int
	numRead int
}

type Event struct {}

func NewWatch(events chan<- Event, logger *slog.Logger) (Watch, error) {
	fd, err := unix.InotifyInit()
	if err != nil {
		return Watch{}, err
	}

	logger.Info("Initialized new inotify instance")

	w := Watch{
		events:                events,
		inotify:               os.NewFile(uintptr(fd), uuid.NewString()),
		buf:                   &inotifyBuffer{},
		logger:                logger,
		nameByWatchDescriptor: make(map[int]string),
	}

	logger.Info("Start serving events", "inotify", w.inotify.Name())
	go w.serveEvents()

	return w, nil
}

type inotifyEvent struct {
	*unix.InotifyEvent
	name string
}

func (w *Watch) readEventName(event *unix.InotifyEvent) string {
	if event.Len == 0 {
		name, ok := w.nameByWatchDescriptor[int(event.Wd)]
		if !ok {
			w.logger.Error("Watch descriptor not found", "wd", int(event.Wd))
			panic("failed to determine file name of event: watch decriptor not found")
		}
		return name
	} else {
		buf := w.buf
		name := strings.TrimRight(string(buf.data[buf.offset:buf.offset+int(event.Len)]), "\x00")
		buf.offset += int(event.Len)
		return name
	}

}

func (w *Watch) readEvent() (*inotifyEvent, error)  {
	buf := w.buf
	if buf.offset >= buf.numRead {
		buf.data = make([]byte, 10*(syscall.SizeofInotifyEvent+unix.NAME_MAX+1)) // NOTE: maybe reuse memory here?
		var err error
		buf.numRead, err = w.inotify.Read(buf.data)
		if err != nil {
			return nil, err
		}
		if buf.numRead == 0 {
			return nil, errors.New("read() from inotify returned 0")
		}
		buf.offset = 0
		w.logger.Debug("Successful read() from inotify", "numRead", buf.numRead)
	}

	var event inotifyEvent
	event.InotifyEvent = (*unix.InotifyEvent)(unsafe.Pointer(&buf.data[buf.offset]))
	buf.offset += syscall.SizeofInotifyEvent
	event.name = w.readEventName(event.InotifyEvent)

	w.logger.Debug("Read event", "wd", event.Wd, "name", event.name, "len", event.Len,
	"flags", event.Mask)

	return &event, nil
}

func (w *Watch) serveEvents() {
	for {
		_, err := w.readEvent()
		if err != nil {
			w.logger.Warn("Got error while reading event, stopped serving events", "err", err.Error())
			close(w.events)
			return
		}
		w.logger.Debug("Serve some event") // TODO: extended log
		w.events <- Event{}
	}
}

func (w *Watch) Subscribe(path string) error {
	wd, err := unix.InotifyAddWatch(int(w.inotify.Fd()), path, unix.IN_CREATE|unix.IN_MODIFY|unix.IN_DELETE|unix.IN_MOVED_TO|unix.IN_MOVED_FROM|unix.IN_MOVE)
	if err != nil {
		return err
	}
	w.nameByWatchDescriptor[wd] = path
	w.logger.Info("Subscribed to new path", "wd", wd, "path", path)
	return nil
}

type InotifyTestSuite struct {
    suite.Suite
	root string
}

func (s *InotifyTestSuite) SetupTest() {
	root, err := os.MkdirTemp("", "inotify-go")
	if err != nil {
		panic(err)
	}

	s.root = root

	h := slog.NewTextHandler(os.Stderr, &slog.HandlerOptions{Level: slog.LevelDebug})
	slog.SetDefault(slog.New(h))
}

func (s *InotifyTestSuite) TearDownTest() {
	err := os.Remove(s.root)
	if err != nil {
		panic(err)
	}
}

func (s *InotifyTestSuite) TestCanSubscribeToFile() {
	file, err := os.CreateTemp(s.root, "test")
	defer os.Remove(file.Name())
	w, _ := NewWatch(make(chan Event), slog.Default())

	w.Subscribe(file.Name())

	require.Nil(s.T(), err)
}

func (s *InotifyTestSuite) TestCanSubscribeToDirectory() {
	file, err := os.CreateTemp(s.root, "test")
	defer os.Remove(file.Name())
	w, _ := NewWatch(make(chan Event), slog.Default())

	w.Subscribe(file.Name())

	require.Nil(s.T(), err)
}

func (s *InotifyTestSuite) TestServeFileCreateEvent() {
	events := make(chan Event)
	w, err := NewWatch(events, slog.Default())
	w.Subscribe(s.root)
	require.Nil(s.T(), err)
	path := s.root+"/test"
	file, _ := os.Create(path)
	defer os.Remove(file.Name())
	file.Close()

	event := <-events

	require.Equal(s.T(), Event{}, event)
}

func TestExampleTestSuite(t *testing.T) {
    suite.Run(t, new(InotifyTestSuite))
}
