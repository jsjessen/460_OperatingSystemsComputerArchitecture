// James Jessen
// 10918967

#ifndef __LIST_H__
#define __LIST_H__


#include "type.h"

void enlist(PROC** list, PROC* new);
PROC* delist(PROC** list);
void printList(char* name, PROC* list);


#endif
