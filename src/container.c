#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include "namespace.h"


void wait_parent(int wstatus, pid_t child_pid) {
    pid_t w = waitpid(child_pid, &wstatus, WUNTRACED | WCONTINUED);
    if (w == -1) {
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

struct child_config
{
  int argc;
  char **argv;
};


static char child_stack[1048576];

// user namespace
static void user_namespace();

// mounts
static void mount();

// pivot root

// network

static int child_fn(void *args) {
    struct child_config *config = args;
    system("hostname raggamuffin");
    if (execvp(config->argv[0], config->argv))
      fprintf(stderr, "execvp failed: %m\n");
      return -1;
    return 0;
}

int main(int argc, char *argv[]){
    if (argc < 2)
        goto arg_cnt_err;
    pid_t child_pid;
    struct child_config config = {argc, &argv[1]};  // remove first argument(file to execute)
    int wstatus;
    int ns_types = CLONE_NEWNS
      | CLONE_NEWCGROUP
      | CLONE_NEWUTS
      | CLONE_NEWIPC
      | CLONE_NEWNET
      | CLONE_NEWPID;

    if ((child_pid = clone(child_fn, child_stack+1048576, ns_types | SIGCHLD, &config)) == -1) {
      fprintf(stderr, "=> clone failed! %m\n");
      exit(EXIT_FAILURE);
    }
    printf("clone() = %ld\n", (long)child_pid);

    goto w_parent;

arg_cnt_err:
    fprintf(stderr, "=> Too few arguments. No executable provided. Exiting...\n");
    exit(EXIT_FAILURE);

w_parent:
    wait_parent(wstatus, child_pid);

    return 0;
}
