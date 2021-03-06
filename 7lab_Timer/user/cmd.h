// James Jessen
// 10918967
// CptS 460

// ------------------------------------------ 
// User Mode: Commands
// ------------------------------------------ 

#ifndef __CMD_H__
#define __CMD_H__


#include "type.h"
#include "syscall.h"
#include "lib/io.h"
#include "lib/string.h"

#define MAX_INPUT_SIZE 64

extern int color;

// u.s
int getcs();

int show_menu();
int (*find_cmd(char *name))(void);
int invalid();


#endif
