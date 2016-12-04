#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("calling malloc for me!\n");
    char * buff = malloc(sizeof(char)*10);
    sprintf(buff, "test");
    printf("ok, my buff has: %s\n", buff);
    free(buff);
}
