/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define _GNU_SOURCE 
#include <stdio.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"
#include "namespace.h"


static int
pivot_root(const char *new_root, const char *put_old)
{
    return syscall(SYS_pivot_root, new_root, put_old);
}

static void
create_dir(char *path) {
    struct stat sb = {0};
    if (stat(path, &sb) == -1) {
        if (mkdir(path, 0777) == -1) {
            errExit("==> Could not create directory");
        }
    }
}

int 
fs_mount(struct child_config* config){
    // new_root and old_root should be in the same hierachy where
    // old_root is under new_root
    char *old_rt = "/alpine";
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", config->mount_dir, old_rt);

    if (mount(NULL, "/", "NULL", MS_SLAVE | MS_REC, NULL) == -1) {
        errExit("==> Could not remount as private");
    }
    
    // Create path
    create_dir(path);

    // make oldroot a bind mount
    if (mount(path, path, "NULL", MS_BIND, NULL) == -1) {
        errExit("==> Could not bind mount");
    }

    // Move Bind mount to new_root
    if (mount(path, config->mount_dir, "NULL", MS_MOVE, NULL) == -1) {
        errExit("==> Could not move mount");
    }
    
    if (pivot_root(config->mount_dir, config->mount_dir) == -1) {
        errExit("==> Could not change root");
    }
    
    if (chdir("/") == -1) {
        errExit("==> Unable to change directory");
    }

    create_dir("/proc");
    mount("proc", "/proc", "proc", 0, NULL);
}