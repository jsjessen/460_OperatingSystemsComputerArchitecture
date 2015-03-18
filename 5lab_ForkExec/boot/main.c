// James Jessen
// 1091896
// CptS 460

//========================= BOOTER ========================= 

// Use identifier names <= 8 chars; the compiler will 
// truncate long names to 8 chars, including the leading _ 

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

#include "ext2.h"

#define NULL 0
#define EMPTY 0
#define FAILURE 0

#define START_SEGMENT 0x1000

// A 1.44MB floppy disk:
#define BLOCK_SIZE 1024
#define INODE_SIZE 128
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
// 1440 disk blocks (0 - 1439)
#define NUM_BLOCKS 1440 
// 80 cylinders 
#define NUM_C 80
// 2 heads per cylinder
#define NUM_H 2
// 18 sectors under each head 
#define NUM_S 18

#define INT_SIZE 4 //bytes
#define U16_SIZE 2 //bytes
#define U16_PER_INT (INT_SIZE / U16_SIZE)

#define ROOT_INODE 2
#define NUM_DIRECT_BLOCKS 12
#define MAX_NAME_LENGTH 256
#define GROUP_DESCRIPTOR_BLOCK 2

#define MAX(X,Y) ( ((X) > (Y)) ? (X) : (Y) )

// Utility Functions 
void prints(char *str);
void gets(char str[]);
void get_block(u16 bno, u8 buf[]);
INODE* get_inode(u16 ino);           // uses buf1
u16 search(INODE* ip, char* target); // uses buf2

// Imported from bs.s 
char getc(void);
void putc(char c);
void readfd(u16 cyl, u16 head, u16 sector, u8 buf[]);
void setes(u16 segment);
void inces(void);
void error(void);

// Globals
u8 buf1[BLOCK_SIZE], buf2[BLOCK_SIZE];
u16 iblock;

int main()
{ 
    GD    *gp;
    INODE *ip;
    u16 i, ino;
    //char os_name[MAX_NAME_LENGTH] = "my_mtx";
    char* os_name = "jj_mtx";

    // Get group descriptor
    get_block((u16)GROUP_DESCRIPTOR_BLOCK, buf1);
    gp = (GD*)buf1;

    // Get first inode block 
    iblock = (u16)gp->bg_inode_table; // = 5

    // Let ip point to Root Inode
    ip = get_inode(ROOT_INODE);

    // Search root inode for "boot"
    ino = search(ip, "boot"); // = 13
    ip = get_inode(ino); 

    // Prompt for filename to boot, e.g. mtx or image, etc. 
    // You may assume that all bootable files are in the /boot directory.
    //prints("Boot: "); 
    //gets(os_name);

    // Find the file
    ino = search(ip, os_name); // = 34
    ip = get_inode(ino);

    // Load the data blocks of the OS into memory at segment 0x1000
    // ------------------------------------------------------------

    // IMPORTANT: get indirect block BEFORE moving ES
    if((u16)ip->i_block[NUM_DIRECT_BLOCKS] != EMPTY)
        get_block((u16)ip->i_block[NUM_DIRECT_BLOCKS], buf2);
    else
        ((u16*)buf2)[0] = EMPTY;

    // ES now points at segment 0x1000
    setes(START_SEGMENT);   

    // Load direct blocks (111 - 122)
    for (i = 0; i < NUM_DIRECT_BLOCKS && (u16)ip->i_block[i] != EMPTY; i++)
    {
        // When read/write disk, BIOS INT13 uses (segment, offset) = (ES, BX)
        // In bs.s, readfd() sets BX to the address of the buf parameter
        // So by using 0, instead of reading a block and writing to a buffer,
        // the block is read and written to (ES, 0)
        get_block((u16)ip->i_block[i], 0); 
        inces(); // increment ES by 1KB
    }

    // Load indirect blocks (123 -> 124 - 155) 
    for(i = 0; i < BLOCK_SIZE / U16_PER_INT && ((u16*)buf2)[i] != EMPTY; i += 2)
    {
        // When read/write disk, BIOS INT13 uses (segment, offset) = (ES, BX)
        // In bs.s, readfd() sets BX to the address of the buf parameter
        // So by using 0, instead of reading a block and writing to a buffer,
        // the block is read and written to (ES, 0)
        get_block(((u16*)buf2)[i], 0);
        inces(); // increment ES by 1KB
    }
    // MTX kernel has at most 64 (1KB) blocks, so no double-indirect blocks.

    // Return to bs.s and jump to (0x1000, 0) to start up MTX
    //prints("\n\n\rStarting..."); getc();
    return 0;
}  

//========================= Utility Functions ========================= 

// Slim print string
void prints(char *str)
{
    while(*str) // Loop until null char
        putc(*str++);
}

// Slim get string, be careful: no max limit
// Assumes memory for str has been allocated
void gets(char str[])
{
    while((*(str) = getc()) != '\r')
        putc(*str++); // So user can see what they're typing

    *str = '\0'; // Append with null char
}

void get_block(u16 bno, u8 buf[])
{
    u16 c,h,s;

    // File system uses LINEAR disk block numbers = 0,1,2,.... etc.
    // BIOS INT13 only accepts parameters in CHS format. 
    // 
    // The PHYSICAL layout of a floppy disk is as follows, where
    // cyl, head, sector all count from 0.
    // 
    //        ----------------------------------------------------------------------
    // linear |s0 s1 .... s17 | s18  .... s35 | s36 .....   s53|s54         s71| ...
    //        ----------------------------------------------------------------------
    // sector | 0 ---------17 |   0 ------ 17 |   0 -------- 17|  0 -----    17|
    // head   |<-- head 0---->|<--- head 1 -->|<--- head 0 --->|<-- head 1 --->| ... 
    // cyl    |<--------- ---cyl 0 ---------->|<--------   cyl 1 ------------->| ...
    // Convert linear block number to CHS format

    s = bno * 2; // A 1024 byte block consists of 2 contigious 512 byte sectors

    c = s / (NUM_H * NUM_S); 
    s = s % (NUM_H * NUM_S);

    h = s / NUM_S; 
    s = s % NUM_S; // sector counts from 1, but it adjusted in bs.s 

    // Read 2 sectors from CHS into buf
    readfd(c, h, s, buf);
}

INODE* get_inode(u16 ino)
{
    u16 block = ((ino - 1) / INODES_PER_BLOCK) + iblock;
    u16 index = ((ino - 1) % INODES_PER_BLOCK);

    get_block(block, buf1);

    return (INODE*)buf1 + index;
}

u16 search(INODE* ip, char* target)
{
    u16 i;
    for(i = 0; i < (u16)ip->i_size / BLOCK_SIZE && i < NUM_DIRECT_BLOCKS; i++)
    {
        u8* bp;
        DIR* dp;

        get_block((u16)ip->i_block[i], buf2);
        bp = buf2;
        dp = (DIR*)buf2;

        while(bp < buf2 + BLOCK_SIZE)
        {
            if(strncmp(dp->name, target, MAX(dp->name_len, strlen(target))) == 0)
                return (u16)dp->inode;

            bp += dp->rec_len; // advance bp by rec_len bytes
            dp = (DIR*)bp;     // pull dp along to the next record
        }
    }
    prints("\n\rFile not found"); getc();
    error();

    return FAILURE;
}
