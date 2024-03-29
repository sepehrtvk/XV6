#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;                   
  release(&tickslock);
  return xticks;
}

int sys_getHelloWorld(void) {
  return getHelloWorld();
}

int sys_getProcCount(void) {
  return getProcCount();
}

extern int readCount;

int sys_getReadCount(void) {
  // return getReadCount();
  cprintf("Number of Read  : %d\n",readCount);
  return readCount;
}

int sys_thread_create(void){
  int stackptr = 0;
  if(argint(0,&stackptr) < 0 ){
    return -1;
  }
  return thread_create((void*) stackptr);
}

int sys_thread_wait(void){
  return thread_wait();
}

int
sys_cps(void)
{
  return cps();
}

int sys_changePolicy(void)
{
  int newPolicy;
  if (argint(0, &newPolicy) < 0)
    return -1;
  else
    return changePolicy(newPolicy);
}

int sys_getTurnAroundTime(void)
{
  int pid;
  if (argint(0, &pid) < 0)
    return -1;
  else
    return getTurnAroundTime(pid);
}

int sys_getWaitingTime(void)
{
  int pid;
  if (argint(0, &pid) < 0)
    return -1;
  else
    return getWaitingTime(pid);
}

int sys_getCBT(void)
{
  int pid;
  if (argint(0, &pid) < 0)
    return -1;
  else
    return getCBT(pid);
}

int sys_setPriority(void)
{
  int newPriority;
  if (argint(0, &newPriority) < 0)
    return -1;
  else
    return setPriority(newPriority);
}

int sys_wait2(void) {
  int *turnAroundtime,  *waitingtime,  *cbttime , *pario;
  if (argptr(0, (void*)&turnAroundtime, sizeof(turnAroundtime)) < 0)
    return -1;
  if (argptr(1, (void*)&waitingtime, sizeof(waitingtime)) < 0)
    return -1;
  if (argptr(2, (void*)&cbttime, sizeof(cbttime)) < 0)
    return -1;
  if (argptr(3, (void*)&pario, sizeof(pario)) < 0)
    return -1;
  return wait2(turnAroundtime,  waitingtime,  cbttime , pario);
}

int sys_setQueue(void)
{
  int queueNum;
  if (argint(0, &queueNum) < 0)
  {    
    return -1;
  }
  else
    return setQueue(queueNum);

}