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

#define NUM_KREG  9
#define NUM_UREG 12 

#define KB        1024
#define WORD_SIZE 2
#define REG_SIZE  2
#define PTR_SIZE  2

#define NAMELEN 32

// ----------------------------------------------------------------
//   |uDS|uES| di| si| bp| dx| cx| bx| ax|uPC|uCS|flag|NEXT segment
// ----------------------------------------------------------------
//    -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 | 
//                                                   End
#define UFLAG_FROM_END (-1  * REG_SIZE) // Flag
#define UCS_FROM_END   (-2  * REG_SIZE) // Code Segment
#define UES_FROM_END   (-11 * REG_SIZE) // Extra Segment
#define UDS_FROM_END   (-12 * REG_SIZE) // Data Segment

// ------------------------------------------------------------------------------
// |Code Data               |DS|ES|di|si|bp|dx|cx|bx|ax|PC|CS|flag|rPC|7|0|xxxxx| 
// --------------------------|---------------------------------------------------
//                           0  1  2  3  4  5  6  7  8  9  10  11   12
//                          usp    
#define UDS_FROM_USP   ( 0 * REG_SIZE)  // Data Segment
#define UES_FROM_USP   ( 1 * REG_SIZE)  // Extra Segment
#define UAX_FROM_USP   ( 8 * REG_SIZE)  // Return Register
#define UCS_FROM_USP   (10 * REG_SIZE)  // Code Segment
#define UFLAG_FROM_USP (11 * REG_SIZE) // Flag

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEPING, ZOMBIE } status_t;

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
int fork();
int exec(char* pathname);

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
int do_exec(char* filename);

// int.c
int kcinth();
int kgetpid();
int kps();
int kchname(char* name);
int kkfork();
int ktswitch();
int kkwait(int *status);
int kkexit(int value);
int kgetc();
int kputc(char c);

// KC Lib
int load(char* filename, int segment);


#endif
