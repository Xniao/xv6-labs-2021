#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// My first xv6 program. However, I may do it for 3 times already...🙄
int
main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(2, "Usage: sleep seconds\n");
    exit(1);
  } 
  int n = atoi(argv[1]);
  sleep(n);
  exit(0);
}
