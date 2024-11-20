#include "common.h"
#include "syscall.h"

void sys_yield();
void sys_exit(int code);
int sys_write(int fd, void *buf, size_t count);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  
  printf("a[0] = %d\n", a[0]);
  switch (a[0]) {
   
    case SYS_yield:
      printf("SYS_yield\n");
      sys_yield();
      c->GPRx = 0;
      break;
    case SYS_exit:
      sys_exit(a[1]);
      break;
    case SYS_write:
      printf("SYS_write\n");
      c->GPRx = sys_write(a[1], (void*)(a[2]), a[3]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

void sys_yield() {
  _yield();
}

void sys_exit(int code) {
  _halt(code);
}

int sys_write(int fd, void *buf, size_t count) {
  // printf("%s\n", (char*)buf);
  if(fd == 1 || fd == 2) {
    for(int i = 0; i < count; i++) {
      _putc(((char*)buf)[i]);
    }
    return count;
  }
  return 0;
}