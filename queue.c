
// queue.c file
extern PROC *freeList;
// WRITE YOUR OWN functions:

int enqueue(PROC **queue, PROC *p)
{
    int SR = int_off();  // IRQ interrupts off, return CPSR
    enter p into *queue by priority; PROCs with the same priority by FIFO;
    int_on(SR);          //  restore CPSR
}				     }

PROC *dequeue(PROC **queue)
{
    int SR = int_off();  // IRQ interrupts off, return CPSR
    remove the FISRT element from *queue;
    int_on(SR);          //  restore CPSR
    return pointer to dequeued PROC;
}

int printList(char *name, PROC *p)
{
    kprintf("%s = ", name);
    while(p){
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    kprintf("NULL\n");
}

