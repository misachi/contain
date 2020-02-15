#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include "namespace.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)


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

// mounts
static void mount2();

// pivot root

// network

// Capabilities

// syscalls(seccomp)

// apparmor

static int child_fn(void *args) {
    char ch;
    struct child_config *config = (struct child_config *) args;
    // sethostname("misachid", 9);
    close(config->fd[1]);

    if (read(config->fd[0], &ch, 1) != 0) {
        fprintf(stderr, "==> Read piped descriptor failed: %m\n");
        exit(EXIT_FAILURE);
    }

    close(config->fd[0]);

    if (execvp(config->argv[0], config->argv))
      fprintf(stderr, "==> execvp failed: %m\n");
      return -1;
    return 0;
}

int main(int argc, char *argv[]){
    pid_t child_pid;
    struct child_config config = {
        .argc = argc,
        .argv = &argv[1]
    };  // remove first argument(file to execute)

    int wstatus;
    int ns_types = CLONE_NEWUSER
        | CLONE_NEWNS
        | CLONE_NEWCGROUP
        | CLONE_NEWUTS
        | CLONE_NEWIPC
        | CLONE_NEWNET
        | CLONE_NEWPID;

    if (pipe(config.fd) == -1)
        errExit("pipe");
    if ((child_pid = clone(child_fn, child_stack+1048576, ns_types | SIGCHLD, &config)) == -1) {
      fprintf(stderr, "=> clone failed! %m\n");
      exit(EXIT_FAILURE);
    }
    printf("clone() = %ld\n", (long)child_pid);

    user_namespace(child_pid);
    close(config.fd[1]);

    wait_parent(wstatus, child_pid);

    exit(EXIT_SUCCESS);
}
