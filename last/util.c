#include "util.h"

typedef struct cache 
{
    char buf[BUF_SIZE];
    char* cp;
    int fd;
    int size; 
}CACHE;

static CACHE cache;

// Read up to size bytes from fd until reach newline (include)
// lineptr will end up containing the line
// Return the number of bytes added to lineptr
// If there are any errors, return -1
// Note it does not null terminate
int getline(char* lineptr, int size, int fd)
{
    char* cp = lineptr;
    int nbytes = 0;
    STAT stat;

    if(fd < 0 || fstat(fd, &stat) < 0)
    {
        printf("Error getline: Bad file descriptor\n");
        return FAILURE;
    }

    while(nbytes < size)
    {
        // Fill the cache if it is for a different file or it is empty
        if(fd != cache.fd || cache.size == 0) 
        {
            cache.size = read(fd, cache.buf, BUF_SIZE);
            cache.cp = cache.buf;
            cache.fd = fd;
        }

        // Check for end of file
        if(*cache.cp == EOF || cache.size <= 0)
        {
            cache.size = -1;
            break;
        }

        *cp = *cache.cp;
        nbytes++;
        cache.cp++;
        cache.size--;

        // Check for end of line
        if(*cp++ == '\n')
            break;
    } 

    return nbytes;
}
