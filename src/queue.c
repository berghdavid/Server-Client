#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "queue.h"

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

void 
enq(struct queue *q, int value)
{
    struct node *e;

    e = (struct node *) malloc(sizeof(struct node));
    assert(e);

    e->value = value;
    e->next = NULL;

    pthread_mutex_lock(&q->lock);

    if(q->head == NULL) {
        q->head = e;
    } else {
        q->tail->next = e;
    }
    q->tail = e;

    pthread_mutex_unlock(&q->lock);
}

int
queueEmpty(const struct queue *q)
{
    return (q->head == NULL);
}

int
deq(struct queue *q)
{
    int ret;
    struct node *e;

    pthread_mutex_lock(&q->lock);
    assert(q->head != NULL);

    ret = q->head->value;

    /* Patch out first element */
    e = q->head;
    q->head = e->next;
    free(e);

    pthread_mutex_unlock(&q->lock);

    return ret;
}

void
queuePrint(struct queue *q)
{
    struct node *e;

    pthread_mutex_lock(&q->lock);

    for(e = q->head; e != 0; e = e->next) {
        printf("%d ", e->value);
    }
    putchar('\n');

    pthread_mutex_unlock(&q->lock);
}

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