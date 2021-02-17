#target: list of dependencies
#	rules to build target from dependencies

CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

load: load.c
	gcc load.c -o load.out

# DungeonGenerator: DungeonGenerator.c
# 	gcc DungeonGenerator.c -o generator.out -lm
