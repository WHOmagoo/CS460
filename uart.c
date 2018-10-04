/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include <stdarg.h>

#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
    char *base;
    int n;
}UART;

UART uart[4];

int uart_init()
{
    int i; UART *up;

    for (i=0; i<4; i++){
        up = &uart[i];
        up->base = (char *)(0x10009000 + i*0x1000);
        up->n = i;
    }
    //uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
    while (*(up->base + FR) & RXFE);
    return *(up->base + DR);
}

int uputc(UART *up, char c)
{
    while(*(up->base + FR) & TXFF);
    *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
    while ((*s = (char)ugetc(up)) != '\r'){
        uputc(up, *s);
        s++;
    }
    *s = 0;
}

int uprints(UART *up, char *s)
{
    while(*s)
        uputc(up, *s++);
}

int pow(int base, int power){
    int result = 1;
    for(;power > 0; power--){
        result *= base;
    }

    return result;
}

int unsignedIntToString(int num, char *result){
    int i = 9;
    int tensPlacesCount = 0;

    for(;i >= 0; i--){
        int tensPlaceValue = (int) (num / pow(10, i));
        tensPlaceValue = tensPlaceValue % 10;

        if(tensPlaceValue != 0){
            if(tensPlacesCount == 0){
                tensPlacesCount = i;
            }

            result[tensPlacesCount - i] = 48 + tensPlaceValue;
        }
    }

    if(tensPlacesCount != 10){
        result[tensPlacesCount + 1] = '\0';
    }
}

int intToString(int num, char *result){
    if(num < 0){
        num *= -1;
        result[0] = '-';
        result++;
    } else {
        //append null character to the end of a positive string
        result[11] = '\0';
    }

    unsignedIntToString((u32) num, result);
}

int intToHexString(int num, char *result){
    int i = 7;

    for(;i >= 0; i--){
        int placeValue = num / pow(16, i) % 16;

        if(placeValue > 9){
            result[7 - i] = 65 + placeValue - 10;
        } else {
            result[7 - i] = 48 + placeValue;
        }
    }
}

int fuprints(UART *up, char *fmt, ...){
    va_list list;
    va_start(list, fmt);
    int index = 1;
    while(*fmt){
        if(*fmt == '%'){
            fmt++;
            if(*fmt) {
                if(*fmt == 'd'){
                    char result[11];
                    intToString(va_arg(list, int), result);

                    int i = 0;
                    for(; i < 11 && result[i] != 0; i++){
                        uputc(up, result[i]);
                    }

                } else if(*fmt == 'c'){
                    char c = (char) va_arg(list, int);
                    uputc(up, c);
                } else if(*fmt == 's'){
                    char *s = va_arg(list, char*);

                    uprints(up, s);
                } else if(*fmt == 'x'){
                    uprints(up, "0x");
                    char result[8];
                    intToHexString(va_arg(list, u32), result);

                    int i = 0;
                    for(; i < 8; i++){
                        uputc(up, result[i]);
                    }
                }
            } else {
                break;
            }

            index++;
        } else {
            uputc(up, *fmt);
        }
        fmt++;
    }

//    va_end(list);
}
