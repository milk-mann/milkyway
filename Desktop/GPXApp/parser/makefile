CC = gcc
CFLAGS = -Wall -std=c11 -g
FOLDER = -Iinclude

all: LinkedListAPI.o GPXParser.o parser

LinkedListAPI.o: ./src/LinkedListAPI.c ./include/LinkedListAPI.h
	$(CC) $(CFLAGS) $(FOLDER) -c -fpic -I/usr/include/libxml2 -lxml2 ./src/LinkedListAPI.c -o ./bin/LinkedListAPI.o

GPXParser.o: ./src/GPXParser.c ./include/GPXParser.h
	$(CC) $(CFLAGS) $(FOLDER) -c -fpic -I/usr/include/libxml2 -lxml2 ./src/GPXParser.c -o ./bin/GPXParser.o

parser: ./bin/GPXParser.o
	gcc -shared -I/usr/include/libxml2 -lxml2 ./bin/GPXParser.o ./bin/LinkedListAPI.o -o ../libgpxparse.so

clean:
	rm ../*.so ./bin/*.o
