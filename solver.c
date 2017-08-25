#include <stdio.h>
#include <string.h>

//W/O brackets first
//Spaces separating every thing, works for single digit


int main() {

    int buffer[255];
    int next = 0;
    int next_print = 0;
    char *string = "2*6/3*4*3/4";
    for (int i = 0; i < strlen(string); i++) {
        if ((string[i] >= 48 && string[i] <= 57) || i == strlen(string) - 1) {
            buffer[next++] = string[i] - '0';
            printf("buffer[next] = %d next is %d\n", buffer[next-1], next);
            if (next_print == 1) {
                printf("%d / %d\n", buffer[next-2], buffer[next-1]);
                buffer[next-1] = buffer[next-2] / buffer[next-1];
            } else if (next_print == 2) {
                printf("%d * %d\n", buffer[next-2], buffer[next-1]);
                buffer[next-1] = buffer[next-1] * buffer[next-2];
            }
        }
        next_print = 0;
        if (string[i] == '*') {
            next_print = 2;
        } else if (string[i] == '/') {
            next_print = 1;
        }
    }

    return 0;
}

/*
void print_buffer(char buf[]) {

    for (int i = 0; i < strlen(buf); i++) {
        printf("%c\n", buf[i]);
    }

}
*/
