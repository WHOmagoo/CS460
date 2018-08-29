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

    //putc('\0');
}

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u8 str_cmp(char *s1, char *s2, u32 length){
    while(length > 0){
        if(*s1 == *s2){
            s1++;
            s2++;
        } else {
            return 0;
        }

        length--;
    }

    return 1;
}


DIR *dp;
u16 len;

u8 loopCounter;

u16 search(INODE *ip, char *name)
{
    for(loopCounter = 0; loopCounter < 11; loopCounter++){
        if((u32) ip->i_block[loopCounter] == (u32) 0){
            break;
        } else {
            getblk((u16) ip->i_block[loopCounter], buf2);

            len = 0;

            while(len < BLK) {
                dp = (DIR *) (buf2 + len);
                len += dp->rec_len;

                if(str_cmp(dp->name, name, dp->name_len) == 1){

                    return (u16) dp->inode;
                }

            }

        }
    }

    error();
//    search for name in the data block of INODE;
//    return its inumber if found
//            else error();
}

GD *gd;
INODE *ip;

u16 bootInodeNum;
u16 inode_table;

u32* up;

main()
{
    getblk(2, buf1);

    gd = (struct GD*) buf1;

    inode_table = gd->bg_inode_table;

    getblk((u16) gd->bg_inode_table, buf1);

    ip = (INODE *) buf1 + 1;

    bootInodeNum = search(ip, "boot");

    getblk((u16) (inode_table + (bootInodeNum - 1) / 8), buf1);
    ip = (INODE *) buf1 + (bootInodeNum - 1) % 8;

//    prints("b");

    bootInodeNum = search(ip, "mtx");

    getblk((u16) (inode_table + (bootInodeNum - 1) / 8), buf1);
    ip = (INODE *) buf1 + (bootInodeNum - 1) % 8;

    //load the indirect block into buf2 before we mess with our memory using setes and inces
    getblk((u16) ip->i_block[12], buf2);


    setes(0x1000);

    for(loopCounter = 0; loopCounter < 12; loopCounter++){
        if((u16) ip->i_block[loopCounter] == (u16) 0){
            putc('<');
            break;
        }
        getblk((u16)ip->i_block[loopCounter], 0);
        putc('*');
        inces();
    }

//    1. Write YOUR C code to get the INODE of /boot/mtx
//    INODE *ip --> INODE
//
//    if INODE has indirect blocks: get i_block[12] int buf2[  ]
//
//
//    2. setes(0x1000);  // MTX loading segment = 0x1000
//
//    3. load 12 DIRECT blocks of INODE into memory
//    for (i=0; i<12; i++){
//        getblk((u16)ip->i_block[i], 0);
//        putc('*');
//        inces();
//    }
//
//    4. load INDIRECT blocks, if any, into memory

    //prints("d-fin\n\r");

    if (ip->i_block[12]){

        up = (u32 *) buf2;

        while(*up){
            getblk((u16)*up, 0);
            putc('.');
            inces();
            up++;
        }
    }

    prints("go?");
    getc();

    return 1;
}