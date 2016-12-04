#include <stdio.h>
#include <unistd.h>

int main() {
    printf("i will try to read from an invalid file with fgets\n");
    char buff[50];

    char *ret = fgets(buff, 49, (FILE*) 0xdeadbeef);
    // should segfault here
    if (!ret) {
        printf("i could not read the input :( so bad\n");
    } else {
        printf("you did it! good boy\n");
    }
}
