CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build: main

main: main.o CircularDoublyLinkedList.o Planet.o GalacticWar.o
	$(CC) $(CFLAGS) main_unlinked.o CircularDoublyLinkedList_unlinked.o Planet_unlinked.o GalacticWar_unlinked.o -o main

CircularDoublyLinkedList.o: CircularDoublyLinkedList.h CircularDoublyLinkedList.c
	$(CC) $(CFLAGS) CircularDoublyLinkedList.c -c -o CircularDoublyLinkedList_unlinked.o

Planet.o: Planet.h Planet.c
	$(CC) $(CFLAGS) Planet.c -c -o Planet_unlinked.o

GalacticWar.o: GalacticWar.h GalacticWar.c
	$(CC) $(CFLAGS) GalacticWar.c -c -o GalacticWar_unlinked.o

main.o: main.c
	$(CC) $(CFLAGS) main.c -c -o main_unlinked.o

run:
	./main

clean:
	rm *.o
	rm main