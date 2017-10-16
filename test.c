#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "test.h"

int main(int argc, char **argv) {
    char *heap_str;

    heap_str = malloc((strlen(data_str)+1) * sizeof(char));
    strncpy(heap_str, data_str, CPLENGTH);
    heap_str[CPLENGTH] = '\0';

    printf("test's PID is %d\n", getpid());

    pr_all(data_str);
    pr_all(heap_str);
    getchar();
    pr_all(data_str);
    pr_all(heap_str);

    free(heap_str);

    return 0;
}
