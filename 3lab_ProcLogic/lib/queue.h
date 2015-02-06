// James Jessen
// 10918967

#ifndef __QUEUE_H__
#define __QUEUE_H__


#include "io.h"
#include "../type.h"

void enqueue(PROC** queue, PROC* new);
PROC* dequeue(PROC** queue);
void printQueue(char* name, PROC* queue);


#endif
