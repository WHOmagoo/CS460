
// queue.c file
extern PROC *freeList;

int enqueue(PROC **queue, PROC *p) {
    int SR = int_off();  // IRQ interrupts off, return CPSR

    PROC *cur = *queue;
    PROC *prev = 0;
    while (cur) {
        if (cur->priority < p->priority) {
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    if (prev == 0) {
        p->next = *queue;
        *queue = p;
    } else {
        p->next = cur;
        prev->next = p;
    }

    int_on(SR);          //  restore CPSR
}

PROC *dequeue(PROC **queue)
{
    int SR = int_off();  // IRQ interrupts off, return CPSR

    PROC *top = *queue;
    *queue = (*queue)->next;

    /*
    remove the FISRT element from *queue;
     */
    int_on(SR);          //  restore CPSR

    return top;
    //return pointer to dequeued PROC;
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

