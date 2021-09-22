// Copyright 2021 Prigoreanu Andreea 311CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./GalacticWar.h"

#define NMAX 21

int main()
{
    /* Se citeste numarul de comenzi */
    int no_commands;
    scanf("%d", &no_commands);

    /* Se creeaza o lista circulara dublu inlantuita numita galaxy  */
    doubly_linked_list_t *galaxy = dll_create(sizeof(planet_t));

    for (int i = 0; i < no_commands; ++i) {
        char command[4];
        scanf("%s", command);

        if (strcmp(command, "ADD") == 0) {
            char name[NMAX];
            unsigned int index_planet, no_shields;
            scanf("%s%u%u", name, &index_planet, &no_shields);

            add(galaxy, name, index_planet, no_shields);
        }

        if (strcmp(command, "BLH") == 0) {
            unsigned int index_planet;
            scanf("%u", &index_planet);

            black_hole(galaxy, index_planet);
        }

        if (strcmp(command, "UPG") == 0) {
            unsigned int index_planet, index_shield;
            int value;
            scanf("%u%u%d", &index_planet, &index_shield, &value);

            upgrade_shield(galaxy, index_planet, index_shield, value);
        }

        if (strcmp(command, "EXP") == 0) {
            unsigned int index_planet;
            int value;
            scanf("%u%d", &index_planet, &value);

            expand_shield(galaxy, index_planet, value);
        }

        if (strcmp(command, "RMV") == 0) {
            unsigned int index_planet, index_shield;
            scanf("%u%u", &index_planet, &index_shield);

            remove_shield(galaxy, index_planet, index_shield);
        }

        if (strcmp(command, "COL") == 0) {
            unsigned int index_planet1, index_planet2;
            scanf("%u%u", &index_planet1, &index_planet2);

            collide(galaxy, index_planet1, index_planet2);
        }

        if (strcmp(command, "ROT") == 0) {
            unsigned int index_planet, units;
            char direction;
            scanf("%u", &index_planet);
            scanf("%c", &direction);
            scanf("%c", &direction);
            scanf("%u", &units);

            rotate_shield(galaxy, index_planet, direction, units);
        }

        if (strcmp(command, "SHW") == 0) {
            unsigned int index_planet;
            scanf("%u", &index_planet);

            show_info(galaxy, index_planet);
        }
    }

    /* Se elibereaza memoria alocata dinamic */
    free_galaxy(galaxy);
    free(galaxy);

    return 0;
}
