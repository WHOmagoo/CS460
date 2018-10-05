//
// Created by whomagoo on 10/4/18.
//

PIPE pipe[NPROC];
int available[NPROC];
PIPE *pipeFree;

void pipe_init(){
    int i;
    PIPE *pipe;
    for(i = 0; i < NPROC; i++){
        available[i] = 1;
    }
}

PIPE* create_pipe(){
    int i = 0;
    for(; i < NPROC; i++){
        if(available[i]){
            pipe[i].status = READY;
            pipe[i].head = 0;
            pipe[i].tail = 0;
            pipe[i].data = 0;
            pipe[i].room = PSIZE;
            available[i] = 0;
            return &(pipe[i]);
        }
    }

    return 0;
}



int read_pipe(PIPE *p, char *buf, int n){
    int r = 0;
    if (n<=0)
        return 0;

    if(p->status != FREE) { // p->status must not be FREE
        while (n) {
            lock();
            while (p->data) {
                *buf++ = p->buf[p->tail++]; // read a byte to buf
                p->tail %= PSIZE;
                p->data--;
                p->room++;
                r++;
                n--;
//
                if (n == 0)
                    break;
            }
//
//            kprintf("Waking up Writer %d\n\r", &(p->room));
            kwakeup((int) &(p->room)); // wakeup writers

            if (r) { // if has read some data
                unlock();
                return r;
            }

            unlock();
            ksleep((int) &(p->data));
//
            // pipe has no data
        }

        printf("Got out somehow");
        ksleep((int) &(p->data)); // sleep for data
    } else {
        kprintf("Trying to read pipe on invalid p");
        return;
    }
}

int write_pipe(PIPE *p, char *buf, int n)
{
    kprintf("Writing to pipe\n\r");
    int r = 0;
    if (n<=0) {
        kprintf("nothing to add");
        return 0;
    }

    if(p && p->status != FREE) { // p->status must not be FREE
        lock();
        while (n) {
            while (p->room > 0) {
                p->buf[p->head++] = *buf++; // write a byte to pipe;
                p->head %= PSIZE;
                p->data++;
                p->room--;
                r++;
                n--;
                if (n == 0)
                    break;
            }

            kwakeup( (int) &(p->data));
            // wakeup readers, if any.

            if (n == 0) {
                kprintf("***Finished writing all bytes***\n");
                // finished writing n bytes
                unlock();
                return r;
            }
            // still has data to write but pipe has no room

            unlock();
            ksleep((int) &(p->room));
            lock();
            kprintf("Resuming after space has filled %d cur buf = %s\n\r", p->room, buf);
            // sleep for room
        }
    } else {
        kprintf("Trying to write pipe on invalid p");
    }
}