CC=gcc
CFLAGS=-I.

build-dir:
	mkdir -p build

draw.o: 
	$(CC) -Wall -g  -c ../shared/draw.c -o build/draw.o
	
serial.o: 
	$(CC) -Wall -g  -c ../shared/serial.c -o build/serial.o
	
shared.so: build-dir draw.o serial.o
	$(CC) -shared -o build/libshared.so build/draw.o build/serial.o
	
game.o: 
	$(CC) -Wall -g -I../shared -c src/game/game.c -o build/game.o
	
protocol.o: 
	$(CC) -Wall -g -I../shared -c src/protocol/protocol.c -o build/protocol.o

server: shared.so protocol.o game.o
	$(CC) -Lbuild/ -Wall -g -I../shared -Iinclude -lshared -lpthread -o build/server.o -c src/main.c
	g++  -o server build/server.o build/libshared.so build/game.o build/protocol.o  -lpthread
	
	
