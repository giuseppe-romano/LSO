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

client: shared.so
	$(CC) -Lbin/ -Wall -g -I../shared -c src/main.c -lshared -o build/client.o
	g++  -o client build/client.o build/libshared.so
	
