CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HASHTABLE=hashtable
HASHRING=hashring
LINKEDLIST=linked_list

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HASHTABLE).o $(HASHRING).o $(LINKEDLIST).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(HASHTABLE).o: $(HASHTABLE).c $(HASHTABLE).h
	$(CC) $(CFLAGS) $^ -c

$(HASHRING).o: $(HASHRING).c $(HASHRING).h
	$(CC) $(CFLAGS) $^ -c

$(LINKEDLIST).o: $(LINKEDLIST).c $(LINKEDLIST).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
