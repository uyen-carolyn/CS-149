#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <memory.h>
#include <pthread.h>
#include "prodcon.h"

#define MAX_SIZE 10

// code based of initial prodcon.c code 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; 
pthread_cond_t fill = PTHREAD_COND_INITIALIZER; 

struct llist_node {
    struct llist_node *next;
    char *str;
};

/**
 * pop a node off the start of the list.
 *
 * @param phead the head of the list. this will be modified by the call unless the list is empty
 * (*phead == NULL).
 * @return NULL if list is empty or a pointer to the string at the top of the list. the caller is
 * incharge of calling free() on the pointer when finished with the string.
 */
char *pop(struct llist_node **phead)
{
    //pthread_mutex_lock(&mutex); 
    if (*phead == NULL) {
        return NULL;
    }
    char *s = (*phead)->str;
    struct llist_node *next = (*phead)->next;
    free(*phead);
    *phead = next;
    //pthread_mutex_unlock(&mutex); 
    return s;
}

/**
 * push a node onto the start of the list. a copy of the string will be made.
 * @param phead the head of the list. this will be modified by this call to point to the new node
 * being added for the string.
 * @param s the string to add. a copy of the string will be made and placed at the beginning of
 * the list.
 */
void push(struct llist_node **phead, const char *s)
{
//    pthread_mutex_lock(&mutex); 
    struct llist_node *new_node = malloc(sizeof(*new_node));
    new_node->next = *phead;
    new_node->str = strdup(s);
    *phead = new_node;
//    pthread_mutex_unlock(&mutex); 
}

// the array of list heads. the size should be equal to the number of consumers
static struct llist_node **heads;

static assign_consumer_f assign_consumer;
static int producer_count;
static int consumer_count;

int loops; 
int done = 0; 

 

//int buffer[MAX]; 
int fill_ptr = 0; 
int use_ptr = 0; 
int count = 0; 
int thread_count = 4; 

static int my_consumer_number;

void queue(int consumer, const char *str)
{
//    pthread_mutex_lock(&mutex); 
    push(&heads[consumer], str);
//    pthread_mutex_unlock(&mutex); 
}

void produce(const char *buffer)
{
//    pthread_mutex_lock(&mutex); 
    int hash = assign_consumer(consumer_count, buffer);
    queue(hash, buffer);
//    pthread_mutex_unlock(&mutex); 
}

char *consume() {
    pthread_mutex_lock(&mutex); 
    char *str = pop(&heads[my_consumer_number]);
    pthread_mutex_unlock(&mutex); 
    return str;
}

void do_usage(char *prog)
{
    printf("USAGE: %s shared_lib consumer_count producer_count ....\n", prog);
    exit(1);
}

struct info {
    int id; 
};

// from class notes
void func(void *v) {
    pthread_mutex_lock(&mutex); 
    struct info *x = v; 
    my_consumer_number = x->id; 
    
    pthread_cond_wait(&fill, &mutex);
    
    pthread_mutex_unlock(&mutex); 
}

// not sure to use this or func()

//void *create_producers(void *arg) {
//    int loops = (int) arg; 
    
//    for (int i = 0; i < loops; i++) {
//        pthread_mutex_lock(&mutex); 
//        while (count == MAX_SIZE) {
//            pthread_cond_wait(&empty, &mutex);
//        }
//        put(i); 
//        pthread_cond_signal(&fill); 
//        pthread_mutex_unlock(&mutex);
//    }
//}

//void *create_consumers(void *arg) {
//    int loops = (int) arg; 
    
//    for (int i = 0; i < loops; i++) {
//        pthread_mutex_lock(&mutex);
//        while (count == 0) {
//            pthread_cond_wait(&fill, &mutex); 
//        }
        
//        int tmp = get();
//        pthread_cond_signal(&empty); 
//        pthread_mutex_unlock(&mutex); 
//    }
//}

int main(int argc, char **argv)
{
    if (argc < 4) {
        do_usage(argv[0]);
    }

    char *shared_lib = argv[1];
    producer_count = strtol(argv[2], NULL, 10);
    consumer_count = strtol(argv[3], NULL, 10);
    
    
//    printf("Multi-threading\n");
    struct info p[producer_count];
    struct info c[consumer_count];
    
    pthread_t threads_p[producer_count];
    pthread_t threads_c[consumer_count];
 
    // use this or the other thing? 
//    for (int i = 0; i < producer_count; i++) {
//        p[i].id = i; 
//        pthread_create(&threads_p[i], NULL, func, &p[i]); 
//    }
    
//    for (int i = 0; i < consumer_count; i++) {
//        pthread_create(&threads_c[i], NULL, func, &c[i]); 
//    }
    

//    printf("Single Threading\n");
    char **new_argv = &argv[4];
    int new_argc = argc - 4;
    setlinebuf(stdout);

    if (consumer_count <= 0 || producer_count <= 0) {
        do_usage(argv[0]);
    }

    void *dh = dlopen(shared_lib, RTLD_LAZY);

    // load the producer, consumer, and assignment functions from the library
    run_producer_f run_producer = dlsym(dh, "run_producer");
    run_consumer_f run_consumer = dlsym(dh, "run_consumer");
    assign_consumer = dlsym(dh, "assign_consumer");
    if (run_producer == NULL || run_consumer == NULL || assign_consumer == NULL) {
        printf("Error loading functions: prod %p cons %p assign %p\n", run_producer,
                run_consumer, assign_consumer);
        exit(2);
    }

    heads = calloc(consumer_count, sizeof(*heads));

    for (int i = 0; i < producer_count; i++) {
        p[i].id = i; 
        pthread_create(&threads_p[i], NULL, func, &p[i]); 
        run_producer(i, producer_count, produce, new_argc, new_argv);
    }

    for (int i = 0; i < consumer_count; i++) {
        my_consumer_number = i;
        pthread_create(&threads_c[i], NULL, func, &c[i]); 
        run_consumer(i, consume, new_argc, new_argv);
    }
    
    //sentinel loop from linkedlist.c
    
    int sentinel_seen = 0; 
    int found = 0; 
    
    while (sentinel_seen != thread_count) {   // is it consumer or thread count
        if(pop == count) {      // which count is it...
            sentinel_seen++;
        }
        else {
            found++; 
        }
    }

    return 0;
}
