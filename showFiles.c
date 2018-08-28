/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

int prints(char *s)
{
    while(*s) {
        putc(*s);
        s++;
    }

    putc('\0');
}

int gets(char *s)
{
    do {
        *s = getc();
        putc(*s);
        *s++;
    } while(*(s -1) != '\r');

    *(s-1) = '\0';
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
//    search for name in the data block of INODE;
//    return its inumber if found
//            else error();
}

GD *gd;
INODE *ip;
DIR *dp;
u16 len;

u8 loopCounter;

main()
{
    getblk(2, buf1);

    gd = (struct GD*) buf1;


    getblk((u16) gd->bg_inode_table, buf1);
    ip = (INODE *) buf1 + 1;

    for(loopCounter = 0; loopCounter < 11; loopCounter++){
        if((u32) ip->i_block[loopCounter] == (u32) 0){
            break;
        } else {
            getblk((u16) ip->i_block[loopCounter], buf2);

            len = 0;

            while(len < BLK) {
                dp = (DIR *) (buf2 + len);
                len += dp->rec_len;

                prints(dp->name);
                prints("\r\n");
            }

        }
    }

    prints("All folders have been shown\r\n"); getc();
}