CC=gcc
CFLAGS=-I.

client: shared.a menu.o
	$(CC) -Lbuild/ -Wall -g -I../shared -lpthread -lshared -o build/client.o -c src/main.c
	g++ -o client build/client.o -lpthread build/menu.o build/libshared.a
	
clean:
	rm -rf build
	
create-dir: clean
	mkdir -p build

draw.o: 
	$(CC) -Wall -g  -c ../shared/draw.c -o build/draw.o
	
serial.o: 
	$(CC) -Wall -g  -c ../shared/serial.c -o build/serial.o

logging.o: 
	$(CC) -Wall -g  -c ../shared/logging.c -o build/logging.o
	
shared.a: create-dir draw.o serial.o logging.o
	ar -cvq build/libshared.a build/draw.o build/serial.o build/logging.o
	
menu.o: 
	$(CC) -Lbuild/ -Wall -g -I../shared -lshared -c src/menu/menu.c -o build/menu.o


	
