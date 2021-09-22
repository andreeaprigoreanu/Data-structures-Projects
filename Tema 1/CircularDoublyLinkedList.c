// Copyright 2021 Prigoreanu Andreea 311CA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./CircularDoublyLinkedList.h"
#include "./utils.h"

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
    DIE(!list, "malloc failed\n");

    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;

    return list;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere? Daca n < 0, eroare.
 */
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    n = n % list->size;

    dll_node_t *curr = list->head;
    unsigned int index = 0;

    while (index < n) {
        curr = curr->next;
        index++;
    }

    return curr;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei. Daca n < 0, eroare.
 */
void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data)
{
    if (n > list->size) {
        n = list->size;
    }

    dll_node_t *newNode = malloc(sizeof(dll_node_t));
    DIE(!newNode, "malloc failed\n");

    newNode->data = malloc(list->data_size);
    DIE(!newNode->data, "malloc failed\n");

    memcpy(newNode->data, data, list->data_size);

    if (n == 0) {
        if (!list->head) {
            list->head = newNode;
            newNode->prev = newNode->next = newNode;
        } else {
            newNode->prev = list->head->prev;
            newNode->prev->next = newNode;
            newNode->next = list->head;
            list->head->prev = newNode;
            list->head = newNode;
        }
    } else {
        dll_node_t *it = dll_get_nth_node(list, n - 1);
        newNode->next = it->next;
        newNode->next->prev = newNode;
        newNode->prev = it;
        it->next = newNode;
    }

    ++list->size;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Daca n < 0, eroare. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    if (!list || !list->head) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    if (n == 0) {
        if (list->size == 1) {
            dll_node_t *node = list->head;
            list->head = NULL;
            node->next = node->prev = NULL;
            --list->size;
            return node;
        } else {
            dll_node_t *node = list->head;
            node->next->prev = node->prev;
            node->prev->next = node->next;
            list->head = node->next;
            node->next = node->prev = NULL;
            --list->size;
            return node;
        }
    } else {
        dll_node_t *node = dll_get_nth_node(list, n);
        node->next->prev = node->prev;
        node->prev->next = node->next;
        node->next = node->prev = NULL;
        --list->size;
        return node;
    }
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void
dll_free(doubly_linked_list_t** pp_list)
{
    if (!(*pp_list)) {
        return;
    }
    dll_node_t *curr;
    while ((*pp_list)->head) {
        curr = dll_remove_nth_node(*pp_list, 0);
        free(curr->data);
        free(curr);
    }
    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista separate printr-un spatiu, incepand de la primul nod din lista.
 */
void
dll_print_int_list(doubly_linked_list_t* list)
{
    if (!list || !list->head) {
        return;
    }

    dll_node_t *curr = list->head;
    for (unsigned int i = 0; i < list->size; ++i) {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    }
    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista separate printr-un spatiu, incepand de la primul nod din
 * lista.
 */
void
dll_print_string_list(doubly_linked_list_t* list)
{
    if (!list || !list->head) {
        return;
    }

    dll_node_t *curr = list->head;
    for (unsigned int i = 0; i < list->size; ++i) {
        printf("%s ", (char *)curr->data);
        curr = curr->next;
    }
    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza o singura data toate valorile int
 * stocate in nodurile din lista, separate printr-un spatiu, incepand de la
 * nodul dat ca parametru si continuand la stanga in lista dublu inlantuita
 * circulara, pana cand sunt afisate valorile tuturor nodurilor.
 */
void
dll_print_ints_left_circular(dll_node_t* start)
{
    if (!start) {
        return;
    }

    dll_node_t *curr = start;
    do {
        printf("%d ", *((int *)curr->data));
        curr = curr->prev;
    } while (curr != start);

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza o singura data toate valorile int
 * stocate in nodurile din lista, separate printr-un spatiu, incepand de la
 * nodul dat ca parametru si continuand la dreapta in lista dublu inlantuita
 * circulara, pana cand sunt afisate valorile tuturor nodurilor.
 */
void
dll_print_ints_right_circular(dll_node_t* start)
{
    if (!start) {
        return;
    }

    dll_node_t *curr = start;
    do {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    } while (curr != start);
    printf("\n");
}


/*
 * Functia adauga valoarea value nodului de pe pozitia n din lista.
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri
 * stocheaza int-uri.
 */
void
dll_upgrade_int_list(doubly_linked_list_t* list, unsigned int n, int value) {
    if (!list || !list->head) {
        return;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    dll_node_t *curr = dll_get_nth_node(list, n);

    *((int *)curr->data) += value;
}

/*
 * Functia roteste lista data in sens trigonometric cu un numar de unitati.
 * Lista este circulara dublu inlantuita, asa ca pentru a roti lista este
 * suficenta mutarea pointerului head pe pozitia egala cu restul impartirii
 * numarului de unitati la numarul de noduri al listei.
 */
void
dll_rotate_trig(doubly_linked_list_t *list, unsigned int units)
{
    units = units % list->size;
    dll_node_t *curr = dll_get_nth_node(list, units);

    list->head = curr;
}

/*
 * Functia roteste lista data in sesul acelor de ceasornic cu un numar de
 * unitati.
 * Lista este circulara dublu inlantuita, asa ca pentru a roti lista este
 * suficenta mutarea pointerului head pe pozitia egala cu diferenta dintre
 * numarul de noduri al listei si restul impartirii numarului de unitati la
 * numarul de noduri al listei.
 */
void
dll_rotate_clk(doubly_linked_list_t *list, unsigned int units)
{
    units = units % list->size;
    dll_node_t *curr = dll_get_nth_node(list, list->size - units);

    list->head = curr;
}
