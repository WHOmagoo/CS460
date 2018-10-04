// YOUR string.c file for

/*****
  int strcpy(char *dest, char *src) : copies string src to dest
  int strncpy(char *dest, char *src, int n) : copies only n bytes
  int strncmp(char *s1, char *s2, int n) : compares n bytes
  int memcpy(char *dest, char *src, int n) : copies b bytes from src to dest
  int memset(char *dest, char c, int n): fills dest with n CONSTANT bytes of c
*****/

int strlen(char *s){
    int counter = 0;
    while(*s){
        counter++;
        s++;
    }

    return counter;
}

int strcmp(char *s1, char *s2){
    while(*s1 && *s2){
        if(*s1 != *s2) {
            return 1;
        }

        s1++;
        s2++;
    }

    return *s1 != *s2;
}

int strncpy(char *dest, char *src, int n){
    while(n > 0){
        *dest = *src;
        dest++;
        src++;
        n--;
    }
}

int strncmp(char *s1, char *s2, int n){
    while(*s1 && *s2 && n > 0){
        if(*s1 != *s2){
            return 1;
        }

        s1++;
        s2++;
        n--;
    }

    return n == 0 || *s1 != *s2;
}

int strcpy(char *dest, char *src){
    while(*src){
        *dest = *src;
        src++;
        dest++;
    }
}

int memcpy(char *dest, char *src, int n) {
    while(n > 0){
        *dest = *src;
        dest++;
        src++;
        n--;
    }
}
int memset(char *dest, char c, int n){
    while(n > 0){
        *dest = c;
        dest++;
        n--;
    }
}
