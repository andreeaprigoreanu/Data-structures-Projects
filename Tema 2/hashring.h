/* Copyright 2021 <Andreea Prigoreanu> */

#ifndef HASHRING_H_
#define HASHRING_H_

/**
 * find_position() - Returneaza pozitia primei etichete de server cu hashul mai
                    mare sau egal cu hashul dat ca parametru.
 * @arg1: Numarul de servere(incluzand replicile).
 * @arg2: Pointer la un vector circular in care sunt stocate etichete
            serverelor.
 * @arg3: Pointer la functia de hash pentru servere.
 * @arg4: Hash-ul valorii cautate.
 */
unsigned int find_position(unsigned int num_servers, unsigned int *hashring,
                      unsigned int (*hash_function_servers)(void*),
                      unsigned int hash_value);

/**
 * add_element() - Insereaza pe pozitia data eticheta serverului.
 * @arg1: Pointer la numarul de servere(incluzand replicile).
 * @arg2: Vector circular in care sunt stocate etichete serverelor.
 * @arg3: Valoarea etichetei inserate.
 * @arg4: Pozitia etichetei care trebuie insetata.
 */
void add_element(unsigned int *num_servers, unsigned int *hashring,
                 unsigned int value, unsigned int pos);

/**
 * delete_element() - Sterge eticheta serverului pe pozitia data.
 * @arg1: Pointer la numarul de servere(incluzand replicile).
 * @arg2: Vector circular in care sunt stocate etichete serverelor.
 * @arg3: Valoarea etichetei care trebuie stearsa.
 * @arg4: Pozitia etichetei care trebuie stearsa.
 */
void delete_element(unsigned int *num_servers, unsigned int *hashring,
                    unsigned int pos);

#endif  /* HASHRING_H_ */
