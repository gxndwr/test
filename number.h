struct random_seed {
    struct random_seed *next;
    struct random_seed *prev;
    int seed;
};

#define RADOM_SEED_POOL_SIZE 10

void print_random_seed_content(void);
void initialize_random_seed_poll(void);
int get_random_digits(int mod);

extern struct random_seed *head_seed;
extern struct random_seed *enqueue_seed;
extern struct random_seed *dequeue_seed;
extern struct random_seed first_seed;
