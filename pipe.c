//
// Created by whomagoo on 10/4/18.
//

#define PSIZE 16;

PIPE pipe[NPROC];
int available[NPROC];
PIPE *pipeFree;

void pipeInit(){
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
            available[i] = 0;
            return &pipe[i];
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
            while (p->data) {
                *buf++ = p->buf[p->tail++]; // read a byte to buf
                p->tail %= SSIZE;
                p->data--;
                p->room++;
                r++;
                n--;
                if (n == 0)
                    break;
            }
            kwakeup(&p->room); // wakeup writers
            if (r) // if has read some data
                return r;

            // pipe has no data
            sleep(&p->data); // sleep for data
        }
    } else {
        kprintf("Trying to read pipe on invalid p");
        return;
    }
}

int write_pipe(PIPE *p, char *buf, int n)
{
    int r = 0;
    if (n<=0)
        return 0;
    if(p && p->status != FREE) { // p->status must not be FREE
        while (n) {
            while (p->room)
                p->buf[p->head++] = *buf++; // write a byte to pipe;
            p->head %= PSIZE;
            p->data++;
            p->room--;
            r++;
            n--;
            if (n == 0)
                break;
        }
        kwakeup(&p->data);
        // wakeup readers, if any.

        if (n == 0) {
            return r;
        }
        // finished writing n bytes
        // still has data to write but pipe has no room
        sleep(&p->room);
        // sleep for room
    } else {
        kprintf("Trying to write pipe on invalid p");
    }
}