/*
 * No copyright is claimed.  This code is in the public domain; do with
 * it what you wish.
 *
 * Compat code so unshare and setns can be used with older libcs
 */

#ifndef UTIL_LINUX_NAMESPACE_H
# define UTIL_LINUX_NAMESPACE_H
# include <sched.h>
#include <limits.h>

# ifndef CLONE_NEWNS
#  define CLONE_NEWNS 0x00020000
# endif
# ifndef CLONE_NEWCGROUP
#  define CLONE_NEWCGROUP 0x02000000
# endif
# ifndef CLONE_NEWUTS
#  define CLONE_NEWUTS 0x04000000
# endif
# ifndef CLONE_NEWIPC
#  define CLONE_NEWIPC 0x08000000
# endif
# ifndef CLONE_NEWNET
#  define CLONE_NEWNET 0x40000000
# endif
# ifndef CLONE_NEWUSER
#  define CLONE_NEWUSER 0x10000000
# endif
# ifndef CLONE_NEWPID
#  define CLONE_NEWPID 0x20000000
# endif

# ifndef CAP_AUDIT_READ
#   define CAP_AUDIT_READ 37
# endif

# ifndef FILE_SIZE
#   define FILE_SIZE 100
# endif

static char child_stack[1048576];
struct child_config
{
  int argc;
  char **argv;
  int fd[2];
  char *mount_dir;
  unsigned int ns_types;
  char *host_name;
};
static void handle_map(char *f_name, char *mapping);
void user_namespace(pid_t _pid);

#endif	/* UTIL_LINUX_NAMESPACE_H */