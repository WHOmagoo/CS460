/************** vid.c file of C2.6 ******************/
int volatile *fb;
int WIDTH = 640; // default to VGA mode for 640x480
int fbuf_init(int mode)
{
    fb = (int *)(0x200000); // at 2 MB to 4 MB
//***************** for 640x480 VGA ****************/
    *(volatile unsigned int *)(0x1000001c) = 0x2C77;
    *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;
    *(volatile unsigned int *)(0x10120004) = 0x090B61DF;
    *(volatile unsigned int *)(0x10120008) = 0x067F1800;

    /***************** for 800X600 SVGA ****************
    *(volatile unsigned int *)(0x1000001c) = 0x2CAC;
    *(volatile unsigned int *)(0x10120000) = 0x1313A4C4;
    *(volatile unsigned int *)(0x10120004) = 0x0505F6F7;
    *(volatile unsigned int *)(0x10120008) = 0x071F1800;
    }
    ****************************************************/

    *(volatile unsigned int *)(0x10120010) = 0x200000; // fbuf
    *(volatile unsigned int *)(0x10120018) = 0x82B;
}