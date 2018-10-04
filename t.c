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

#include "type.h"
#include "string.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "queue.c"
#include "kernel.c"
#include "timer.c"
#include "uart.c"
#include "pipe.c"

PIPE *kpipe;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if(vicstatus & (1<<4)){
        timer_handler(0);
    }
    if (vicstatus & 0x80000000){
        if (sicstatus & 0x08){
            kbd_handler();
        }
    }
}
int body();

int pipe_writer() // pipe writer task code
{
    struct uart *up = &uart[0];
    char line[128];

    kprintf("WRITER IS PID %d", running->pid);

    while(1){
//        uprints(up, "Enter a line for task1 to get : ");
        kprints("WRITER Enter a line for task1 to get :\n\r");
//        kprintf("task%d waits for line from UART0\n", running->pid);
        kprintf("WRITER task%d waits for line from console\n", running->pid);
//        ugets(up, line);
        kgets(line);


//        uprints(up, "\r\n");
        kprintf("\r\n");
//        printf("task%d writes line=[%s] to pipe\n", running->pid, line);
        kprintf("task%d writes line=[%s] to pipe, length = %d\n", running->pid, line, strlen(line));

        write_pipe(kpipe, line, strlen(line));


        int SP = int_on();
        tswitch();

        int_off(SP);
    }
}

int pipe_reader()
// pipe reader task code
{
    char line[128];
    int i, n;

    kprintf("READER IS PID %d", running->pid);

    while(1){
//        printf("task%d reading from pipe\n", running->pid);
        kprintf("READ - task%d reading from pipe\n", running->pid);
        n = read_pipe(kpipe, line, PSIZE);

//        printf("task%d read n=%d bytes from pipe : [", running->pid, n);
        kprintf("<READ> -task%d read n=%d bytes from pipe : [", running->pid, n);
        for (i=0; i<n; i++) {
            kputc(line[i]);
        }
        kprintf("]\n");

    }
}

int main()
{

    int i;
    char line[128];
    u8 kbdstatus, key, scode;

    color = WHITE;
    row = col = 0;


///* enable timer0,1, uart0,1 SIC interrupts */
    VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4
    VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5

    /* enable KBD IRQ */
    VIC_INTENABLE |= 1<<31;  // SIC to VIC's IRQ31
    SIC_ENSET |= 1<<3;       // KBD int=3 on SIC

    fbuf_init();
    kprintf("Welcome to Wanix in ARM\n");
    uart_init();
    kbd_init();
    pipe_init();
// initialize PIPEs
    kpipe = create_pipe(); // create global kpipe
    init();
    kprintf("P0 kfork tasks\n");

    kfork((int)pipe_writer, 1); // pipe writer process
    kfork((int)pipe_reader, 1); // pipe reader process

    printList("Ready queue init", readyQueue);

    while(1){
        if (readyQueue)
            tswitch();
    }
}

//int main()
//{
//    int i;
//    char line[128];
//    u8 kbdstatus, key, scode;
//
//    color = WHITE;
//    row = col = 0;
//
//
///* enable timer0,1, uart0,1 SIC interrupts */
//    VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4
//    VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5
//
//    /* enable KBD IRQ */
//    VIC_INTENABLE |= 1<<31;  // SIC to VIC's IRQ31
//    SIC_ENSET |= 1<<3;       // KBD int=3 on SIC
//
//    fbuf_init();
//    timer_init();
//    kbd_init();
//
//
//    kprintf("Welcome to WANIX in Arm - Made by Hugh McGough\n");
//    timer_start(0);
//    init();
//    kfork((int)body, 1);
//
//    while(1){
//        if (readyQueue)
//            tswitch();
//    }
//}
