package inotify

import (
	"errors"
	"fmt"
	"path/filepath"
	"strings"
	"syscall"
	"unsafe"

	"golang.org/x/sys/unix"
)

type inotifyBuffer struct {
	data []byte
	offset int
	numRead int
}

type inotifyEvent struct {
	*unix.InotifyEvent
	name string
}

func (e inotifyEvent) Contains(masks... int) bool {
	var mask int
	for _, s := range masks {
		mask |= s
	}
	return (int(e.Mask)&mask) != 0
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
		w.logger.Debug("Successful read() from inotify", "numRead", buf.numRead, "inotify", w.inotify.Name())
	}

	var event inotifyEvent
	event.InotifyEvent = (*unix.InotifyEvent)(unsafe.Pointer(&buf.data[buf.offset]))
	buf.offset += syscall.SizeofInotifyEvent
	event.name = w.readEventName(event.InotifyEvent)

	w.logger.Debug("Read event", "wd", event.Wd, "name", event.name, "len", event.Len,
	"flags", event.Mask)

	return &event, nil
}

func (w *Watch) convertToEvent(inotifyEvent *inotifyEvent) Event {
	dir, ok := w.nameByWatchDescriptor[int(inotifyEvent.Wd)]
	if !ok {
		panic("failed to determine dir of event: watch decriptor not found")
	}
	switch {
	case inotifyEvent.Contains(unix.IN_ACCESS):
		return "acess"
	case inotifyEvent.Contains(unix.IN_ATTRIB):
		return "attrib"
	case inotifyEvent.Contains(unix.IN_CLOSE_WRITE):
		return "close_write"
	case inotifyEvent.Contains(unix.IN_CLOSE_NOWRITE):
		return "close_nowrite"
	case inotifyEvent.Contains(unix.IN_CREATE):
		return CreateEvent{
			Path: filepath.Join(dir, inotifyEvent.name),
		}
	case inotifyEvent.Contains(unix.IN_DELETE):
		return DeleteEvent{
			Path: filepath.Join(dir, inotifyEvent.name),
		}
	case inotifyEvent.Contains(unix.IN_DELETE_SELF):
		return DeleteEvent{
			Path: inotifyEvent.name,
			IsWatched: true,
		}
	case inotifyEvent.Contains(unix.IN_MODIFY):
		return "modify"
	case inotifyEvent.Contains(unix.IN_MOVE_SELF):
		return "move_self"
	case inotifyEvent.Contains(unix.IN_MOVED_FROM):
		return "moved_from"
	case inotifyEvent.Contains(unix.IN_MOVED_TO):
		return "moved_to"
	case inotifyEvent.Contains(unix.IN_OPEN):
		return "open"
	default:
		return "no handle"
	}
}

func (w *Watch) serveEvents(stop <-chan struct{}) {
	defer w.logger.Info("Stop serving events", "inotify", w.inotify.Name())
	w.logger.Info("Start serving events", "inotify", w.inotify.Name())
	for {
		event, err := w.readEvent()
		if err != nil {
			w.logger.Warn(fmt.Sprintf("Got error while reading events: %v", err.Error()))
			close(w.events)
			return
		}
		select {
		case w.events <- w.convertToEvent(event):
			w.logger.Debug("Serve event", "value", event) // TODO: extended log
		case <-stop:
			w.logger.Debug("serveEvents got stop signal")
			close(w.events)
			return
		}
	}
}
