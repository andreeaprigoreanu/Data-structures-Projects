/* Copyright 2021 <Andreea Prigoreanu> */
#include <stdlib.h>
#include <string.h>

#include "hashring.h"
#include "load_balancer.h"
#include "utils.h"

#define MAX_SERVERS 100000
#define MAX_BUFFER 400

struct load_balancer {
	/* TODO. */
    unsigned int num_servers;
    server_memory **servers;
    unsigned int *hashring;
};

load_balancer* init_load_balancer() {
	/* TODO. */
    load_balancer *main = (load_balancer *)calloc(1, sizeof(load_balancer));
    DIE(!main, "calloc failed");

    main->num_servers = 0;

    main->servers = (server_memory **)calloc(MAX_SERVERS,
                     sizeof(server_memory *));
    DIE(!main->servers, "calloc failed\n");

    main->hashring = (unsigned int *)calloc(3 * MAX_SERVERS,
                      sizeof(unsigned int));
    DIE(!main->hashring, "calloc failed\n");

    return main;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	/* TODO. */
    unsigned int hash_key = hash_function_key(key);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                hash_function_servers, hash_key);
    if (pos == main->num_servers) {
        pos = 0;
    }

    *server_id = main->hashring[pos] % MAX_SERVERS;

    server_store(main->servers[*server_id], key, value);
}

char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	/* TODO. */
    unsigned int hash_key = hash_function_key(key);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                hash_function_servers, hash_key);
    if (pos == main->num_servers) {
        pos = 0;
    }

    *server_id = main->hashring[pos] % MAX_SERVERS;

	return server_retrieve(main->servers[*server_id], key);
}

static void move_keys(load_balancer* main, unsigned int hash_server,
                      unsigned int server_id, unsigned int pos)
{
    unsigned int hash_next, hash_prev;
    unsigned int next_id;

    if (pos == 0) {
        hash_next = hash_function_servers(&main->hashring[0]);
        hash_prev =
                hash_function_servers(&main->hashring[main->num_servers - 1]);
        next_id =  main->hashring[0] % MAX_SERVERS;

        if (next_id == server_id) {
            return;
        }

        unsigned int hmax = main->servers[next_id]->products->hmax;
        for (unsigned int i = 0; i < hmax; ++i) {
            linked_list_t *bucket =
                                main->servers[next_id]->products->buckets[i];
            ll_node_t *node = bucket->head;

            while (node) {
                char key[MAX_BUFFER];
                memcpy(key, (char *)(((struct info *)node->data)->key),
                       strlen((char *)(((struct info *)node->data)->key)) + 1);
                char value[MAX_BUFFER];
                memcpy(value, (char *)(((struct info *)node->data)->value),
                     strlen((char *)(((struct info *)node->data)->value)) + 1);

                node = node->next;

                unsigned int hash_key = hash_function_key(key);

                if ((hash_key <= hash_server && hash_key < hash_next) ||
                    (hash_key > hash_prev && hash_key > hash_server)) {
                    server_store(main->servers[server_id], key, value);
                    server_remove(main->servers[next_id], key);
                }
            }
        }
        
    } else {
        if (pos == main->num_servers) {
            if (main->num_servers == 1) {
                hash_next = hash_function_servers(&main->hashring[pos - 1]);
                next_id = main->hashring[pos - 1] % MAX_SERVERS;

                if (next_id == server_id) {
                    return;
                }

                unsigned int hmax = main->servers[next_id]->products->hmax;
                for (unsigned int i = 0; i < hmax; ++i) {
                    linked_list_t *bucket =
                                main->servers[next_id]->products->buckets[i];
                    ll_node_t *node = bucket->head;

                    while (node) {
                        char key[MAX_BUFFER];
                        memcpy(key, (char *)(((struct info *)node->data)->key),
                            strlen((char *)(((struct info *)node->data)->key))
                                    + 1);
                        char value[MAX_BUFFER];
                        memcpy(value, 
                           (char *)(((struct info *)node->data)->value),
                           strlen((char *)(((struct info *)node->data)->value))
                           + 1);

                        node = node->next;

                        unsigned int hash_key = hash_function_key(key);
                        if (hash_key <= hash_server && hash_key > hash_next) {
                            server_store(main->servers[server_id], key, value);
                            server_remove(main->servers[next_id], key);
                        }
                    }
                }                
            } else {
                hash_next = hash_function_servers(&main->hashring[pos - 1]);
                hash_prev = hash_function_servers(&main->hashring[pos - 2]);
                next_id = main->hashring[0] % MAX_SERVERS;

                if (next_id == server_id) {
                    return;
                }

                unsigned int hmax = main->servers[next_id]->products->hmax;
                for (unsigned int i = 0; i < hmax; ++i) {
                    linked_list_t *bucket =
                                main->servers[next_id]->products->buckets[i];
                    ll_node_t *node = bucket->head;

                    while (node) {
                        char key[MAX_BUFFER];
                        memcpy(key, (char *)(((struct info *)node->data)->key),
                             strlen((char *)(((struct info *)node->data)->key))
                             + 1);
                        char value[MAX_BUFFER];
                        memcpy(value, 
                           (char *)(((struct info *)node->data)->value),
                           strlen((char *)(((struct info *)node->data)->value))
                           + 1);

                        node = node->next;

                        unsigned int hash_key = hash_function_key(key);
                        if (hash_key <= hash_server && hash_key > hash_prev) {
                            server_store(main->servers[server_id], key, value);
                            server_remove(main->servers[next_id], key);
                        }
                    }
                }

            }
        } else {
            hash_next = hash_function_servers(&main->hashring[pos]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[pos] % MAX_SERVERS;

            if (next_id == server_id) {
                return;
            }

            unsigned int hmax = main->servers[next_id]->products->hmax;
            for (unsigned int i = 0; i < hmax; ++i) {
                linked_list_t *bucket =
                                main->servers[next_id]->products->buckets[i];
                ll_node_t *node = bucket->head;

                while (node) {
                    char key[MAX_BUFFER];
                    memcpy(key, (char *)(((struct info *)node->data)->key),
                           strlen((char *)(((struct info *)node->data)->key))
                           + 1);
                    char value[MAX_BUFFER];
                    memcpy(value, (char *)(((struct info *)node->data)->value),
                           strlen((char *)(((struct info *)node->data)->value))
                           + 1);

                    node = node->next;

                    unsigned int hash_key = hash_function_key(key);
                    if (hash_key <= hash_server && hash_key > hash_prev) {
                        server_store(main->servers[server_id], key, value);
                        server_remove(main->servers[next_id], key);
                    }
                }
            }
        }
    }
}

void loader_add_server(load_balancer* main, int server_id) {
	/* TODO. */
    main->servers[server_id] = init_server_memory();

    unsigned int label = server_id;
    unsigned int hash_server = hash_function_servers(&label);
    unsigned int pos = find_position(main->num_servers, main->hashring, 
                                     &hash_function_servers, hash_server);
    if (main->num_servers) {
        if (hash_function_servers(&main->hashring[pos]) == hash_server) {
            if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
                ++pos;
            }
        }
        move_keys(main, hash_server, server_id, pos);
        add_element(&main->num_servers, main->hashring, label, pos);
    } else {
        add_element(&main->num_servers, main->hashring, label, 0);
    }

    label = MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring, 
                        &hash_function_servers, hash_server);
    if (hash_function_servers(&main->hashring[pos]) == hash_server) {
        if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
            ++pos;
        }
    }
    move_keys(main, hash_server, server_id, pos);
    add_element(&main->num_servers, main->hashring, label, pos);

    label = 2 * MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);
    if (hash_function_servers(&main->hashring[pos]) == hash_server) {
        if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
            ++pos;
        }
    }
    move_keys(main, hash_server, server_id, pos);
    add_element(&main->num_servers, main->hashring, label, pos);
}

static void move_server(load_balancer* main, unsigned int server_id,
                        unsigned int pos)
{
    unsigned int next_id;
    unsigned int hash_next, hash_prev, hash_server;
    if (pos == 0) {
        hash_server = hash_function_servers(&main->hashring[pos + 1]);
        hash_next = hash_function_servers(&main->hashring[pos + 1]);
        hash_prev =
                hash_function_servers(&main->hashring[main->num_servers - 1]);
        next_id =  main->hashring[pos + 1] % MAX_SERVERS;

        if (next_id == server_id) {
            return;
        }

        unsigned int hmax = main->servers[server_id]->products->hmax;
        for (unsigned int i = 0; i < hmax; ++i) {
            linked_list_t *bucket =
                                main->servers[server_id]->products->buckets[i];
            ll_node_t *node = bucket->head;

            while (node) {
                char key[MAX_BUFFER];
                memcpy(key, (char *)(((struct info *)node->data)->key),
                       strlen((char *)(((struct info *)node->data)->key)) + 1);
                char value[MAX_BUFFER];
                memcpy(value, (char *)(((struct info *)node->data)->value),
                       strlen((char *)(((struct info *)node->data)->value))
                       + 1);

                node = node->next;

                unsigned int hash_key = hash_function_key(key);
                if ((hash_key > hash_next && hash_key > hash_prev) ||
                    (hash_key <=hash_next && hash_key <= hash_server)) {
                    server_store(main->servers[next_id], key, value);
                    server_remove(main->servers[server_id], key);
                }
            }
        }
    } else {
        if (pos == main->num_servers - 1) {
            hash_server = hash_function_servers(&main->hashring[pos + 1]);
            hash_next = hash_function_servers(&main->hashring[0]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[0] % MAX_SERVERS;

            if (next_id == server_id) {
                return;
            }

            unsigned int hmax = main->servers[server_id]->products->hmax;
            for (unsigned int i = 0; i < hmax; ++i) {
                linked_list_t *bucket =
                                main->servers[server_id]->products->buckets[i];
                ll_node_t *node = bucket->head;

                while (node) {
                    char key[MAX_BUFFER];
                    memcpy(key, (char *)(((struct info *)node->data)->key),
                           strlen((char *)(((struct info *)node->data)->key))
                           + 1);
                    char value[MAX_BUFFER];
                    memcpy(value, (char *)(((struct info *)node->data)->value),
                           strlen((char *)(((struct info *)node->data)->value))
                           + 1);

                    node = node->next;

                    unsigned int hash_key = hash_function_key(key);
                    if (hash_key > hash_next && hash_key > hash_prev) {
                        server_store(main->servers[next_id], key, value);
                        server_remove(main->servers[server_id], key);
                    }
                }
            }
        } else {
            hash_server = hash_function_servers(&main->hashring[pos + 1]);
            hash_next = hash_function_servers(&main->hashring[pos + 1]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[pos + 1] % MAX_SERVERS;

            if (next_id == server_id) {
                return;
            }

            unsigned int hmax = main->servers[server_id]->products->hmax;
            for (unsigned int i = 0; i < hmax; ++i) {
                linked_list_t *bucket =
                                main->servers[server_id]->products->buckets[i];
                ll_node_t *node = bucket->head;

                while (node) {
                    char key[MAX_BUFFER];
                    memcpy(key, (char *)(((struct info *)node->data)->key),
                           strlen((char *)(((struct info *)node->data)->key))
                           + 1);
                    char value[MAX_BUFFER];
                    memcpy(value, (char *)(((struct info *)node->data)->value),
                           strlen((char *)(((struct info *)node->data)->value))
                           + 1);

                    node = node->next;

                    unsigned int hash_key = hash_function_key(key);
                    if (hash_key <= hash_next && hash_key > hash_prev) {
                        server_store(main->servers[next_id], key, value);
                        server_remove(main->servers[server_id], key);
                    }
                }
            }
        }
    }
}

void loader_remove_server(load_balancer* main, int server_id) {
	/* TODO. */
    unsigned int label = server_id;
    unsigned int hash_server = hash_function_servers(&label);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                     &hash_function_servers, hash_server);

    move_server(main, server_id, pos);

    delete_element(&main->num_servers, main->hashring, pos);

    label = MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring, 
                        &hash_function_servers, hash_server);

    move_server(main, server_id, pos);

    delete_element(&main->num_servers, main->hashring, pos);

    label = 2 * MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);
    
    move_server(main, server_id, pos);
    
    delete_element(&main->num_servers, main->hashring, pos);

    free_server_memory(main->servers[server_id]);
    main->servers[server_id] = NULL;
}

void free_load_balancer(load_balancer* main) {
    /* TODO. */
    for (unsigned int i = 0; i < MAX_SERVERS; ++i) {
        if (main->servers[i]) {
            free_server_memory(main->servers[i]);
        }
    }
    free(main->servers);

    free(main->hashring);

    free(main);
}
