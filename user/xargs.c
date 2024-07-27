#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int fd = 0;
// THINKING: TM的，C语言的这些数组也太难搞了，都需要初始化啥的...
// 从标准输入0读取参数，以空格和\n分割，遇到\n则返回，从argv的末尾（argc）开始加新的参数
int 
readline(int argc, char **argv) 
{
  // 从管道左侧读入
  char buf[1024];
  int n = 0;
  // 一个字符一个字符地读取，直到为'\n'或末尾
  while(read(fd, buf+n, 1) == 1) {
    if (n > 1023) {
      fprintf(2, "xargs: too much args!\n");
      exit(1);
    }
    if (buf[n] == '\n') break;
    n++;
  }
  buf[n] = 0;
  if(n == 0) return 0;
  // 找到空格分割的各个参数
  int offset = 0;
  while(offset < n) {
    // argv直接用局部变量buf会出问题！
    // 添加新的参数
    argv[argc++] = buf + offset;
    // 找到下一个空白处
    while(buf[offset] != ' ' && offset < n) {
      offset++;
    }
    buf[offset++] = 0;
    // 找到下一个非空白处
    while(buf[offset] == ' ' && offset < n) {
      buf[offset++] = 0;
    }
  }
  return argc;
}

int
main(int argc, const char *argv[])
{
  
  if(argc <= 1){
    fprintf(2, "Uasge: xargs command [argumens...]\n");
    exit(-1);
  }
  // 字符指针用的时候需要分配内存！分配内存！分配内存！
  // 实际的命令
  char *command = malloc(strlen(argv[1])+1);
  // strcpy会复制'\0'，所以无需检查
  strcpy(command, argv[1]);
  // 实际执行的参数, 每个元素都需要分配内存再使用
  char *new_argv[MAXARG];
  for(int i = 0; i < argc-1; i++) {
    new_argv[i] = malloc(strlen(argv[i+1])+1);
    strcpy(new_argv[i], argv[i+1]);
  }
  // TODO: 这一部分是参考的，有时间自己想下还有什么更好的解决方法
  // 思路：用read去读管道内容，遇到结束或\n就把去处理参数，然后执行
  int curr_argc = argc-1;
  while((curr_argc = readline(argc-1, new_argv)) != 0) {
    new_argv[curr_argc] = 0;
    if (fork() == 0) {
      exec(command, new_argv);
      fprintf(2, "xargs: execute error!\n");
      exit(1);
    }
    wait(0);
  }
  exit(0);
}
