#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// My second xv6 program.
int
main(int argc, char *argv[])
{
  if (argc != 1) {
    fprintf(2, "Usage: pingpong\n");
    exit(1);
  }

  int pid;
  int p[2];
  pipe(p);

  // 添加错误处理
  if (fork() == 0) {
    // child
    pid = getpid();
    char buf[2];
    if (read(p[0], buf, 1) != 1) {
      fprintf(2, "Child failed to read.\n");
      exit(1);
    }
    close(p[0]);
    printf("%d: received ping\n", pid);
    if (write(p[1], buf, 1) != 1) {
      fprintf(2, "Child failed to write.\n");
      exit(1);
    }
    close(p[1]);
  }else{
    pid = getpid();
    // A empty byte: 00
    char byte[2] = "H";
    char buf[2];
    buf[1] = 0;
    // send to child
    if (write(p[1], byte, 1) != 1) {
      fprintf(2, "Parent failed to write.\n");
      exit(1);
    }
    close(p[1]);
    // 等待子进程read并write，如果不wait可能会出现“僵尸进程”！即便理论上read也会阻塞
    wait(0);
    if (read(p[0], buf, 1) != 1) {
      fprintf(2, "Parent failed to read.\n");
      exit(1);
    }
    printf("%d: received pong\n", pid);
    close(p[0]);
  }

  exit(0);
}
