#include <stdio.h>
#include <unistd.h>

int main() {
    for (int i = 0; i < 5; i++) {
        printf("going to sleep for 5s\n");
        sleep(5);
        printf("awake\n");
    }
}
