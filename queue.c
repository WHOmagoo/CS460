
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


    //TODO possibly move this to ksleep and kwakeup


    //END

    /*
    remove the FISRT element from *queue;
     */
    int_on(SR);          //  restore CPSR

    return top;
    //return pointer to dequeued PROC;
}

void printChildren(char *name, PROC *p){
    PROC *curChild = p->child;

    kprintf("%s = ", name);

    while(curChild){
        kprintf("[%d%d]->", curChild->pid, p->priority);
        curChild = curChild->sibling;
    }

    kprintf("NULL\n");
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

void addChild(PROC *queue, PROC *child){
    if(child) {
        child->parent = queue;

        PROC *cur = queue->child;
        PROC *prev = 0;
        while (cur) {
            prev = cur;
            cur = cur->sibling;
        }

        if (prev) {
            prev->sibling = child;
        } else {
            queue->child = child;
        }
    }
}

PROC * findRoot(PROC *queue){
    PROC *prev = queue;
    while (prev != queue->parent){
        prev = queue;
        queue = queue->parent;
    }

    kprintf("Found root with pid of %d", queue->pid);
    return queue;
}

int removeChild(PROC *parent, PROC *child){
    PROC *curChild = parent->child;
    PROC *prevChild = 0;
    while(curChild){
        if(curChild->pid == child->pid){
            if(prevChild == 0){
                parent->child = curChild->sibling;
            } else {
                prevChild->sibling = curChild->sibling;
            }

            child->sibling = 0;
            child->parent = 0;

            return 1;
        }

        prevChild = curChild;
        curChild = curChild->sibling;
    }

    return 0;
}