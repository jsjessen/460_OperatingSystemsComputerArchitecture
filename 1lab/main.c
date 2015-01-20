// James Jessen
// 1091896
// CptS 460

// Note: Use identifier names <= 8 chars; the compiler will 
// truncate long names to 8 chars, including the leading _ 

#include <string.h>
#include "ext2.h"

#define NUM_DIRECT 12
#define EMPTY 0

// A 1.44MB floppy disk with 1024 byte blocks:
#define BLOCK_SIZE 1024
// 1440 disk blocks (0 - 1439)
#define NUM_BLOCKS 1440 
// 80 cylinders 
#define NUM_C 80
// 2 heads per cylinder
#define NUM_H 2
// 18 sectors under each head 
#define NUM_S 18

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD    *gp;
INODE *ip;
DIR   *dp;

// Utility Functions 
void print_num(u16 n);
u16 prints(char *str);
u16 gets(char str[], int max);
u16 getblk(u16 blk, u8 buf[]);
u16 search(char* target);

// Exported from bs.s 
char getc();
void kc_putc(char c);
void readfd(int cyl, int head, int sector, char *buf);
void setes(int segment);
void inces();
// ------------------------------------------------------
// char getc();
// void kc_putc(char c);
// void readfd(int cyl, int head, int sector, char *buf);
// void setes(int segment);
// void inces();
// ------------------------------------------------------

int main()
{ 
    char file[64];

    u16 i, iblk;
    u8 buf1[BLOCK_SIZE], buf2[BLOCK_SIZE];

    prints("booter start\n\r");  

    // read block 2 to get group descriptor
    getblk((u16)2, buf1);
    gp = (GD*)buf1;

    // block where inodes start
    iblk = (u16)gp->bg_inode_table;
    prints("inode block = "); print_num(iblk); getc();

    // (1).read first inode block into buf1[ ] (contains 8 inodes)
    getblk(iblk, buf1);
    ip = (INODE*)buf1;

    // (2).let ip point to root INODE (inode #2)
    // Inode count starts from 1 not 0, so 1->2 is a single increment
    ip++; 

    // (3).For each DIRECT block of / do:
    // read data block into buf2[ ]; // dir entries, need DIR*
    // step through the data block to print the names of the dir entries 
    // search / for "boot", get ino, get its block
    u16 ino = search(ip, "boot");
    ip = getino(ino);

    // Prompt for filename to boot, e.g. mtx or image, etc. 
    // You may assume that all bootable files are in the /boot directory.
    do
    {
        prints("Boot: ");
        gets(file, 64);
        ino = search(file);

        if(!ino)
            prints("File does not exist\n");
    }
    while(ino == 0);

    // Find the file. 
    // Recall that "finding a file amounts to finding its inode". 
    ino = search(ip, file);
    ip = getino(ino);

    // From the file's inode, find the disk blocks of the file:
    //       i_block[0] to i_block[11] are DIRECT blocks, and
    //       i_blokc[12] points to INDIRECT blocks.
    // MTX kernel has at most 64 (1KB) blocks, so no double-indirect blocks.

    // Load the blocks of /boot/mtx into memory at the segment 0x1000.

    // Load
    // ES = 0x1000
    // Load 1kb
    // ES = 0x2000
    // Load 1kb
    // ...

    // use setes() and inces()

    // Assume: INODE *ip -> INODE of a file
    setes(0x1000);   // ES now points at segment 0x1000

    // Loads 12 DIRECT BLOCKs of the file into 
    // 0x1000, 0x1000+1k,0x1000+2K, ......,0x1000+11K

    for (i = 0; i < NUM_DIRECT; i++)
    {
        getblk((u16)ip->i_block[i], 0);  // load block to (ES, 0)
        inces();     // increment ES by 1KB (in 16-byte units)
    }

    getblk((u16)ip->i_block[NUM_DIRECT], 0);  // load block to (ES, 0)

    // READ INDIRECT BLOCKS BEFORE INC ES
    // Otherwise the read might be out of bounds or some such

    // The above code loads 12 DIRECT BLOCKs of the file into 
    // 0x1000, 0x1000+1k,0x1000+2K, ......,0x1000+11K

    // Any errro condition, call error() in assembly.

    // =====================================================================  

    prints("\n\rAll done!\n\r");

    // If main() loads the disk blocks of mtx successfully, 
    // it returns to bs.s, which jumps to (0x1000, 0) to start up MTX.

    return 0;
}  

//============================== Utility Functions ============================== 

// Only works for 0 <= n < 10,000
void print_num(u16 n)
{
    putc((n / 1000) + '0'); 
    n =  (n % 1000);

    putc((n / 100)  + '0'); 
    n =  (n % 100);

    putc((n / 10)   + '0'); 
    putc((n % 10)   + '0'); 
}

// Print string
u16 prints(char *str)
{
    u16 i = 0;
    while(str[i]) // Loop until null char
        putc(str[i++]);

    return i;
}

// Get string
// Assumes memory for str has been allocated
// Max specifies the maximum number of characters to read
u16 gets(char str[], int max)
{
    u16 i = 0;
    while(i < max)
    {
        char c = getc();

        if(c == '\n' || c == '\r' || c == '\0')
            break;
        else
            str[i++] = c;
    }
    str[i] = '\0'; // Append with null char
    return i;
}

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

// when calling, cast u32 blk -> u16 blk or will shift buf[]
// remember, bcc makes it all 16 bit, so u32 takes 2 pushes (larger)
u16 getblk(u16 blk, u8 buf[])
{
    // Convert linear block number to CHS format
    u16 c,h,s;

    if(blk >= NUM_BLOCKS)
    {
        prints("error: getblk(): requested block outside of floppy disk range\n");
        return 0;
    }

    s = blk * 2; // A 1024 byte disk block consists of 2 contigious 512 byte sectors

    c = s / (NUM_H * NUM_S);
    s = s % (NUM_H * NUM_S);

    h = s / NUM_S;
    s = s % NUM_S;

    // Read 2 sectors from CHS into buf
    readfd(c, h, s, buf);
    return blk;
}

u16 search(char* target)
{
    u16 ino = 0;
    u16 i;
    for(i = 0; i < NUM_DIRECT; i++)
    {
        if(ip->i_block[i] == EMPTY)
            break;

        getblk((u16)ip->iblock[i], buf2);

        u8* cp = buf2;
        DIR* dp = (DIR*)buf2;

        while(cp < buf2 + BLOCK_SIZE)
        {
            char name[256];
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = 0;

            prints(name);

            // Add break to stop searching when found
            if(strcmp(name, target) == 0)
                ino = dp->inode;

            cp += dp->rec_len; // advance cp by rec_len bytes
            dp = (DIR*)cp;     // pull dp along to the next record
        }
    }
}
