// Copyright 2021 Prigoreanu Andreea 311CA

#ifndef GALACTICWAR_H_
#define GALACTICWAR_H_

#include "./Planet.h"

/*
 * Implementarea funtiilor urmatoare se va face in GalacticWar.c .
 */

void
add(doubly_linked_list_t *galaxy, char *name, unsigned int n,
    unsigned int no_shields);

void
black_hole(doubly_linked_list_t *galaxy, unsigned int index_planet);

void
upgrade_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
    unsigned int index_shield, int value);

void
expand_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
              int value);

void
remove_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
    unsigned int index_shield);

void
collide(doubly_linked_list_t *galaxy, unsigned int index_planet1,
    unsigned int index_planet2);

void
rotate_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
              char direction, unsigned int units);

void
show_info(doubly_linked_list_t *galaxy, unsigned int index_planet);

void
free_galaxy(doubly_linked_list_t *galaxy);
#endif  // GALACTICWAR_H_
