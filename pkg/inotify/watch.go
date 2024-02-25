package inotify

import (
	"log/slog"
	"os"

	"github.com/google/uuid"
	"golang.org/x/sys/unix"
)

type Watch struct {
	events chan<- Event
	logger *slog.Logger

	inotify *os.File
	buf *inotifyBuffer
	nameByWatchDescriptor map[int] string

	stopServing chan<- struct{}
}

func NewWatch(events chan<- Event, logger *slog.Logger) (Watch, error) {
	fd, err := unix.InotifyInit()
	if err != nil {
		return Watch{}, err
	}

	stopServing := make(chan struct{})

	w := Watch{
		events:                events,
		logger:                logger,
		inotify:               os.NewFile(uintptr(fd), uuid.NewString()),
		buf:                   &inotifyBuffer{},
		nameByWatchDescriptor: make(map[int]string),
		stopServing:           stopServing,
	}

	logger.Info("Initialized new inotify instance", "inotify", w.inotify.Name())

	go w.serveEvents(stopServing)

	return w, nil
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

func (w *Watch) Close() {
	close(w.stopServing)
	w.inotify.Close()
}
