#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER){
    switch (policy)
    {
    case DEFAULT:
      yield();
      break;

    case ROUND_ROBIN:
      // if (myproc()->rrRemainingTime == 0)
      // {
      //   // Reset remaining time back to quantum
      //   myproc()->rrRemainingTime = QUANTUM;
      //   yield();
      // }
      // else
      // {
      //   myproc()->rrRemainingTime--;
      // }
      if(ticks % QUANTUM==0){
        yield();
      }
      break;

    case MULTILAYRED_PRIORITY:

      switch (myproc()->queue){
      case 1:
        if( ticks % (QUANTUM+6) == 0 ){
          yield();
        }
        break;

      case 2:
        if( ticks % (QUANTUM+5) == 0 ){
          yield();
        }
        break;

      case 3:
        if( ticks % (QUANTUM+4) == 0 ){
          yield();
        }
        break;

      case 4:
        if( ticks % (QUANTUM+3) == 0 ){
          yield();
        }
        break;

      case 5:
        if( ticks % (QUANTUM+2) == 0 ){
          yield();
        }
        break;

      case 6:
        if( ticks % (QUANTUM+1) == 0 ){
          yield();
        }
        break;
      
      default:
        yield();
        break;
      }
      
    break;

    case DYNAMIC_MULTILAYER_PRIOITY:
        switch (myproc()->queue){
      case 1:
        if( ticks % (QUANTUM+6) == 0 ){
          //  myproc()->queue++;
          setQueue(myproc()->queue+1);
          yield();
         
        }
        break;

      case 2:
        if( ticks % (QUANTUM+5) == 0 ){
          setQueue(myproc()->queue+1);
          yield();
        }
        break;

      case 3:
        if( ticks % (QUANTUM+4) == 0 ){
           setQueue(myproc()->queue+1);
          yield();
        }
        break;

      case 4:
        if( ticks % (QUANTUM+3) == 0 ){
           setQueue(myproc()->queue+1);
          yield();
        }
        break;

      case 5:
        if( ticks % (QUANTUM+2) == 0 ){
           setQueue(myproc()->queue+1);
          yield();
        }
        break;

      case 6:
        if( ticks % (QUANTUM+1) == 0 ){
          setQueue(myproc()->queue+1);
          yield();
        }
        break;
      
      default:
        setQueue(myproc()->queue+1);
          yield();
        break;
      }
    break;

    case PRIORITY:
      yield();
      break;
    }
}

  // Update each process sleeping, runnable, running time
  if (tf->trapno == T_IRQ0 + IRQ_TIMER)
  {
    updateTimes();
  }

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
