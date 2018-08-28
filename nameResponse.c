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

//Begin functions for name echo
char ans[64];

int prints(char *s)
{
    while(*s) {
        putc(*s);
        s++;
    }
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

//returns 0 if the strings are different, 1 if they are the same
int strcmp(char *s1, char *s2){
    while(1){
        if(*s1 == *s2){
            if(*s1 == '\0'){
                return 1;
            }
            s1++;
            s2++;
        } else {
            return 0;
        }
    }
}

//End functions for name echo

//End functions for loading hard drvie

nameEcho()
{
    while(1){
        prints("What's your name? ");
        gets(ans);  prints("\n\r");

        if (ans[0]==0 || strcmp(ans, "quit")){
            prints("Ok, moving on\n\r");
            return;
        }
        prints("Welcome ");
        prints(ans);
        prints("\n\r");
    }
}

main()
{
    nameEcho();
}