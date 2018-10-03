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

//#include "defines.h"
#include "string.c"

int color;

#include "type.h"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "queue.c"
#include "kernel.c"
#include "timer.c"


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

// IRQ interrupts handler entry point
// void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER *tp[4];

int interpritMode(int cpsr, int *result){
    int i = 4;

    while(i >= 0){
        result[i] = cpsr & 1;
        i--;
        cpsr = cpsr >> 1;
    }

}

int getcpsr() {
    int result = 1;
    __asm__ (
        "mrs %0, cpsr;"
        : "=r" (result)
    );

    return result;
}

int getspsr() {
    int result = 1;
    __asm__ (
    "mrs %0, spsr;"
    : "=r" (result)
    );

    return result;
}

int showCurCPSR(){
    int cpsrResult[5];

    interpritMode(getcpsr(), cpsrResult);

    kprintf("CPSR %d%d%d%d%d\n", cpsrResult[0],cpsrResult[1],cpsrResult[2],cpsrResult[3],cpsrResult[4]);
}

int showCurSPSR(){
    int cpsrResult[5];

    interpritMode(getspsr(), cpsrResult);

    kprintf("SPSR %d%d%d%d%d\n", cpsrResult[0],cpsrResult[1],cpsrResult[2],cpsrResult[3],cpsrResult[4]);
}

int sayCurMode(){
    int cpsrResult[5];
    interpritMode(getcpsr(), cpsrResult);

    int irqMode[] = {1,0,0,1,0};
    int svcMode[] = {1,0,0,1,1};

    int i = 0;

    int *cur;
    char *result;


    if(cpsrResult[4] == 1){
        cur = svcMode;
        result = "CPU is in SVC mode\n";
    } else {
        cur = irqMode;
        result = "CPU is in IRQ mode\n";
    }

    while(i < 4){
        if(cur[i] != cpsrResult[i]){
            kprintf("There was an error matching the current mode. ", result);

            for(i = 0; i < 5; i++){
                kprintf("(%d,%d)", cur[i], cpsrResult[i]);
            }

            kprintf("\n");
            return;
        }

        i++;
    }

    kprintf(result);
    return;
}

int sayPrevMode(){
    int cpsrResult[5];
    interpritMode(getspsr(), cpsrResult);

    int irqMode[] = {1,0,0,1,0};
    int svcMode[] = {1,0,0,1,1};

    int i = 0;

    int *cur;
    char *result;


    if(cpsrResult[4] == 1){
        cur = svcMode;
        result = "CPU was is in SVC mode\n";
    } else {
        cur = irqMode;
        result = "CPU was is in IRQ mode\n";
    }

    while(i < 4){
        if(cur[i] != cpsrResult[i]){
            kprintf("There was an error matching the saved mode. ");

            for(i = 0; i < 5; i++){
                kprintf("(%d,%d)", cur[i], cpsrResult[i]);
            }

            kprintf("\n");
            return;
        }

        i++;
    }

    kprintf(result);
    return;
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

//main from lab3
//int main() {
//
//    int i;
//    char line[128];
//
//    color = YELLOW;
//    row = col = 0;
//    fbuf_init();
//
//
//    /* enable timer0,1, uart0,1 SIC interrupts */
//    VIC_INTENABLE |= (1 << 4);  // timer0,1 at bit4
//    VIC_INTENABLE |= (1 << 5);  // timer2,3 at bit5
//
//    VIC_INTENABLE |= (1 << 31); // SIC to VIC's IRQ31
//
//    /* enable KBD IRQ */
//    SIC_ENSET = 1 << 3;     // KBD int=3 on SIC
//    SIC_PICENSET = 1 << 3;  // KBD int=3 on SIC
//
//    /****
//
//    IRQ mode: 10010 (0x12)
//    SVC mode: 10011 (0x13)
//
//    ****/
//
//    kprintf("SVC mode is 10011\n");
//    kprintf("IRQ mode is 10010\n");
//    sayCurMode();
//    kprintf("C3.2 start: test timer KBD drivers by interrupts\n");
//    timer_init();
//    kbd_init();
//    /***************
//    for (i=0; i<4; i++){
//       tp[i] = &timer[i];
//       timer_start(i);
//    }
//    ************/
//    timer_start(0);
//
//
//    while (1) {
//        color = CYAN;
//        kprintf("Enter a line from KBD\n");
//        kgets(line);
//        color = CYAN;
//        kprintf("line = %s\n", line);
//
//    }
//}

int body();

void listen(){

    char line[128];
    row = col = 0;

    while (1) {
        color = CYAN;
        kprintf("Enter a line from KBD\n");
        kgets(line);
        color = CYAN;
        kprintf("line = %s\n", line);

    }
}

//main from lab4
int main() {
    int i;
    char line[128];
    u8 kbdstatus, key, scode;

    color = WHITE;
    row = col = 0;


/* enable timer0,1, uart0,1 SIC interrupts */
    VIC_INTENABLE |= (1 << 4);  // timer0,1 at bit4
    VIC_INTENABLE |= (1 << 5);  // timer2,3 at bit5

    /* enable KBD IRQ */
    VIC_INTENABLE |= 1 << 31;  // SIC to VIC's IRQ31
    SIC_ENSET |= 1 << 3;       // KBD int=3 on SIC

    fbuf_init();
    timer_init();
    kbd_init();


    kprintf("Welcome to WANIX in Arm - Made by Hugh McGough\n");
    timer_start(0);
    init();
    kfork((int) body, 1);
    kfork((int)listen, 1);

    while (1) {
        if (readyQueue)
            tswitch();
    }

    return 0;
}


