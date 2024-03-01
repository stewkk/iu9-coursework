package inotify

type Event any

type CreateEvent struct {
	Path string
	IsDir bool
}

type DeleteEvent struct {
	Path string
	IsDir bool
}

type ModifyEvent struct {
	Path string
}

type RenameEvent struct {
	Path string
	MovedTo string
	IsDir bool
}

type OpenEvent struct {
	Path string
}

type CloseEvent struct {
	Path string
	IsReadOnly bool
}
