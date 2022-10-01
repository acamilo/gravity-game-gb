REM Automatically generated from Makefile
mkdir -p obj
lcc   -c -o obj\main.o src\main.c
lcc   -c -o obj\dungeon_map.o res\dungeon_map.c
lcc   -c -o obj\dungeon_tiles.o res\dungeon_tiles.c
lcc   -o obj\game.gb obj\main.o obj\dungeon_map.o obj\dungeon_tiles.o 
