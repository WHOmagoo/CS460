#!/usr/bin/env bash
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o
arm-none-eabi-ld -T t.ld ts.o t.o strlib /usr/lib/gcc/arm-none-eabi/4.9.3/libgcc.a -o t.elf
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 128M -kernel t.bin -serial mon:stdio -serial /dev/pts/0 -serial /dev/pts/0 -serial /dev/pts/1
#the last serial is the one that is used for the uart in this program









