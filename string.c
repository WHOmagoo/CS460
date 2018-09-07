//
// Created by whomagoo on 9/6/18.
//

int strlen(char *s){
    int counter = 0;
    while(*s){
        counter++;
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