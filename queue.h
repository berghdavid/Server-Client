#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* standard linked list element */
struct 
node {
    struct node *next;
    int value;
};

struct 
queue {
    struct node *head;  /* dequeue this next */
    struct node *tail;  /* enqueue after this */
};

/* create a new empty queue */
struct 
queue * queueCreate(void)
{
    struct queue *q;

    q = (struct queue *) malloc(sizeof(struct queue));

    q->head = q->tail = NULL;

    return q;
}

/* add a new value to back of queue */
void 
enq(struct queue *q, int value)
{
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
}

int
queueEmpty(const struct queue *q)
{
    return (q->head == NULL);
}

/* remove and return value from front of queue */
int
deq(struct queue *q)
{
    int ret;
    struct node *e;

    assert(!queueEmpty(q));

    ret = q->head->value;

    /* patch out first element */
    e = q->head;
    q->head = e->next;

    free(e);

    return ret;
}

/* print contents of queue on a single line, head first */
void
queuePrint(struct queue *q)
{
    struct node *e;

    for(e = q->head; e != 0; e = e->next) {
        printf("%d ", e->value);
    }
    
    putchar('\n');
}

/* free a queue and all of its elements */
void
queueDestroy(struct queue *q)
{
    while(!queueEmpty(q)) {
        deq(q);
    }

    free(q);
}

/*
int
main(int argc, char **argv)
{
    int i;
    struct queue *q;

    q = queueCreate();

    for(i = 0; i < 5; i++) {
        printf("enq %d\n", i);
        enq(q, i);
        queuePrint(q);
    }

    while(!queueEmpty(q)) {
        printf("deq gets %d\n", deq(q));
        queuePrint(q);
    }

    queueDestroy(q);

    return 0;
}*/