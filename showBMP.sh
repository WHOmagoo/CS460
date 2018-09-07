#!/usr/bin/env bash

echo Obj copy
arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm image1 image1.o
#arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm image2 image2.o

echo Assemble
arm-none-eabi-as -mcpu=arm926ej-s -g tsBMP.s -o tsBMP.o

echo Compile
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g showBMP.c -o showBMP.o

echo Link
arm-none-eabi-ld -T tBMP.ld image1.o tsBMP.o showBMP.o strlib /usr/lib/gcc/arm-none-eabi/4.9.3/libgcc.a -o t.elf

echo Objcopy
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy
qemu-system-arm -M versatilepb -m 128M -kernel t.bin -serial mon:stdio