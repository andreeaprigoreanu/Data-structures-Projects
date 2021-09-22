/* Copyright 2021 <> */
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

#define MAX_PRODUCTS 1000

server_memory* init_server_memory() {
	/* TODO. */
	server_memory *server = (server_memory *)malloc(sizeof(server_memory));
	DIE(!server, "malloc failed\n");

	unsigned int hmax = MAX_PRODUCTS;
	server->products = ht_create(hmax, hash_function_key,
								 compare_function_strings);

	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	/* TODO. */
	ht_put(server->products, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	/* TODO. */
	ht_remove_entry(server->products, key);
}

char* server_retrieve(server_memory* server, char* key) {
	/* TODO. */
	char *value = (char *)ht_get(server->products, key);
	return value;
}

void free_server_memory(server_memory* server) {
	/* TODO. */
	ht_free(server->products);
	free(server);
}
