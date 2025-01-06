#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  for(int j = 0; j < 10000; j ++) {
    j ++;
    if (j == 100) {
      printf("Hello World from Navy-apps for the %dth time!\n", i++);
      j = 0;
    }
  }
  return 0;
}
