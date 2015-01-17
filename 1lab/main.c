//==================================== t.c ====================================

//                      ************* LOGIC *************                        
//
//    (1). Prompt for filename to boot, e.g. mtx or image, etc. You may assume
//         that all bootable files are in the /boot directory.
//
//    (2). Find the file. Recall that "finding a file amounts to finding its 
//         inode". 
//
//    (3). From the file's inode, find the disk blocks of the file:
//               i_block[0] to i_block[11] are DIRECT blocks, and
//               i_blokc[12] points to INDIRECT blocks.
//         MTX kerenl has at most 64 (1KB) blocks, so no double-indirect blocks.
// 
//    (4). Load the blocks of /boot/mtx into memory at the segment 0x1000.
//
//    (5). Any errro condition, call error() in assembly.
//
//    ==========================================================================  
//
//    (6). If YOUR main() loads the disk blocks of mtx successfully, it returns to
//         bs.s, which jumps to (0x1000, 0) to start up MTX.

#include "ext2.h"  /* download this from LAB1 */
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;


#define BLK 1024

// bs.s exports the following functions:
//      char getc();
//      void putc(char c);
//      void readfd(int cyl, int head, int sector, char *buf);
//      void setes(int segment);
//      void inces();

// Print a string
int prints(char *s)
{
    int i = 0;
    while(*(s + i) != '\0')
    {
        putc(*(s + i));
        i++;
    }
    return i;
}

// Get a string
int gets(char s[])
{
    int i = 0;
    while(1)
    {
        char c = getc();

        if(c == '\n' || c == '\r' || c == '\0')
            break;
        else
            s[i++] = c;
    }
    s[i] = '\0'; // Append with null char
    return i;
}

//      File system uses LINEAR disk block numbers = 0,1,2,.... etc.
//      BIOS INT13 only accepts parameters in CHS format. 
// 
//      YOU must convert a block number to CHS before calling readfd(c,h,s,buf).
// 
// A 1.44MB floppy disk has 80 cylinders, 2 heads per cylinder and 18 sectors
// under each head. The PHYSICAL layout of a floppy disk is as follows, where
// cyl, head, sector all count from 0.
// 
//        ----------------------------------------------------------------------
// linear |s0 s1 .... s17 | s18  .... s35 | s36 .....   s53|s54         s71| ...
//        ----------------------------------------------------------------------
// sector | 0 ---------17 |   0 ------ 17 |   0 -------- 17|  0 -----    17|
// head   |<-- head 0---->|<--- head 1 -->|<--- head 0 --->|<-- head 1 --->| .... 
// cyl    |<--------- ---cyl 0 ---------->|<--------   cyl 1 ------------->| ....
// 
//      A disk block consists of 2 contigious sectors. Given a block number, blk,
//      which counts from 0 to 1339, how to convert blk into (cyl, head, sector)? 
//      -------------------------------------------------------------------------
//                Use the Mailman's algorithm (per CS360) to
//      -------------------------------------------------------------------------

// Converts linear block number to CHS format
u16 getblk(u16 blk, char buf[])
{
    // chs does this
    // loop through all possible values and compare

    // when calling, cast u32 blk -> u16 blk or will shift buf[]
    // remember, bcc makes it all 16 bit, so u32 takes 2 pushes (larger)
    
    //readfd(cyl, head, sector, buf);
}

GD    *gp;
INODE *ip;
DIR   *dp;

int main()
{ 
    u16  i, iblk;
    char buf1[BLK], buf2[BLK];

    prints("booter start\n\r");  

    /* read blk#2 to get group descriptor 0 */
    getblk((u16)2, buf1);
    gp = (GD *)buf1;

    // block where inodes start
    iblk = (u16)gp->bg_inode_table; // typecast u32 to u16

    // only works < 10
    prints("inode_block="); putc(iblk+'0'); getc();

    /******** write C code to do these: ********************
      (1).read first inode block into buf1[ ] // contains 8 inodes

      (2).let ip point to root INODE (inode #2)

      (3).For each DIRECT block of / do:
      read data block into buf2[ ]; // dir entries, need DIR*
      step through the data block to print the names of the dir entries 

    // search "boot", get ino, get its block
    // then
    // search "mtx", get ino, get its block

    // Load
    ES = 0x1000
    Load 1kb
    ES = 0x2000
    Load 1kb
    ...

    (4).prints("\n\rAll done\n\r");
     ******************************************************/
}  
