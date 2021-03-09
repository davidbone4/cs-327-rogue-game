#target: list of dependencies
#	rules to build target from dependencies

CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

build:
	gcc DungeonGenerator.c pathfinding.c heap.c monster.c main.c -o rlg327 -lm -lcurses

clean:
	rm rlg327
