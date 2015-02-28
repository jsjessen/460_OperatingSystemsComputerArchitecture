// James Jessen
// 10918967
// CptS 460

// ------------------------------------------ 
// MTX Kernel Mode
// ------------------------------------------ 

#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "type.h"
#include "lib/io.h"
#include "lib/queue.h"
#include "lib/list.h"
#include "lib/transfer.h"

#define NPROC 9        

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEPING, ZOMBIE } status_t;
typedef enum { FAILURE=-1, SUCCESS=0 } result_t;
typedef enum { false, true } bool;

extern PROC proc[], *running, *freeList, *sleepList, *readyQueue;
extern char* states[];
extern char* pname[];
extern int procSize;
extern int nproc;
extern int color;

// main.c
int body();
void scheduler();

// ts.s
int int80h();
int tswitch(); // TEST if it returns an int
int goUmode();

// kernel.c
PROC *kfork(char* filename);
int kexit(u16 exitValue);

// wait.c
void ksleep(int event);
void kwakeup(int event);
int kwait(int* status);

// vkernel.c
void do_ps();
void do_tswitch();
int do_kfork(char* filename);
int do_exit();
void do_sleep();
void do_wakeup();
void do_wait();

// int.c
int kcinth();
int kgetpid();
int kps();
int kchname(char* y);
int kkfork();
int ktswitch();
int kkwait(int *status);
int kkexit(int value);
int kgetc();
int kputc(char c);


#endif
