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
    /* numarul total de replici ale serverelor */
    unsigned int num_servers;

    /* vector de pointeri la structuri de tipul server_memory */
    server_memory **servers;

    /* 
     * vector circular in care sunt stocate etichetele replicilor de servere
     * ordonate dupa hash
     */
    unsigned int *hashring;
};

load_balancer* init_load_balancer() {
	/* TODO. */

    /* se aloca memoria pentru o structura de tipul load_balancer */
    load_balancer *main = (load_balancer *)calloc(1, sizeof(load_balancer));
    DIE(!main, "calloc failed");

    main->num_servers = 0;

    /* se aloca memoria vectorul de pointeri la structuri server_memory */
    main->servers = (server_memory **)calloc(MAX_SERVERS,
                     sizeof(server_memory *));
    DIE(!main->servers, "calloc failed\n");

    /* se aloca memoria vectorul in care sunt stocate etichetele replicilor */
    main->hashring = (unsigned int *)calloc(3 * MAX_SERVERS,
                      sizeof(unsigned int));
    DIE(!main->hashring, "calloc failed\n");

    return main;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	/* TODO. */
    /*
     * se gaseste pozitia etichetei replicii serverului pe care trebuie adaugat
     * obiectul
     */
    unsigned int hash_key = hash_function_key(key);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                hash_function_servers, hash_key);
    if (pos == main->num_servers) {
        pos = 0;
    }

    /* se calculeaza id-ul serverului */
    *server_id = main->hashring[pos] % MAX_SERVERS;

    /* se apeleaza functia de server_store pentru serverul gasit*/
    server_store(main->servers[*server_id], key, value);
}

char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	/* TODO. */
    /*
     * se gaseste pozitia etichetei replicii serverului pe care este sticat
     * obiectul
     */
    unsigned int hash_key = hash_function_key(key);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                hash_function_servers, hash_key);
    if (pos == main->num_servers) {
        pos = 0;
    }

    /* se calculeaza id-ul serverului */
    *server_id = main->hashring[pos] % MAX_SERVERS;

    /* se apeleaza functia de server_retreive pentru serverul gasit */
	return server_retrieve(main->servers[*server_id], key);
}

/**
 * move_keys_load() - Functia gaseste serverele vecine si, daca este cazul, 
                      remapeaza obiectele pe noul server.
 * @arg1: Pointer la structura de tip load_balancer.
 * @arg2: Hash-ul etichetei replicii serverului care trebuie adaugat.
 * @arg3: Id-ul serverului nou adaugat.
 * @arg4: Pozitia din hashring pe care trebuie inserata eticheta replicii
         serverului.
 */
static void move_keys_load(load_balancer* main, unsigned int hash_server,
                      unsigned int server_id, unsigned int pos)
{
    unsigned int hash_next, hash_prev;
    unsigned int next_id;

    /* replica trebuie adaugata pe pozitia 0 in hashring: */
    if (pos == 0) {
        /*
         * se calculeaza hash-urile replicilor intre care trebuie inserata noua
         * replica si id_ul serverului de pe care trebuie transferate obiectele
         */
        hash_next = hash_function_servers(&main->hashring[0]);
        hash_prev =
                hash_function_servers(&main->hashring[main->num_servers - 1]);
        next_id =  main->hashring[0] % MAX_SERVERS;

        /*
         * daca serverul de pe care trebuie transferate obiectele coincide cu
         * cel al replicii care trebuie adaugate, cheile nu trebuie remapate
         */
        if (next_id == server_id) {
            return;
        }

        /*
         * se itereaza prin bucket-urile serverului succesor noului server
         * pentru a transfera cheile care respecta conditiile
         */
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
        /* replica trebuie adaugata pe ultima pozitie in hashring: */
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
                /*
                 * se calculeaza hash-urile replicilor dupa care trebuie
                 * inserata noua replica si id_ul serverului de pe care trebuie
                 * transferate obiectele
                 */
                hash_next = hash_function_servers(&main->hashring[pos - 1]);
                hash_prev = hash_function_servers(&main->hashring[pos - 2]);
                next_id = main->hashring[0] % MAX_SERVERS;

                /*
                 * daca serverului de pe care trebuie transferate obiectele
                 * coincide cu cel al replicii care trebuie adaugate, cheile
                 * nu trebuie remapate
                 */
                if (next_id == server_id) {
                    return;
                }

                /*
                 * se itereaza prin bucket-urile serverului succesor noului
                 * server pentru a transfera cheile care respecta conditiile
                 */
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
            /*
             * se calculeaza hash-urile replicilor intre care trebuie inserata
             * noua replica si id_ul serverului de pe care trebuie transferate
             * obiectele
             */
            hash_next = hash_function_servers(&main->hashring[pos]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[pos] % MAX_SERVERS;

            /*
             * daca serverului de pe care trebuie transferate obiectele
             * coincide cu cel al replicii care trebuie adaugate, cheile nu
             * trebuie remapate
             */
            if (next_id == server_id) {
                return;
            }

            /*
             * se itereaza prin bucket-urile serverului succesor noului server
             * pentru a transfera cheile care respecta conditiile
             */
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
    /* se aloca memoria pentru noul server cu id-ul server_id */
    main->servers[server_id] = init_server_memory();

    /* se adauga replica 1: */
    unsigned int label = server_id;
    unsigned int hash_server = hash_function_servers(&label);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                     &hash_function_servers, hash_server);
    if (main->num_servers != 0) {
        if (hash_function_servers(&main->hashring[pos]) == hash_server) {
            if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
                ++pos;
            }
        }
        move_keys_load(main, hash_server, server_id, pos);
        add_element(&main->num_servers, main->hashring, label, pos);
    } else {
        add_element(&main->num_servers, main->hashring, label, 0);
    }

    /* se adauga replica 2: */
    label = MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);
    if (hash_function_servers(&main->hashring[pos]) == hash_server) {
        if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
            ++pos;
        }
    }
    move_keys_load(main, hash_server, server_id, pos);
    add_element(&main->num_servers, main->hashring, label, pos);

    /* se adauga replica 3: */
    label = 2 * MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);
    if (hash_function_servers(&main->hashring[pos]) == hash_server) {
        if ((unsigned int)server_id > main->hashring[pos] % MAX_SERVERS) {
            ++pos;
        }
    }
    move_keys_load(main, hash_server, server_id, pos);
    add_element(&main->num_servers, main->hashring, label, pos);
}

static void move_keys_remove(load_balancer* main, unsigned int server_id,
                        unsigned int pos)
{
    unsigned int next_id;
    unsigned int hash_next, hash_prev, hash_server;

    /* replica care trebuie eliminata se afla pe pozitia 0 in hashring: */
    if (pos == 0) {
        /*
         * se calculeaza hash-urile replicilor vecine relicii ce trebuie sterse
         * din hashring si id_ul serverului pe care trebuie transferate obiectele
         */
        hash_server = hash_function_servers(&main->hashring[pos + 1]);
        hash_next = hash_function_servers(&main->hashring[pos + 1]);
        hash_prev =
                hash_function_servers(&main->hashring[main->num_servers - 1]);
        next_id =  main->hashring[pos + 1] % MAX_SERVERS;

        /*
         * daca serverul de pe care trebuie trebuie tansferate obiectele coincide cu
         * cel al replicii pe care trebuie adaugate, cheile nu trebuie remapate
         */
        if (next_id == server_id) {
            return;
        }

        /*
         * se itereaza prin bucket-urile serverului succesor serverului care
         * trebuie sters pentru a transfera cheile care respecta conditiile
         */
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
        /*
         * replica care trebuie eliminata se afla pe ultima pozitie in
         * hashring:
         */
        if (pos == main->num_servers - 1) {
            /*
             * se calculeaza hash-urile replicilor vecine relicii ce trebuie
             * sterse din hashring si id_ul serverului pe care trebuie
             * transferate obiectele
             */
            hash_server = hash_function_servers(&main->hashring[pos + 1]);
            hash_next = hash_function_servers(&main->hashring[0]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[0] % MAX_SERVERS;

            /*
             * daca serverul de pe care trebuie trebuie tansferate obiectele
             * coincide cu cel al replicii pe care trebuie adaugate, cheile
             * nu trebuie remapate
             */
            if (next_id == server_id) {
                return;
            }

            /*
             * se itereaza prin bucket-urile serverului succesor serverului
             * care trebuie sters pentru a transfera cheile care respecta
             * conditiile
             */
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
            /*
             * se calculeaza hash-urile replicilor vecine relicii ce trebuie
             * sterse din hashring si id_ul serverului pe care trebuie
             * transferate obiectele
             */
            hash_server = hash_function_servers(&main->hashring[pos + 1]);
            hash_next = hash_function_servers(&main->hashring[pos + 1]);
            hash_prev = hash_function_servers(&main->hashring[pos - 1]);
            next_id =  main->hashring[pos + 1] % MAX_SERVERS;

            /*
             * daca serverul de pe care trebuie trebuie tansferate obiectele
             * coincide cu cel al replicii pe care trebuie adaugate, cheile
             * nu trebuie remapate
             */
            if (next_id == server_id) {
                return;
            }

            /*
             * se itereaza prin bucket-urile serverului succesor serverului
             * care trebuie sters pentru a transfera cheile care respecta
             * conditiile
             */
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
    /* se sterge replica 1: */
    unsigned int label = server_id;
    unsigned int hash_server = hash_function_servers(&label);
    unsigned int pos = find_position(main->num_servers, main->hashring,
                                     &hash_function_servers, hash_server);

    move_keys_remove(main, server_id, pos);

    delete_element(&main->num_servers, main->hashring, pos);

    /* se sterge replica 3: */
    label = MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);

    move_keys_remove(main, server_id, pos);

    delete_element(&main->num_servers, main->hashring, pos);

    /* se sterge replica 3: */
    label = 2 * MAX_SERVERS + server_id;
    hash_server = hash_function_servers(&label);
    pos = find_position(main->num_servers, main->hashring,
                        &hash_function_servers, hash_server);

    move_keys_remove(main, server_id, pos);

    delete_element(&main->num_servers, main->hashring, pos);

    /* se elibereaza memoria alocata pentru serverul care trebuie eliminat */
    free_server_memory(main->servers[server_id]);
    main->servers[server_id] = NULL;
}

void free_load_balancer(load_balancer* main) {
    /* TODO. */
    /* se elibereaza memoria alocata pentru fiecare server din sistem */
    for (unsigned int i = 0; i < MAX_SERVERS; ++i) {
        if (main->servers[i]) {
            free_server_memory(main->servers[i]);
        }
    }
    /* se elibereaza memoria alocata pentru vectorul de pointeri la servere */
    free(main->servers);

    /* se elibereaza memoria alocata pentru vectorul circular */
    free(main->hashring);

    /* se elibereaza memoria pentru load_balancer */
    free(main);
}
