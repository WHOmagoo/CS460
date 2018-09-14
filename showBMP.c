//
// Created by whomagoo on 9/7/18.
//

typedef unsigned int   u32;

//#include "defines.h" // device base addresses, etc.
#include "vid.c" // LCD driver
#include "uart.c" // UART driver
extern char _binary_image1_start;
#define WIDTH 640

UART *up;

int show_bmp(char *p, int start_row, int start_col)
{
    int h, w, pixel, rsize, i, j;
    unsigned char r, g, b;
    char *pp;
    int *q = (int *)(p+14); // skip over 14-byte file header

    int offset = *q;

    //q+= 4;

    w = *(q+1);
    h = *(q+2); // image width in pixels
// image height in pixels
    p += 54; // p-> pixels in image
//BMP images are upside down, each row is a multiple of 4 bytes
    rsize = 4*((3*w + 3)/4); // multiple of 4
    p += (h-1)*rsize; // last row of pixels
    for (i=start_row; i<start_row + h / 2; i++){
        pp = p;
        for (j=start_col; j<start_col + w / 2; j++){
            b = *pp; g = *(pp+1); r = *(pp+2); // BRG values
            pixel = (b<<16) | (g<<8) | r;
// pixel value

            fb[i * WIDTH + j] = pixel; // write to frame buffer
            pp += 3;
            pp += 3;
        }
        p -= rsize;
        p -= rsize;
// advance pp to next pixel
// to preceding row
    }
    fuprints(up, "\nBMP image height=%d width=%d\n", h, w);
}

int main()
{
    char c,* p;
    uart_init();
    up = &uart[0]; // initialize UARTs
// use UART0
    fbuf_init(0); // default to VGA mode
    while(1){
        p = &_binary_image1_start;
        show_bmp(p, 0, 0); // display image1
        uprints(up, "enter a key from this UART : ");
        ugetc(up);
//        p = &_binary_image2_start;
//        show_bmp(p,120, 0); // display image2
    }
    while(1);
// loop here
    //Hi
}