#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "util.h"


void file_open(int *fd, char *f_name) {
    if ((*fd = open(f_name, O_RDWR)) == -1) {
        fprintf(stderr, "==> Read File error: %m\n");
        exit(EXIT_FAILURE);
    }
}

void file_write(int *fd, char *content) {
    /* 
    proc/[id]/setgroups requires that the buffer size match
    exact length of string(deny). Hence we use strlen to get size of the string
    **/
    if (write(*fd, content, strlen(content)) == -1) {
        fprintf(stderr, "==> Write file Error: %s\n", strerror(errno));
    }
}