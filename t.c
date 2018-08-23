/*********  t.c file *********************/

int prints(char *s)
{
    while(*s) {
        putc(*s);
        s++;
    }
}

int gets(char *s)
{
    do {
        *s = getc();
        putc(*s);
        *s++;
    } while(*(s -1) != '\r');

    *(s-1) = '\0';
}

int strcmp(char *s1, char *s2){
    while(1){
        if(*s1 == *s2){
            if(*s1 == '\0'){
                return 1;
            }
            s1++;
            s2++;
        } else {
            return 0;
        }
    }
}

char ans[64];

main()
{
    while(1){
        prints("What's your name? ");
        gets(ans);  prints("\n\r");

        if (ans[0]==0 || strcmp(ans, "quit")){
            prints("return to assembly and hang\n\r");
            return;
        }
        prints("Welcome "); prints(ans); prints("\n\r");
    }
}

