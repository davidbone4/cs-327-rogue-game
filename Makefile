#target: list of dependencies
#	rules to build target from dependencies

DungeonGenerator: DungeonGenerator.c
	gcc DungeonGenerator.c -o generator.out -lm
