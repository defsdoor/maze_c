maze: maze.c
	gcc maze.c libattopng.c -o maze
maze.asm: maze.c
	gcc -S maze.c libattopng.c -o maze.asm
