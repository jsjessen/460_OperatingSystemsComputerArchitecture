// James Jessen
// 10918967
// CptS 460

// ------------------------------------------ 
// MTX User Mode
// ------------------------------------------ 

#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "../type.h"
#include "../lib/io.h"

#define LEN 64

extern char *cmd[];

int show_menu();
int find_cmd(char *name);
int getpid();
int ps();
int chname();
int kfork();
int kswitch();
int wait();
int geti_(); // lib or this?
int exit();
;
// syscalls;
int _exit(int exitValue);
int getc_();
int putc_(char c);
;
int invalid(char *name);

#endif
