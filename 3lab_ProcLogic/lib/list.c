#include "list.h"

// Put process at end of free list
void enlist(PROC** list, PROC* new)
{
    PROC* p = *list;

    // If empty list, create frist element
    if(!(*list))
    {
        *list = new;
        new->next = NULL;
        return;
    }

    // Go to end of list
    while(p->next) 
        p = p->next;

    p->next = new;
}

// Get first free process
PROC* delist(PROC** list, PROC* removeMe)
{
    PROC* p = *list;

    if(*list != NULL && removeMe == *list)
        *list = (*list)->next; // First item
    else
    {
        // Search for proc
        while(p && p->next != removeMe)
            p = p->next;

        // Not in list 
        if(!p) return NULL;

        // p->next == removeMe
        p->next = removeMe->next; 
    }

    removeMe->next = NULL;
    return removeMe;
}

// Print contents of list 
void printList(char* name, PROC* list)
{
    PROC* p = list;

    printf("%s = ", name);
    while(p) 
    {
        // [PID, Priority]
        printf("[%d, %d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("0\n");
}
