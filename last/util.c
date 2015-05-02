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

void print_buf(int fd, char* msg)
{
    write(fd, msg, strlen(msg));
}

void perror(char* file_name, char* function_name, char* error_msg)
{
    char buf[1024];
    sprintf(buf, " Error %s -> %s(): %s\n", file_name, function_name, error_msg);
    print_buf(STDERR, buf);
    exit(FAILURE);
}

/*
char* get_input()
{
    int i = 0;
    int size = 16;
    char* buf;

    if((buf = (char*)malloc(size)) == NULL)
    {
        perror("input.c: initial buf malloc");
        return NULL;
    }
    buf[0] = 0;

    // Loop through input 1 char at a time
    while(1)
    {
        int c = getchar();

        // Increase buffer size as needed
        if((i + 1) == size) 
        { 
            size += size;

            // Double Size
            if((buf = (char*)realloc(buf, size)) == NULL)
            {
                perror("input.c: buf realloc");
                return NULL;
            }
        }

        // Accept input or if no input, allow user to cycle
        if (c == '\n')
        {
            if (buf[0] == 0)
                return NULL;
            else
                break;
        }

        // Replace block of whitespace with single space
        if (isspace(c))
        {
            // Ignore leading whitespace
            if (buf[0] == 0)
                continue;

            while (isspace(c))  
                c = getchar();

            ungetc(c, STDIN);
            buf[i++] = ' ';
            continue;
        }

        buf[i++] = c;
    }
    buf[i] = 0; // Append with null char

    return buf;
}

char** parse(char* input, char* delimiters)
{
    int i = 0;
    int size = 8;
    char* tok;
    char** buf;

    if((buf = (char**)malloc(size * sizeof(char*))) == NULL)
    {
        perror("parse.c: initial ouput malloc");
        return NULL;
    }

    if((tok = strtok(input, delimiters)) == NULL)
        return NULL;

    while(tok)
    {
        int length = strlen(tok) + 1;
        if((buf[i] = (char*)malloc(length)) == NULL)
        {
            perror("parse.c: buf malloc for tok");
            return NULL;
        }
        strcpy(buf[i], tok);

        // Increase buffer size as needed
        if((i+1) == size)
        {
            size += size;

            // Double buffer size
            if((buf = (char**)realloc(buf, size * sizeof(char*))) == NULL)
            {
                perror("parse.c: buf realloc");
                return NULL;
            }
        }

        tok = strtok(NULL, delimiters);
        i++;
    }
    buf[i] = NULL;

    return buf;
}
*/
