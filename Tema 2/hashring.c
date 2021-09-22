// Copyright 2021 Andreea Prigoreanu

#include <stdio.h>

#include "hashring.h"


unsigned int find_position(unsigned int num_servers, unsigned int *hashring,
                      unsigned int (*hash_function_servers)(void*),
                      unsigned int hash_value)
{
    unsigned int pos = 0;

    while (pos < num_servers &&
           hash_function_servers(&hashring[pos]) < hash_value) {
        ++pos;
    }

    return pos;
}

void add_element(unsigned int *num_servers, unsigned int *hashring,
                 unsigned int value, unsigned int pos)
{
    ++(*num_servers);

    for (unsigned int i = *num_servers - 1; i > pos; --i) {
        hashring[i] = hashring[i - 1];
    }
    hashring[pos] = value;
}

void delete_element(unsigned int *num_servers, unsigned int *hashring,
                    unsigned int pos)
{
    for (unsigned int i = pos; i < *num_servers; ++i) {
        hashring[i] = hashring[i + 1];
    }

    --(*num_servers);
}
