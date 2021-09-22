#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "hashtable.h"

/*
 * Functie de comparare a cheilor:
 */
int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing pentru cheie:
 */
unsigned int
hash_function_servers(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int
hash_function_key(void *a)
{
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
	DIE(!ht, "malloc failed\n");

	ht->buckets = (linked_list_t **)malloc(sizeof(linked_list_t *) * hmax);
	DIE(!ht->buckets, "malloc failed\n");
    for (unsigned int i = 0; i < hmax; ++i) {
        ht->buckets[i] = ll_create(sizeof(struct info));
    }

	ht->size = 0;
	ht->hmax = hmax;

	ht->hash_function = hash_function;
    ht->compare_function = compare_function;

	return ht;
}


/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune tipul ei), in momentul in care
 * se creeaza o noua intrare in hashtable (in cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie
 * a valorii la care pointeaza key si adresa acestei copii trebuie salvata in structura info asociata intrarii din ht.
 * Pentru a sti cati octeti trebuie alocati si copiati, folositi parametrul key_size_bytes.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel put(ht, key_actual, value_actual),
 * valoarea la care pointeaza key_actual poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia unei intrari din hashtable.
 * Nu ne dorim acest lucru, fiindca exista riscul sa ajungem in situatia in care nu mai stim la ce cheie este
 * inregistrata o anumita valoare.
 */
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
    unsigned int index = ht->hash_function(key) % ht->hmax;

	linked_list_t *bucket = ht->buckets[index];

	ll_node_t *node = bucket->head;

	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (ht->compare_function(key, ((struct info *)node->data)->key) == 0) {
			free(((struct info *)node->data)->value);
			
			((struct info *)node->data)->value = malloc(value_size);
			memmove(((struct info *)node->data)->value, value, value_size);

			return;
		}
		node = node->next;
	}

	struct info data;

	data.key = malloc(key_size);
	memmove(data.key, key, key_size);
	
	data.value = malloc(value_size);
	memmove(data.value, value, value_size);

	++ht->size;

	ll_add_nth_node(bucket, 0, &data);
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key) % ht->hmax;

	linked_list_t *bucket = ht->buckets[index];

	ll_node_t *node = bucket->head;

	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (ht->compare_function(key, ((struct info *)node->data)->key) == 0) {
			return ((struct info *)node->data)->value;
		}
		node = node->next;
	}

	return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int
ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key) % ht->hmax;

	linked_list_t *bucket = ht->buckets[index];

	ll_node_t *node = bucket->head;

	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (ht->compare_function(key, ((struct info *)node->data)->key) == 0) {
			return 1;
		}
		node = node->next;
	}

	return 0;
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o intrare din hashtable (adica memoria
 * pentru copia lui key --vezi observatia de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int index = ht->hash_function(key) % ht->hmax;

	linked_list_t *bucket = ht->buckets[index];

	ll_node_t *node = bucket->head;

	for (unsigned int i = 0; i < bucket->size; ++i) {
		if (ht->compare_function(key, ((struct info *)node->data)->key) == 0) {
			ll_node_t *removed_entry = ll_remove_nth_node(bucket, i);
			free(((struct info *)removed_entry->data)->key);
			free(((struct info *)removed_entry->data)->value);
			free(removed_entry->data);
			free(removed_entry);
			--ht->size;
			return;
		}
		node = node->next;
	}
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void
ht_free(hashtable_t *ht)
{
	for (unsigned int i = 0; i < ht->hmax; ++i) {
		linked_list_t *bucket = ht->buckets[i];

		unsigned int size = bucket->size;
		for (unsigned int j = 0; j < size; ++j) {
			ll_node_t *removed_entry = ll_remove_nth_node(bucket, 0);

			free(((struct info *)removed_entry->data)->key);
			free(((struct info *)removed_entry->data)->value);
			free((struct info *)removed_entry->data);
			free(removed_entry);
		}

		free(bucket);
	}

	free(ht->buckets);
    free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
