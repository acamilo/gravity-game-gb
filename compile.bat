REM Automatically generated from Makefile
mkdir -p obj
lcc   -c -o obj\main.o src\main.c
lcc   -c -o obj\map.o res\map.c
lcc   -c -o obj\tiles.o res\tiles.c
lcc   -o obj\game.gb obj\main.o obj\map.o obj\tiles.o 
