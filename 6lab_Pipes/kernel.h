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

#define NPROC 9        // Number of Processes
#define NOFT  20       // Number of Open File Tables
#define NPIPE 10       // Number of Pipes

#define READ_PIPE  4
#define WRITE_PIPE 5

#define NUM_KREG  9    // Number of Kernel-mode Registers
#define NUM_UREG 12    // Number of User-mode Registers

#define KB        1024 // bytes per kilobyte
#define WORD_SIZE 2    // bytes per word
#define REG_SIZE  2    // bytes per register (16-bit real mode)
#define PTR_SIZE  2    // bytes per pointer

//=====================================================================================
//               uSP
// ----------------------------------------------------------------------------
// |Code Data...|uDS|uES|udi|usi|ubp|udx|ucx|ubx|uax|uPC|uCS|Flag| Next Segment
// ----------------------------------------------------------------------------
//               -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 | 
//                                                               End of Segment
//=====================================================================================
#define UFLAG_FROM_END (-1  * REG_SIZE) // Flag
#define UCS_FROM_END   (-2  * REG_SIZE) // Code Segment
#define UES_FROM_END   (-11 * REG_SIZE) // Extra Segment
#define UDS_FROM_END   (-12 * REG_SIZE) // Data Segment

//=====================================================================================
//               uSP
// ------------------------------------------------------------------------------------
// |Code Data...|uDS|uES|udi|usi|ubp|udx|ucx|ubx|uax|uPC|uCS|Flag|rPC|7|0| Next Segment
// ------------------------------------------------------------------------------------
//                0   1   2   3   4   5   6   7   8   9   10   11        |
//                                                                       End of Segment
//=====================================================================================
#define UDS_FROM_USP   ( 0 * REG_SIZE)  // Data Segment
#define UES_FROM_USP   ( 1 * REG_SIZE)  // Extra Segment
#define UAX_FROM_USP   ( 8 * REG_SIZE)  // Return Register
#define UCS_FROM_USP   (10 * REG_SIZE)  // Code Segment
#define UFLAG_FROM_USP (11 * REG_SIZE)  // Flag

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEPING, ZOMBIE } status_t;

extern PROC proc[], *running, *freeList, *sleepList, *readyQueue;
extern char* states[];
extern char* pname[];
extern int procSize;
extern int nproc;
extern int color;
extern OFT oft[NOFT];
extern PIPE pipe[NPIPE];

// main.c
int body();
void scheduler();

// ts.s
int int80h();
int tswitch(); // TEST if it returns an int
int goUmode();

// kernel.c
int do_ps();
void do_tswitch();
int do_exit();
void do_sleep();
void do_wakeup();
int do_wait(int* status);
int do_exec(char* filename);
int kmode();
int kexit(u16 exitValue);

// wait.c
void ksleep(int event);
void kwakeup(int event);
int kwait(int* status);

// fork_exec.c
PROC *kfork(char* filename);
int fork();
int exec(char* pathname);

// pipe.c
void show_pipe(PIPE *p);
int pfd();
int read_pipe(int fd, char* buf, int n);
int write_pipe(int fd, char* buf, int n);
int kpipe(int pd[2]);
int close_pipe(int fd);

// int.c
int kcinth();
int do_ps();
int chname(char* name);
int kkwait(int *status);
int kkexit(int value);

// KC Lib
int load(char* filename, int segment);


#endif
