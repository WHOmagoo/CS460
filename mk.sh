#!/usr/bin/env bash
# 1. What does the following statement do? Assemble the code from ts.s using the cpu architecture for arm926ej with debug stuff included
echo Assemble
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o

echo Compile
# 2. What does the following statement do? Compile the code from t.c for arm926ej cpu leaving debug stuff in
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o

echo Link
# 3. What does the following statement do? link object files from t.ld ts.o, t.o and strlib into one t.elf file. -T means to use scriptfile
arm-none-eabi-ld -T t.ld ts.o t.o strlib /usr/lib/gcc/arm-none-eabi/4.9.3/libgcc.a -o t.elf

echo Package
# 4. What does the following statement do? This will rewrite object code from t.elf in a binary executable file
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M realview-pbx-a9 -m 128M -kernel t.bin \
-serial mon:stdio -serial /dev/pts/0 -serial /dev/pts/0 -serial /dev/pts/0

#qemu-system-arm -M versatilepb -m 128M -kernel t.bin \
#-serial mon:stdio