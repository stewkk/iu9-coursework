package inotify

type Event any

type CreateEvent struct {
	Path string
}

type DeleteEvent struct {
	Path string
	Eof  bool
}
