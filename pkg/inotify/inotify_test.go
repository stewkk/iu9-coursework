package inotify

import (
	"log/slog"
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/require"
	"github.com/stretchr/testify/suite"
)

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
	defer w.Close()

	w.Subscribe(file.Name())

	require.Nil(s.T(), err)
}

func (s *InotifyTestSuite) TestCanSubscribeToDirectory() {
	file, err := os.CreateTemp(s.root, "test")
	defer os.Remove(file.Name())
	w, _ := NewWatch(make(chan Event), slog.Default())
	defer w.Close()

	w.Subscribe(file.Name())

	require.Nil(s.T(), err)
}

func (s *InotifyTestSuite) TestServeFileCreateEvent() {
	events := make(chan Event)
	w, _ := NewWatch(events, slog.Default())
	defer w.Close()
	w.Subscribe(s.root)
	path := filepath.Join(s.root, "test")
	file, _ := os.Create(path)
	defer os.Remove(file.Name())
	file.Close()

	event := <-events

	require.Equal(s.T(), CreateEvent{
		eventBase: eventBase{
			Path: file.Name(),
		},
	}, event)
}

func TestExampleTestSuite(t *testing.T) {
    suite.Run(t, new(InotifyTestSuite))
}
