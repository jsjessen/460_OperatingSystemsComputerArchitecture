// James Jessen
// 10918967

#ifndef __STRING_H__
#define __STRING_H__


int strlen(char* str);

int strcmp(char* str1, char* str2);
int strncmp(char* str1, char* str2, int n);

char* strcpy(char* dest, char* source);
char* strncpy(char* dest, char* source, int n);

int isspace(int c);
int islower(int c);
int isupper(int c);
int isalpha(int c);
int isdigit(int c);
int isalnum(int c);

int atoi(char* str);


#endif
