#ifndef __UTIL_H__
#define __UTIL_H__

#include "type.h"
#include "ucode.h"

#define BUF_SIZE 1024

int getline(char* lineptr, int size, int fd);
void perror(char* file_name, char* function_name, char* error_msg);
void print_buf(int fd, char* msg);

#endif
