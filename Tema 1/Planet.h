// Copyright 2021 Prigoreanu Andreea 311CA

#ifndef PLANET_H_
#define PLANET_H_

#include "./CircularDoublyLinkedList.h"
#include "./utils.h"


typedef struct planet_t planet_t;
struct planet_t
{
    /* numele planetei */
    char *name;
    /* numarul de planete distruse */
    int destroyed_planets;
    /* lista de scuturi */
    doubly_linked_list_t *shields;
};

/*
 * Implementarea funtiilor urmatoare se va face in Planet.c .
 */

planet_t *
create_planet(char *name, unsigned int no_shields);

void
free_planet(planet_t **pp_planet);
#endif  // PLANET_H_
