package inotify

import (
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
	inotify *os.File
	buf inotifyBuffer

	logger *slog.Logger

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
		buf:                   inotifyBuffer{},
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

func (w *Watch) getEventName(event *unix.InotifyEvent) string {
	if event.Len == 0 {
		name, ok := w.nameByWatchDescriptor[int(event.Wd)]
		if !ok {
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
	buf := &w.buf
	if buf.offset >= buf.numRead {
		buf.data = make([]byte, 10*(syscall.SizeofInotifyEvent+unix.NAME_MAX+1))
		var err error
		buf.numRead, err = w.inotify.Read(buf.data)
		if err != nil || buf.numRead == 0 {
			return nil, err
		}
		buf.offset = 0
	}

	var event inotifyEvent
	event.InotifyEvent = (*unix.InotifyEvent)(unsafe.Pointer(&buf.data[buf.offset]))
	buf.offset += syscall.SizeofInotifyEvent
	event.name = w.getEventName(event.InotifyEvent)

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

func (w *Watch) Subscribe(path string) {
	wd, err := unix.InotifyAddWatch(int(w.inotify.Fd()), path, unix.IN_CREATE|unix.IN_MODIFY|unix.IN_DELETE|unix.IN_MOVED_TO|unix.IN_MOVED_FROM|unix.IN_MOVE)
	if err != nil {
		panic(err)
	}
	w.nameByWatchDescriptor[wd] = path
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
	w, _ := NewWatch(events, slog.Default())
	w.Subscribe(s.root)
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
