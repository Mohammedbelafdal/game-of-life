CC=gcc
LIBS= -lSDL2 -lSDL2_ttf
FLAGS= -O2 -Wall -Wextra
game.exe: main.c
				$(CC) $(LIBS) main.c  $(FLAGS)  -o  game.exe 
clean :
	echo done