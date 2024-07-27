#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 抽象为每一个筛和下一个筛的连接 
void child(int fd) {
  int n,m;
  // 最后一个筛所创建的子进程
  if (read(fd, &n, 4) == 0) {
    close(fd);
    exit(0);
  }
  printf("prime %d\n", n);
  /* debug */
  // printf("prime %d fd: %d\n", n, fd);
  int p[2];
  pipe(p);
  if (fork() == 0) {
    close(p[1]);
    child(p[0]);
  }else {
    close(p[0]);
    while(read(fd, &m, 4) == 4) {
      if (m % n == 0) continue;
      // Send to next process
      write(p[1], &m, 4);
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}

// My thrid xv6 program.
int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  if (fork() == 0) {
    close(p[1]);
    child(p[0]);
  }else {
    // 写入2-35给下一个筛
    close(p[0]);
    for(int i = 2; i <= 35; i++) {
      // Send to next process
      write(p[1], &i, 4);
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
