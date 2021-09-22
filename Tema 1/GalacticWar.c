// Copyright 2021 Prigoreanu Andreea 311CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Planet.h"

/*
 * Functie pentru adaugarea unei noi planete pe pozitia n din galaxie.
 * Se aloca si se initializeaza o structura de tip planet_t cu numele si
 * numarul de scuturi primiti ca parametru de functie. Aceasta structura este
 * adaugata ca nod in lista galaxy.
 */
void
add(doubly_linked_list_t *galaxy, char *name, unsigned int n,
    unsigned int no_shields)
{
    if (n > galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    planet_t *planet_data = create_planet(name, no_shields);

    dll_add_nth_node(galaxy, n, planet_data);

    free(planet_data);

    printf("The planet %s has joined the galaxy.\n", name);
}

/*
 * Functia extrage nodul de tip planet_t de pe pozitia index_planet din
 * lista galaxy, iar apoi elibereaza memoria alocata pentru datele planetei si
 * pentru structura planeta.
 */
void
black_hole(doubly_linked_list_t *galaxy, unsigned int index_planet)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    dll_node_t *destroyed_planet = dll_remove_nth_node(galaxy, index_planet);
    planet_t *destroyed_planet_data = (planet_t *)destroyed_planet->data;

    printf("The planet %s has been eaten by the vortex.\n",
           destroyed_planet_data->name);

    free_planet(&destroyed_planet_data);
    free(destroyed_planet);
}

/*
 * Functia aduna value la  valoarea scutului de pe pozitia index_shield in
 * lista de scuturi a planetei de pe pozitia index_planet din lista galaxy.
 */
void
upgrade_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
               unsigned int index_shield, int value)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet */
    dll_node_t *planet = dll_get_nth_node(galaxy, index_planet);
    /* Se obtine un pointer la datele planetei de pe poztitia index_planet */
    planet_t *planet_data = (planet_t *)planet->data;

    if (index_shield >= planet_data->shields->size) {
        printf("Shield out of bounds!\n");
        return;
    }

    /* 
     * Se apeleaza functia dll_upgrade_int_list pentru scutul de pe pozitia
     * index_shield al listei de scuturi acorespunzatoare planetei de pe
     * pozitia index_planet.
     */
    dll_upgrade_int_list(planet_data->shields, index_shield, value);
}

/*
 * Functia adauga un nou scut cu valoarea value in lista de scuturi a planetei
 * de pe pozitia index_planet din lista galaxy.
 */
void
expand_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
              int value)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet */
    dll_node_t *planet = dll_get_nth_node(galaxy, index_planet);
    /* Se obtine un pointer la datele planetei de pe poztitia index_planet */
    planet_t *planet_data = (planet_t *)planet->data;

    /* 
     * Se apeleaza functia dll_add_nth_node pentru adaugarea unui scut in lista
     * de scuturi ale planetei de pe pozitia index_planet
     */
    dll_add_nth_node(planet_data->shields, planet_data->shields->size, &value);
}

/*
 * Functia elimina scutul de pe pozitia index_shield in lista de scuturi a
 * planetei de pe pozitia index_planet din lista galaxy.
 */
void
remove_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
    unsigned int index_shield)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet */
    dll_node_t *planet = dll_get_nth_node(galaxy, index_planet);
    /* Se obtine un pointer la datele planetei de pe poztitia index_planet */
    planet_t *planet_data = (planet_t *)planet->data;

    if (index_shield >= planet_data->shields->size) {
        printf("Shield out of bounds!\n");
        return;
    }

    if (planet_data->shields->size == 4) {
        printf("A planet cannot have less than 4 shields!\n");
        return;
    }

    /* Se obtine un pointer la scutul de pe pozitia index_shield */
    dll_node_t *shield = dll_remove_nth_node(planet_data->shields,
                                                 index_shield);
    /* Se elibereaza memoria alocata pentru scutul eliminat din lista */
    free(shield->data);
    free(shield);
}

/*
 * Functia extrage nodul de tip planet_t de pe pozitia index_planet din
 * lista galaxy, iar apoi elibereaza memoria alocata pentru datele planetei si
 * pentru structura planeta.
 */
static void
implode(doubly_linked_list_t *galaxy, unsigned int index_planet)
{
    dll_node_t *destroyed_planet = dll_remove_nth_node(galaxy, index_planet);
    planet_t *destroyed_planet_data = (planet_t *)destroyed_planet->data;

    free_planet(&destroyed_planet_data);
    free(destroyed_planet);
}

/*
 * Functia realizeaza coliziunea dintre planetele aflate pe pozitiile
 * index_planet1 si index_planet2 in galaxie.
 */
void
collide(doubly_linked_list_t *galaxy, unsigned int index_planet1,
        unsigned int index_planet2)
{
    if (index_planet1 >= galaxy->size || index_planet2 >= galaxy->size){
        printf("Planet out of bounds!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet1 */
    dll_node_t *planet1 = dll_get_nth_node(galaxy, index_planet1);
    /* Se obtine un pointer la datele planetei de pe poztitia index_planet2 */
    planet_t *planet1_data = (planet_t *)planet1->data;

    /* Se obtine un pointer la planeta de pe pozitia index_planet2 */
    dll_node_t *planet2 = dll_get_nth_node(galaxy, index_planet2);
    /* Se obtine un pointer la datele planetei de pe poztitia index_planet2 */
    planet_t *planet2_data = (planet_t *)planet2->data;

    /* Se obtine indecsii scuturilor de coliziune */
    unsigned int index_shield1 = planet1_data->shields->size / 4;
    unsigned int index_shield2 = planet2_data->shields->size / 4 * 3;

    /* Se obtine un pointer la scutul de pe pozitia index_shield1 */
    dll_node_t *planet1_shield = dll_get_nth_node(planet1_data->shields,
                                                    index_shield1);
    /* Se obtine un pointer la scutul de pe pozitia index_shield2 */
    dll_node_t *planet2_shield = dll_get_nth_node(planet2_data->shields,
                                                    index_shield2);

    /* Se actualizeaza valorile scuturilor dupa coliziune */
    upgrade_shield(galaxy, index_planet1, index_shield1, -1);
    upgrade_shield(galaxy, index_planet2, index_shield2, -1);

    if (*((int *)(planet1_shield->data)) == -1 &&
        *((int *)(planet2_shield->data)) == -1) {
        printf("The planet %s has imploded.\n", planet1_data->name);
        implode(galaxy, index_planet1);

        printf("The planet %s has imploded.\n", planet2_data->name);
        /* 
         * Pentru ca planeta de pe pozitia index_planet 1 a facut implozie,
         * noua pozitie a planetei 2 este index_planet2 - 1
         */
        implode(galaxy, index_planet2 - 1);
    } else {
        if (*((int *)(planet1_shield->data)) == -1) {
            printf("The planet %s has imploded.\n", planet1_data->name);
            implode(galaxy, index_planet1);
            ++planet2_data->destroyed_planets;
        } else {
            if (*((int *)(planet2_shield->data)) == -1) {
                printf("The planet %s has imploded.\n", planet2_data->name);
                implode(galaxy, index_planet2);
                ++planet1_data->destroyed_planets;
            }
        }
    }
}

/*
 * Functia realizeaza rotirea scutului planetei de pe pozitia index_planet
 * in sensul acelor de ceasornic sau un sens trigonometric(directia c sau t)
 * cu un numar de unitati units.
 */
void
rotate_shield(doubly_linked_list_t *galaxy, unsigned int index_planet,
              char direction, unsigned int units)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    if (direction != 'c' && direction != 't') {
        printf("Not a valid direction!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet */
    dll_node_t *planet = dll_get_nth_node(galaxy, index_planet);
    /* Se obtine un pointer la lista de scuturi ale planetei */
    doubly_linked_list_t *shields = ((planet_t *)planet->data)->shields;

    if (direction == 'c') {
        /* Se apeleaza functia dll_rotate_clk pentru rotirea in sensul acelor
         * de ceasornic cu un numar de unitati a listei de scuturi
         */
        dll_rotate_clk(shields, units);
    }
    if (direction == 't') {
        /* Se apeleaza functia dll_rotate_trig pentru rotirea in sens
         * trigonometric cu un numar de unitati a listei de scuturi
         */
        dll_rotate_trig(shields, units);
    }
}

/*
 * Functia afiseaza informatii despre planeta aflata pe pozitia index_planet
 * in galaxie: numele, planetele vecine, lista de scuturi si cate planete a
 * ucis.
 */
void
show_info(doubly_linked_list_t *galaxy, unsigned int index_planet)
{
    if (index_planet >= galaxy->size) {
        printf("Planet out of bounds!\n");
        return;
    }

    /* Se obtine un pointer la planeta de pe pozitia index_planet */
    dll_node_t *planet = dll_get_nth_node(galaxy, index_planet);

    /* Se afiseaza numele planetei: */
    printf("NAME: %s\n", ((planet_t *)planet->data)->name);

    /* Se afiseaza planetele vecine: */
    if (galaxy->size == 1) {
        /* In acest caz planeta contine o singura planeta */
        printf("Closest: none\n");
    } else  {
        if (galaxy->size == 2) {
            /* In acest caz planeta contine doua planete */
            printf("CLOSEST: %s\n", ((planet_t *)planet->prev->data)->name);
        } else {
            printf("CLOSEST: %s and %s\n",
                    ((planet_t *)planet->prev->data)->name,
                    ((planet_t *)planet->next->data)->name);
        }
    }

    /* Se lista de scuturi: */
    printf("SHIELDS: ");
    dll_print_int_list(((planet_t *)planet->data)->shields);

    /* Se afiseaza numarul de planete ucise: */
    printf("KILLED: %d\n", ((planet_t *)planet->data)->destroyed_planets);
}

/*
 * Functia elibereaza memoria folosita fiecare structura de tip planeta din
 * galaxie.
 */
void
free_galaxy(doubly_linked_list_t *galaxy)
{
    if (!galaxy) {
        return;
    }
    while (galaxy->head) {
        implode(galaxy, 0);
    }
}
