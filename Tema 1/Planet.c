// Copyright 2021 Prigoreanu Andreea 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Planet.h"

/*
 * Functie pentru alocarea si initializarea unei structuri de tip planeta
 * (Setare valori initiale pentru campurile specifice structurii planet_t).
 */
planet_t*
create_planet(char *name, unsigned int no_shields) {
    planet_t *planet = (planet_t *)malloc(sizeof(planet_t));
    DIE(!planet, "malloc failed\n");


    planet->name = malloc(sizeof(char) * strlen(name) + 1);
    memcpy(planet->name, name, strlen(name) + 1);
    DIE(!name, "malloc failed!\n");

    planet->destroyed_planets = 0;

    planet->shields = dll_create(sizeof(int));

    int data = 1;
    for (unsigned int i = 0; i < no_shields; ++i) {
        dll_add_nth_node(planet->shields, i, &data);
    }

    return planet;
}

/*
 * Functia elibereaza memoria folosita de campurile unei
 * structuri de tip planet_t, iar la sfarsit elibereaza memoria folosita de
 * structura planet_t.
 */
void
free_planet(planet_t **pp_planet) {
    free((*pp_planet)->name);
    dll_free(&(*pp_planet)->shields);
    free((*pp_planet));
}
