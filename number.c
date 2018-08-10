#include <stdio.h>
#include <stdlib.h>
#include "number.h"

struct random_seed *head_seed;
struct random_seed *enqueue_seed;
struct random_seed *dequeue_seed;
struct random_seed first_seed;

void print_random_seed_content(void)
{
    struct random_seed *p;
    printf("\nprint all linked list items:\n");
    p = head_seed;
    while(p->next != head_seed) {
        printf("p->prev: %p\n", p->prev);
        printf("p      : %p\n", p);
        printf("p->seed      : %d\n", p->seed);
        printf("p->next: %p\n", p->next);
        p = p->next;
        printf("-------------\n");
    }
    printf("p->prev: %p\n", p->prev);
    printf("p      : %p\n", p);
    printf("p->seed      : %d\n", p->seed);
    printf("p->next: %p\n", p->next);
}

void initialize_random_seed_poll(void)
{
    int i, ret;
    struct random_seed *p;

    head_seed = &first_seed;
    enqueue_seed = head_seed;
    dequeue_seed = head_seed;
    head_seed->next = &first_seed;
    head_seed->prev = &first_seed;

    //dbg("struct size:%lu\n", sizeof(struct random_seed));
    for (i=0; i<RADOM_SEED_POOL_SIZE; i++) {
        p = malloc(sizeof(struct random_seed));
        p->seed = i*i; /* make default digit hard to predict */
        p->next = head_seed;
        p->prev = head_seed->prev;
        head_seed->prev->next = p;
        head_seed->prev = p;
        head_seed = p;
    }
}

int get_random_digits(int mod)
{
    int digit;

	srand(dequeue_seed->prev->seed);
    dequeue_seed = dequeue_seed->next;
	digit = rand()%mod;
    //dbg("\nrandom seed:%d\n", dequeue_seed->seed);
    //dbg("\nrandom digit: %d\n", digit);
    return digit;
}
