EXECUTABLE = quadtree
SOURCE_FILES = main.c quadtreeFunctions.c
FLAGS = -Wall -g

.PHONY: all build run clean
all: build

build:
	gcc  $(FLAGS) $(SOURCE_FILES) -o $(EXECUTABLE)

run:
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)