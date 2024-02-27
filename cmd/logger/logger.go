package main

import (
	"fmt"
	"log/slog"
	"os"
	"time"

	"github.com/stewkk/iu9-coursework/pkg/inotify"
)

func main() {
	events := make(chan inotify.Event, 10)
	w, err := inotify.NewWatch(events, slog.Default())
	if err != nil {
		panic(err)
	}
	defer w.Close()
	select {
	case e := <-events:
		switch e := e.(type) {
		case inotify.CreateEvent:
			fmt.Println(e.Path)
		}
	case <-time.After(1*time.Second):
		os.Exit(1)
	}
}
