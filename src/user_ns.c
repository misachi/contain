#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#include "namespace.h"


static void handle_map(char *f_name, char *mapping) {
    int fd;
    file_open(&fd, f_name);
    file_write(&fd, mapping);
    close(fd);
}

// user namespace
void user_namespace(pid_t _pid) {
    uid_t _uid = getuid();
    gid_t _gid = getgid();
    int fd;
    char f_name[FILE_SIZE];
    char mapping[FILE_SIZE];

    // Handle uid mappings
    snprintf(f_name, PATH_MAX, "/proc/%ld/uid_map", (long)_pid);
    snprintf(mapping, sizeof(mapping), "0 %ld 1", (long) _uid);
    handle_map(f_name, mapping);

    /*
    Update setgroups file since (from docs) calls to setgroups(2) are
    also not permitted if /proc/[pid]/gid_map has not yet been set
    **/
    snprintf(f_name, PATH_MAX, "/proc/%ld/setgroups", (long)_pid);
    snprintf(mapping, sizeof(mapping), "deny");
    handle_map(f_name, mapping);

    // Handle gid mappings
    snprintf(f_name, PATH_MAX, "/proc/%ld/gid_map", (long)_pid);
    snprintf(mapping, sizeof(mapping), "0 %ld 1", (long) _gid);
    handle_map(f_name, mapping);
}