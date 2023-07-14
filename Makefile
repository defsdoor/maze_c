maze: maze.c
	gcc maze.c -o maze
maze.asm: maze.c
	gcc -S maze.c -o maze.asm
