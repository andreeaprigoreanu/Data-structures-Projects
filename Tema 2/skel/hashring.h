#ifndef HASHRING_H_
#define HASHRING_H_

unsigned int find_position(unsigned int num_servers, unsigned int *hashring,
                      unsigned int (*hash_function_servers)(void*),
                      unsigned int hash_value);

void add_element(unsigned int *num_servers, unsigned int *hashring,
                 unsigned int value, unsigned int pos);

void delete_element(unsigned int *num_servers, unsigned int *hashring,
                    unsigned int pos);

#endif  /* HASHRING_H_ */
