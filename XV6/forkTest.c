#include "types.h"
#include "stat.h"
#include "user.h"

int x = 0;

int main() {
  int tid = fork();

  if (tid < 0) {
    printf(2, "error!\n");
  } else if (tid == 0) {
    // child
    for(;;) {
      x++;
      sleep(100);
    }
  } else {
    // parent
    for(;;) {
      printf(1, "x = %d\n", x);
      sleep(100);
    }
  }

  exit();
}