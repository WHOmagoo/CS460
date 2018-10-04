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

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf

typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);
int body();

int init()
{
    int i, j;
    PROC *p;
    kprintf("kernel_init()\n");
    for (i=0; i<NPROC; i++){
        p = &proc[i];
        p->pid = i;
        p->status = READY;
        p->next = p + 1;
    }
    proc[NPROC-1].next = 0; // circular proc list
    freeList = &proc[0];
    readyQueue = 0;

    printf("create P0 as initial running process\n");
    p = dequeue(&freeList);
    p->priority = 0;
    p->ppid = 0; p->parent = p;  // P0's parent is itself
    running = p;
    kprintf("running = %d\n", running->pid);
    printList("freeList", freeList);
}

int kexit(int exitValue)
{
    printf("proc %d kexit\n", running->pid);
    if(running->pid != 1) {
        running->status = ZOMBIE;
        running->exitCode = exitValue;
        if(running->child) {
            addChild(&proc[1], running->child);

            PROC *curChild = running->child->sibling;
            while(curChild) {
                curChild->parent = &proc[1];
                curChild = curChild->sibling;
            }

            //wakeup p1 because it was given children
            kwakeup(&proc[1]);
        }

        kwakeup(running->parent);
//        enqueue(&freeList, running);   // putproc(running);
        tswitch();
    } else {
        printf("p1 never dies!!!\n");
    }
}

int ksleep(int event) {
    int SR = int_off();
// disable IRQ and return CPSR
    kprintf("Zleep \\%d/ for %d", running->pid, event);
    running->event = event;
    running->status = SLEEP;
    enqueue(&sleepList, running);
    tswitch(); // switch process
    int_on(SR); // restore original CPSR
}

int kwakeup(int event)
{
    int SR = int_off();
// disable IRQ and return CPSR

    PROC *p = sleepList;

    kprintf("Waking up on %d = {", event);

    kprintf("{\n\r");
    while(p){
        kprintf("status = %d, event = %d, pid %d\n\r", p->status, p->event, p->pid);
        if (p->status==SLEEP && p->event==event){
            p->status = READY;
            enqueue(&readyQueue, p);
            kprintf("\\%d/] ", p->pid);
        }
        p = p->next;
    }

    kprintf("}\n\n\r");
    int_on(SR);
// restore original CPSR
}

PROC *kfork(int func, int priority)
{
    int i;
    PROC *p = dequeue(&freeList);
    if (p==0){
        printf("no more PROC, kfork failed\n");
        return 0;
    }
    p->status = READY;
    p->priority = priority;
    p->ppid = running->pid;
    p->parent = running;

    addChild(running, p);

    // set kstack to resume to body
    // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
    //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
    for (i=1; i<15; i++)
        p->kstack[SSIZE-i] = 0;
    p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
    p->ksp = &(p->kstack[SSIZE-14]);
    enqueue(&readyQueue, p);
    printf("%d kforked a child %d\n", running->pid, p->pid);
    printList("readyQueue", readyQueue);
    return p;
}

int kwait(int *status){

    if(!(running->child)){
        printf("Waiting for no children\n");
        return -1;
    }

    PROC *child = running->child;

    while (child) {
        if (child->status == ZOMBIE) {
            (*status) = child->exitCode;
            removeChild(child->parent, child);

            child->status = FREE;
            enqueue(&freeList, child);

            return child->pid;
        }

        child = child->sibling;
    }

    ksleep((int) running);
}

int scheduler()
{
    kprintf("proc %d in scheduler ", running->pid);
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    kprintf("next running = %d\n", running->pid);
}

void doExit(){
        kprintf("Input a number for exit code: ");
        char numberString[16];
        kgets(numberString);
        int n = atoi(numberString);
        kexit(n);
}

int atoi(char *string){
    int result = 0;
    while(*string){
        if(*string >= 48 && *string <= 57){
            result = result * 10 + *string - 48;
        }
        string++;
    }

    return result;
}

void doWait(){
    int status;
    int pid = kwait(&status);
    printf("ZOMBIE child pid %d, exit code %d\n", pid, status);
}

int body(int pid, int ppid, int func, int priority)
{

    kprintf("pid = %d, ppid = %d, func = %x, priority = %d\n", pid, ppid, func, priority);

    char c; char line[64];
    //int pid;
    kprintf("proc %d resume to body()\n", running->pid);
    while(1){
        pid = running->pid;
        if (pid==0) color=BLUE;
        if (pid==1) color=WHITE;
        if (pid==2) color=GREEN;
        if (pid==3) color=CYAN;
        if (pid==4) color=YELLOW;
        if (pid==5) color=WHITE;
        if (pid==6) color=GREEN;
        if (pid==7) color=WHITE;
        if (pid==8) color=CYAN;


        printList("readyQueue", readyQueue);
        printChildren("Children", running);
        kprintf("proc %d running, parent = %d  ", running->pid, running->ppid);
        kprintf("input a char [s|f|q|w] : ");
        c = kgetc();
        printf("%c\n", c);

        switch(c){
            case 's': tswitch();            break;
            case 'f': kfork((int)body, 1);  break;
            case 'q': doExit();             break;
            case 'w': doWait();             break;
        }
    }
}
