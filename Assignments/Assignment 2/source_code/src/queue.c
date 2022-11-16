#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
	return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
	/* TODO: put a new process to queue [q] */
	// If queue is full -> ERROR
	if (q->size == MAX_QUEUE_SIZE)
		return;
	// If queue is not full -> SUCCESS
	q->proc[q->size++] = proc;
}

struct pcb_t *dequeue(struct queue_t *q)
{
	/* TODO: return a pcb whose priority is the highest
	 * in the queue [q] and remember to remove it from q
	 * */
	// If queue is empty, ERROR
	if (q->size <= 0)
		return NULL;
	// If queue is not empty, SUCCESS
	int highestPriority = 0;
	int pos = -1;
	// find the highest priority of a pcb
	for (int i = 0; i < q->size; i++)
	{
		if (highestPriority < q->proc[i]->priority)
		{
			highestPriority = q->proc[i]->priority;
			pos = i;
		}
	}
	if (pos == -1)
		return NULL;
	// delete that pcb
	struct pcb_t *qtemp = q->proc[pos];
	for (int i = pos; i < q->size - 1; i++)
	{
		q->proc[i] = q->proc[i + 1];
	}
	// decrease size of q
	q->size--;
	return qtemp;
}

