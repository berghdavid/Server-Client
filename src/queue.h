#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

/* Standard linked list element */
struct 
node {
    struct node *next;
    int value;
};

struct 
queue {
    struct node *head;  /* Dequeue this next */
    struct node *tail;  /* Enqueue after this */
    pthread_mutex_t lock;
};

/* Create a new empty queue */
struct 
queue * queueCreate(void)
{
    struct queue *q;

    q = (struct queue *) malloc(sizeof(struct queue));

    q->head = q->tail = NULL;


    if(pthread_mutex_init(&q->lock, NULL) != 0) {
        printf("Error initializing queue lock.");
    }

    return q;
}

/* Add a new value to back of queue */
void 
enq(struct queue *q, int value)
{
    pthread_mutex_lock(&q->lock);

    struct node *e;

    e = (struct node *) malloc(sizeof(struct node));
    assert(e);

    e->value = value;

    /* Because I will be the tail, nobody is behind me */
    e->next = NULL;

    if(q->head == NULL) {
        /* If the queue was empty, I become the head */
        q->head = e;
    } else {
        /* Otherwise I get in line after the old tail */
        q->tail->next = e;
    }

    /* I become the new tail */
    q->tail = e;

    pthread_mutex_unlock(&q->lock);
}

int
queueEmpty(const struct queue *q)
{
    return (q->head == NULL);
}

/* Remove and return value from front of queue */
int
deq(struct queue *q)
{
    pthread_mutex_lock(&q->lock);
    int ret;
    struct node *e;

    assert(q->head != NULL);

    ret = q->head->value;

    /* Patch out first element */
    e = q->head;
    q->head = e->next;

    free(e);

    pthread_mutex_unlock(&q->lock);

    return ret;
}

/* Print contents of queue on a single line, head first */
void
queuePrint(struct queue *q)
{
    pthread_mutex_lock(&q->lock);
    
    struct node *e;

    for(e = q->head; e != 0; e = e->next) {
        printf("%d ", e->value);
    }
    putchar('\n');

    pthread_mutex_unlock(&q->lock);
}

/* Free a queue and all of its elements */
void
queueDestroy(struct queue *q)
{
    pthread_mutex_lock(&q->lock);
    while(!queueEmpty(q)) {
        deq(q);
    }

    free(q);
    pthread_mutex_unlock(&q->lock);
}