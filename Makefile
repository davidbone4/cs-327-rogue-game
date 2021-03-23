#target: list of dependencies
#	rules to build target from dependencies

CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

build:
	g++ DungeonGenerator.cpp pathfinding.cpp heap.cpp monster.cpp main.cpp -o rlg327 -lm -lcurses

clean:
	rm rlg327
