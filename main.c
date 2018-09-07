//
// Created by whomagoo on 8/23/18.
//

#include <stdio.h>
#include <zconf.h>

int fuprints(char *a, char *fmt, ...){
//    va_list list;
//    va_start(list, fmt);
    int index = 1;
    while(*fmt){
        if(*fmt == '%'){
            fmt++;
            if(*fmt) {
                if(*fmt == 'd'){
                    char result[11];
                    //intToString(va_arg(list, int), result);

                    int i = 0;
                    for(; i < 11 && result[i] != 0; i++){
                        printf("%c", result[i]);
                    }

                } else if(*fmt == 'c'){
                    //char c = (char) va_arg(list, int);
                    char c = *(fmt + index);
                    printf("%c", c);
                } else if(*fmt == 's'){
                    //char *s = va_arg(list, char*);

                    char *s = '\0';

                    while(*s){
                        printf("%c", *s);
                        s++;
                    }
                } else if(*fmt == 'x'){
                    printf("0x");
                    char result[8];
                    //intToHexString(va_arg(list, __uint32_t), result);

                    for(int i = 0; i < 8; i++){
                        printf("%c", result[i]);
                    }
                }
                index++;
            }
        } else {
            printf("%c", *fmt);
        }
        fmt++;
    }

    //va_end(list);
}

int pow(int base, int power){
    int result = 1;
    for(;power > 0; power--){
        result *= base;
    }

    return result;
}

int unsignedIntToString(__uint32_t num, char *result){
    int i = 9;
    int tensPlacesCount = 0;

    for(;i >= 0; i--){
        int tensPlaceValue = num / pow(10, i) % 10;

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

    unsignedIntToString((__uint32_t) num, result);
}

int intToHexString(__uint32_t num, char *result){
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

int main(){
    printf("Lab2 test\n");
    char *input = "Number %d, Hex number %x, String %s, char %c, COOL!\n";
    int num = -2147483649;
    int hex = 0xF43A;
    char* s = "Mambo combo";
    char c = '(';
    fuprints(0, input, num, hex, s, c);
    return 0;
}
