/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

void file_open(int *fd, char *f_name);
void file_write(int *fd, char *content);

int fs_mount(
//    char *src,
//    char *target,
//    char *fstype, 
//    unsigned long flags,
//    const void *data
);