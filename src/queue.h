#ifndef HEADER_QUEUE
#define HEADER_QUEUE

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
queue * queueCreate(void);

/* Add a new value to back of queue */
void 
enq(struct queue *q, int value);

int
queueEmpty(const struct queue *q);

/* Remove and return value from front of queue */
int
deq(struct queue *q);

/* Print contents of queue on a single line, head first */
void
queuePrint(struct queue *q);

/* Free a queue and all of its elements */
void
queueDestroy(struct queue *q);

#endif
