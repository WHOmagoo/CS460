MAIN="showFiles"

as86 -o bs.o bs.s

bcc -c -ansi $MAIN.c

ld86 -d bs.o $MAIN.o /usr/lib/bcc/libc.a

ls -l a.out

dd if=a.out of=mtximage bs=1024 count=1 conv=notrunc

rm *.o
echo done, now booting

qemu-system-i386 -fda mtximage -no-fd-bootchk
