#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
// sys_pgaccess actual function
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va, addr; // start va and bitmask address
  int page_nums;      // number of pages to check

  if(argaddr(0, &va) < 0 || argint(1, &page_nums) < 0 || argaddr(2, &addr) < 0)
    return -1;
  // 最多只能查找64个页
  if (page_nums > 64) 
    return -1;

  uint64 bitmask = 0x0;
  pagetable_t pagetable = myproc()->pagetable;
  for (int i = 0; i < page_nums; i++, va += PGSIZE) {
    if (va > MAXVA) 
      return -1;
    
    pte_t *p = walk(pagetable, va, 0);
    // 页表项不存在，触发page_fault
    if (!p)
      return -1;
    // 页表项存在，同时Access标志位位1
    if (*p & PTE_A) {
      // 给掩码的第i位设置为1
      bitmask |= (1 << i);
      // 重置PTE_A标志位
      *p &= ~PTE_A;
    }
  }
  // 将bitmask拷贝回用户空间
  if (copyout(pagetable, addr, (char *)&bitmask, sizeof(bitmask)) < 0) 
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
