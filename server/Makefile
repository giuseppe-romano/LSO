CC=gcc
CFLAGS=-I.

server: create-dir shared.a protocol.o game.o menu.o player.o
	$(CC) -Lbuild/ -Wall -g -I../shared -lpthread -lshared -o build/server.o -c src/main.c
	g++ -o server build/server.o build/game.o build/protocol.o build/menu.o build/player.o -lpthread build/libshared.a
	
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
	
game.o: 
	$(CC) -Wall -g -I../shared -c src/game/game.c -o build/game.o
	
protocol.o: 
	$(CC) -Lbuild/ -Wall -g -I../shared -lshared -c src/protocol/protocol.c -o build/protocol.o

menu.o: 
	$(CC) -Lbuild/ -Wall -g -I../shared -lshared -c src/menu/menu.c -o build/menu.o
	
player.o: 
	$(CC) -Lbuild/ -Wall -g -I../shared -lshared -c src/player/player.c -o build/player.o	
	


	
	
