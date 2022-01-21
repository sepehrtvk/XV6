#include "types.h"
#include "stat.h"
#include "user.h"

// number of children created
#define NUM 4


int main(int argc, char *argv[])
{
    changePolicy(1);
    
    // int turnaroundsSUM = 0; 
    // int waitingsSUM = 0;    
    // int CBTsSUM = 0;    


    int main_pid = getpid();
    // int sums[3]={0};    


    //make NUM child process
    for (int i = 0; i < NUM; i++)
    {
        if (fork() > 0)
            break;
    }

    if (main_pid != getpid())
    {
        //print pid with i
        for (int i = 0; i < 8; i++){
            int pid = getpid();
            printf(1, "PID=/%d/ : i=/%d/\n",pid , i);
        }

        int thisPid = getpid(); 

        //wait();
        int turnAroundTime = getTurnAroundTime(thisPid);
        int waitingTime = getWaitingTime(thisPid);
        int cbpTime = getCBT(thisPid);

        // sums[0]+=turnAroundTime;
        // sums[1]+=waitingTime;
        // sums[2]+=cbpTime;


        printf(1, " Process ID : %d\n", thisPid);
        printf(1,"--------------------------\n");
        printf(1, "| TurnAround Time = %d  | \n", turnAroundTime);
        printf(1, "| Waiting Time = %d     | \n", waitingTime);
        printf(1, "| CPU Burst Time = %d    | \n", cbpTime);
        printf(1, "\n\n");


    }
    //print average

    //wait to finish
      while (wait() != -1);
    // wait();
    // printf(1,"--------------------------\n");
    // printf(1, "| TurnAround Time Average = %d  | \n", sums[0]/NUM);
    // printf(1, "| Waiting Time Average = %d     | \n", sums[1]/NUM);
    // printf(1, "| CPU Burst Time Average = %d    | \n", sums[2]/NUM);
    // printf(1, "\n\n");

    
    exit();
}