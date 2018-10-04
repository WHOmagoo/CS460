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

// timer.c file
/***** timer confiuguration values *****/
#define CTL_ENABLE          ( 0x00000080 )
#define CTL_MODE            ( 0x00000040 )
#define CTL_INTR            ( 0x00000020 )
#define CTL_PRESCALE_1      ( 0x00000008 )
#define CTL_PRESCALE_2      ( 0x00000004 )
#define CTL_CTRLEN          ( 0x00000002 )
#define CTL_ONESHOT         ( 0x00000001 )

// timer register offsets from base address
/**** byte offsets *******
#define TLOAD   0x00
#define TVALUE  0x04
#define TCNTL   0x08
#define TINTCLR 0x0C
#define TRIS    0x10
#define TMIS    0x14
#define TBGLOAD 0x18
*************************/
/** u32 * offsets *****/
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer{
    u32 *base;            // timer's base address; as u32 pointer
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
}TIMER;
volatile TIMER timer[4];  // 4 timers; 2 timers per unit; at 0x00 and 0x20

typedef struct tqe{
    int relativeTime;
    PROC *p;
    struct tqe *next;
} TQE;


TQE tqeBuf[NPROC];
TQE *tqeRoot;
TQE *tqeFree;


int kprintf(char *fmt, ...);
int kpchar(char, int, int);
int unkpchar(char, int, int);

tqeInit(){
    int i;
    TQE *tqe;
    for(i = 0; i < NPROC; i++){
        tqe = &tqeBuf[i];
        tqe->next = tqe + 1;
    }

    tqe[NPROC - 1].next = 0;
    tqeFree = &tqeBuf[0];
}

TQE* getNewTQE(){
    if(tqeFree) {
        TQE *result = tqeFree->next;
        if (tqeFree != tqeFree->next) {
            tqeFree->next = result->next;
        } else {
            tqeFree = 0;
        }
        return result;
    } else {
        kprintf("No TQE free\n\r");
        return 0;
    }
}


//Parameter totalTime is total time but is subtracted from and used like relative time.
void tqeInsert(int totalTime, PROC *p){
    lock();
    TQE *prev = 0;
    TQE *cur = tqeRoot;
    while(cur){
        if(cur->relativeTime > totalTime){
            TQE *inserting = getNewTQE();
            if(inserting) {
                inserting->p = p;
                inserting->relativeTime = totalTime;
                inserting->next = cur;

                //adjust relative time for the tqe after the one we just inserted
                cur->relativeTime-= totalTime;
                if (prev) {
                    prev->next = inserting;
                } else {
                    tqeRoot = inserting;
                }
            }

            //TODO uncomment this when testing is over
            ksleep((int)p);
            //kprintf("Inserted in the middle");
            return;
        }

        //We turn totalTime into a relative time by subtracting here.
        totalTime -= cur->relativeTime;

        prev = cur;
        cur = cur->next;
    }

    TQE *inserting = getNewTQE();
    if(inserting) {
        inserting->relativeTime = totalTime;

        //should be 0 always
        inserting->next = cur;
        if (prev) {
            prev->next = inserting;
           //kprintf("Inserted in the end");
        } else {

            tqeRoot = inserting;
           // kprintf("Made new queue");
        }
    }

    unlock();
}

void tqeIncrement(){
    if(tqeRoot) {
        tqeRoot->relativeTime--;
        if (tqeRoot->relativeTime <= 0) {
            TQE *cur = tqeRoot;
            PROC *p = cur->p;

            tqeRoot = cur->next;

            if(tqeFree){
                cur->next = tqeFree->next;
                tqeFree->next = cur;
            } else {
                tqeFree = cur;
                cur->next = cur;
            }

            kwakeup((int)p);

        }
    }

}

itoa(char *result, int number){
    if(number < 0){
        *result = '-';
        result++;
        number *= -1;
    }

    char buf[16];
    int place = 0;

    while(number > 0 && place < 16){
        buf[place] = number % 10 + 48;
        place++;
        number = number / 10;
    }



    for(; place > 0; place--){
        *result = buf[place - 1];
        result++;
    }

    *result = '\0';
}

void printTqe(){
    TQE *cur = tqeRoot;
    char queue[80];

    int i = 0;
    for(; i < 80; i++){
        queue[i] = '\0';
    }

    while(cur){
        strncat(queue, 80, "[t=");
        char itoaBuf[16];

        itoa(itoaBuf, cur->relativeTime);

        strncat(queue, 80, itoaBuf);
        strncat(queue, 80, ", pid=");

        itoa(itoaBuf, (int) cur->p);

        strncat(queue, 80, itoaBuf);
        strncat(queue, 80, "]->");
        cur = cur->next;
    }

    strncat(queue, 80, "[null]");

    i = 0;
    for(; i < 79; i++){
        kpchar(queue[i], 0, i);
    }
}

void tqeprint(){
    TQE *cur = tqeRoot;
    char queue[80];

    int i = 0;
    for(; i < 80; i++){
        queue[i] = '\0';
    }

    while(cur){
        strncat(queue, 80, "[t=");
        char itoaBuf[16];

        itoa(itoaBuf, cur->relativeTime);

        strncat(queue, 80, itoaBuf);
        strncat(queue, 80, ", pid=");

        itoa(itoaBuf, cur->relativeTime);

        strncat(queue, 80, itoaBuf);
        strncat(queue, 80, "]->");
        cur = cur->next;
    }

    strncat(queue, 80, "[null]\n\r");

    kprintf(queue);
}

void timer_init()
{
    int i;
    TIMER *tp;
    kprintf("timer_init()\n");

    tqeInit();

    for (i=0; i<4; i++){
        tp = &timer[i];
        if (i==0) tp->base = (u32 *)0x101E2000;
        if (i==1) tp->base = (u32 *)0x101E2020;
        if (i==2) tp->base = (u32 *)0x101E3000;
        if (i==3) tp->base = (u32 *)0x101E3020;
        //printf("%d: %x %x %x\n", i, tp->base, tp->base+TLOAD, tp->base+TVALUE);
        *(tp->base+TLOAD) = 0x0;   // reset
        *(tp->base+TVALUE)= 0xFFFFFFFF;
        *(tp->base+TRIS)  = 0x0;
        *(tp->base+TMIS)  = 0x0;
        *(tp->base+TLOAD) = 0x100;
        *(tp->base+TCNTL) = 0x62; //011-0000=|NOTEn|Pe|IntE|-|scal=00|32-bit|0=wrap|
        *(tp->base+TBGLOAD) = 0x0F00;

        tp->tick = tp->hh = tp->mm = tp->ss = 0;
        strcpy((char *)tp->clock, "00:00:00");
    }
}

void timer_handler(int n) {
    u32 ris, mis, value, load, bload, i;
    TIMER *t = &timer[n];

    t->tick++;

    if (t->tick >= 128 * 2){
        t->tick = 0; t->ss++;
        if (t->ss >= 60){
            t->ss = 0; t->mm++;
            if (t->mm >= 60){
                t->mm = 0; t->hh++;
            }
        }
        tqeIncrement();
        printTqe();
    }

    if (t->tick == 0){
        for (i=0; i<8; i++){
            unkpchar(t->clock[i], n, 70+i);
        }
        t->clock[7]='0'+(t->ss%10); t->clock[6]='0'+(t->ss/10);
        t->clock[4]='0'+(t->mm%10); t->clock[3]='0'+(t->mm/10);
        t->clock[1]='0'+(t->hh%10); t->clock[0]='0'+(t->hh/10);

        color = n+1;
        for (i=0; i<8; i++){
            kpchar(t->clock[i], n + 1, 70+i);
        }
    }

    timer_clearInterrupt(n);
    return;
}

void timer_start(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];

    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base+TCNTL) |= 0x80;  // set enable bit 7
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    *(tp->base+TCNTL) &= 0x7F;  // clear enable bit 7
}

void doT(PROC *p){
    kprintf("Please enter a number of seconds to sleep on");
    char line[16];
    kgets(line);
    int n = atoi(line);
    tqeInsert(n, p);
}