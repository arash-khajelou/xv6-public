#include "types.h"
#include "stat.h"
#include "user.h"

void hello(void) { printf(1, "Hello, xv6 world!\n"); }

int main(int argc, char *argv[]) {
  hello();
  exit();
}
