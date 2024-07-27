#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 获取文件名或最后一个/后的名字
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}


void
find(char *path, char *pat);

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find directory pattern\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}

// 在指定path中搜索与pat相同的文件或目录
void
find(char *path, char *pat) 
{
  // printf("Path: %s Pattern: %s\n", path, pat);
  // 判断是否为文件夹，如果不是则看是不是含有pat，是则打印
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  // 打开文件，获取文件描述符
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  // 获取path的文件信息到st中
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  // 直接判断是否含有pattern
  char *filename = fmtname(path);
  // printf("%d %d \n", strlen(filename), strlen(pat));
  if (!strcmp(filename, pat)) {
    printf("%s\n", path);
  }
  // 如果是文件夹则递归进行find
  if (st.type == T_DIR) {
    // 路径过长的情况
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    // 给buf加上'/'，同时p移动到/之后方便添加新的文件名
    *p++ = '/';
    // 目录的fd里存储的应该就是dirent内容，不同从目录文件中读取entry
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      // 跳过inode为0的项
      if(de.inum == 0)
        continue;
      // 拼接，得到目录下文件的路径
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      // printf("Entry: %s\n", p);
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      // 跳过.,..目录
      char *dirname = fmtname(buf);
      // printf("Dir: %s\n", dirname);
      if (!strcmp(dirname, ".") || !strcmp(dirname, "..")) {
        continue;
      }
      // 直接递归搜索该目录下的所有项
      find(buf, pat);
    }
  }

  close(fd);
}