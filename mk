# 1. What does the following statement do? Assemble the code from ts.s using the cpu architecture for arm926ej with debug stuff included
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o

# 2. What does the following statement do? Compile the code from t.c for arm926ej cpu leaving debug stuff in
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o

# 3. What does the following statement do? link object files from t.ld ts.o, t.o and strlib into one t.elf file. -T means to use scriptfile
arm-none-eabi-ld -T t.ld ts.o t.o strlib -o t.elf

# 4. What does the following statement do? This will rewrite object code from t.elf in a binary executable file
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 128M -kernel t.bin \
-serial mon:stdio