#include <stdio.h>
#include <sys/statvfs.h>

#if defined(__linux__)
#include <sys/vfs.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/mount.h>
#endif

#ifndef PROC_SUPER_MAGIC
#define PROC_SUPER_MAGIC 0x9fa0UL
#endif

int main(void) {
  struct statfs s;
  if (statfs("/proc", &s) != 0) {
    printf("proc_fs=unavailable\n");
    return 0;
  }
  if ((unsigned long)s.f_type == PROC_SUPER_MAGIC) {
    printf("proc_magic=0x%lx\n", (unsigned long)s.f_type);
    return 0;
  }
  printf("proc_magic=other (0x%lx)\n", (unsigned long)s.f_type);
  return 0;
}
