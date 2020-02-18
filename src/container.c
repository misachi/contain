#define _GNU_SOURCE 
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mount.h>

#include "namespace.h"
#include "util.h"


// Usage: ./contain.out -i -n -p -c -U -m /tmp/images/ -u misachi /bin/ash

void wait_parent(int wstatus, pid_t child_pid) {
    pid_t w;
    if ((w = waitpid(child_pid, &wstatus, WUNTRACED | WCONTINUED)) == -1) {
        perror("waitpid\n");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(wstatus)) {
        printf("exited, status=%d\n", WEXITSTATUS(wstatus));
    } else if (WIFSIGNALED(wstatus)) {
        printf("killed by signal %d\n", WTERMSIG(wstatus));
    } else if (WIFSTOPPED(wstatus)) {
        printf("stopped by signal %d\n", WSTOPSIG(wstatus));
    } else if (WIFCONTINUED(wstatus)) {
        printf("continued\n");
    }
}

// Cgroups

// network

// Capabilities

// syscalls(seccomp)

// apparmor

static int child_fn(void *args) {
    struct child_config *config = (struct child_config *) args;
    sethostname(config->host_name, strlen(config->host_name));

    if (fs_mount((struct child_config *) args) == -1) {
        fprintf(stderr, "Mount Error: %m\n");
        exit(EXIT_FAILURE);
    }

    char ch;
    close(config->fd[1]);

    if (read(config->fd[0], &ch, 1) != 0) {
        fprintf(stderr, "==> Read piped descriptor failed: %m\n");
        exit(EXIT_FAILURE);
    }

    close(config->fd[0]);

    if (execvp(config->argv[0], config->argv))
      errExit("==> execvp Failed");
    return 0;
}


static void prepare_args(struct child_config* config, int argc, char *argv[]){
    int opt;
    int count = 1;
    while ((opt = getopt(argc, argv, "inpcUm:u:")) != -1) {
        switch(opt) {
            case 'p':
                config->ns_types |= CLONE_NEWPID;
                break;
            case 'i':
                config->ns_types |= CLONE_NEWIPC;
                break;
            case 'u':
                config->ns_types |= CLONE_NEWUTS;
                if (!(optarg)) {
                    errExit("No hostname provided");
                }
                config->host_name = optarg;
                break;
            case 'm':
                config->ns_types |= CLONE_NEWNS;
                if (!(optarg)) {
                    errExit("No filepath mount");
                }
                config->mount_dir = optarg;
                break;
            case 'c':
                config->ns_types |= CLONE_NEWCGROUP;
                break;
            case 'n':
                config->ns_types |= CLONE_NEWNET;
                break;
            case 'U':
                config->ns_types |= CLONE_NEWUSER;
                break;
            default:
                exit(EXIT_FAILURE);
        }
        count++;
    }
}

int main(int argc, char *argv[]){
    pid_t child_pid;
    struct child_config config = {
        .argc = argc - 1,
    };  // remove first argument(file to execute)
    
    int wstatus;
  
    prepare_args(&config, argc, argv);
    config.argv = &argv[optind];
  
    if (pipe(config.fd) == -1)
        errExit("pipe");

    if ((child_pid = clone(child_fn, child_stack+1048576, config.ns_types | SIGCHLD, &config)) == -1) {
      fprintf(stderr, "=> clone failed! %m\n");
      exit(EXIT_FAILURE);
    }
 
    printf("clone() = %ld\n", (long)child_pid);

    user_namespace(child_pid);
    close(config.fd[1]);

    wait_parent(wstatus, child_pid);

    exit(EXIT_SUCCESS);
}
