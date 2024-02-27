package inotify

type Event any

type CreateEvent struct {
	eventBase
}

type eventBase struct {
	Path string
}

