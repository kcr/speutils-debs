
#define SPU_DMALIB_STATIC 1

#include <speutils/spefifo.h>
#include <speutils/spu_dmalib.h>
#include <speutils/spu_message.h>
#include <speutils/messages.h>

#include <stdio.h>

#include "spu_taskmanager.h"
#include "spu_taskpipe.h"

#define STAGES 4

static int current_task=0;
static int current_state=WAIT;

static struct task_state_s tstate;
static unsigned int cycle=0;

//static struct task_pipe_s pipe;
//static uint32_t *tasks[4];

static struct pipe_entry_s pipe[STAGES];

static struct fifo_s *fifo;

void check_tasks()
{
    while (check_for_message())
    {
        tstate.front = spu_read_in_mbox();;
    }
}


void stage1()
{
    printf("spu: stage1\n");
    check_tasks();
    if (in_tasks_remaining(&tstate)){
//         printf("spu: stage1: fetching task\n");
        int context=cycle%STAGES;
//         printf("spu: stage1: context %d\n",context);
         printf("spu: task fifo EA 0x%08x%08x\n",(int)fifo->task_fifo>>32,(int)fifo->task_fifo);
         printf("spu: task LS 0x%08x\n",pipe[context].task);
        dmaGet(pipe[context].task,fifo->task_fifo+(tstate.in_back*TASK_SIZE),TASK_SIZE,pipe[context].id);

        pipe[context].active=1;
//         printf("spu: incrementing in back\n");
        tstate.in_back = increment(tstate.in_back, fifo->max_entries);
        pipe[context].entry=tstate.in_back;
    }
   /* if ( in_tasks_remaining(&tstate) )
    {
        printf("incrementing in_back\n");
        increment(&tstate.in_back, fifo->max_entries);
    }*/
    check_tasks();
}

void stage2()
{
    int i;
    printf("spu: stage2\n");
    int context=(cycle+1)%STAGES;

    if (pipe[context].active) {
//         printf("spu: stage2: context %d\n",context);
//         printf("spu: stage2 active\n");
        dmaWaitTag(pipe[context].id);
         printf("Stage2: Task\n");
        for(i = 0 ; i < TASK_SIZE;i++)
        {
            printf("%c",pipe[context].task[i]);
        }
        printf("\n");
    }
    check_tasks();
}


void stage3()
{
    printf("spu: stage3\n");

    int context=(cycle + 2)%STAGES;
    if (pipe[context].active){
  //      printf("spu: stage3: context %d\n",context);
    }
    check_tasks();
}

void stage4()
{
    printf("spu: stage4\n");
    int context=(cycle + 3)%STAGES;

    //TODO add if done.. do
    if (pipe[context].active) {
      //  printf("spu: stage4: context %d\n",context);
      //  printf("spu: stage4 incrementing out back\n");
        tstate.out_back = increment(tstate.out_back, fifo->max_entries);
        printf("spu: stage4 task %d complete\n",pipe[context].entry);
        pipe[context].active=0;
        printf("spu: sizeof struct ack_s %d\n",sizeof(struct ack_s));
        printf("spu: localstore 0x%08x\n",pipe[context].ack);
        printf("spu: ack fifo begins at EA 0x%08x%08x\n",(int)fifo->ack_fifo>>32,(int)fifo->ack_fifo);
        //printf("spu: EA is 0x%08x\n",fifo->ack_fifo+(pipe[context].entry*sizeof(struct ack_s)));
        dmaPut(pipe[context].ack,fifo->ack_fifo+(pipe[context].entry*sizeof(struct ack_s)),sizeof(struct ack_s),pipe[context].id);
        pipe[context].active=0;
    }
    check_tasks();
}

void pipeline()
{
    int i;
    do {
        /*
        for (i = 0 ; i < 4;i++)
        {
            printf("pipe %d, active %d\n",i,pipe[i].active);
        }*/
        stage1();
        stage2();
        stage3();
        stage4();
        cycle--;
       // printf("spu: front %d, in_back %d ,out_back %d\n",tstate.front,tstate.in_back,tstate.out_back);
    } while (tstate.front != tstate.out_back);
        printf("spu: END: front %d, in_back %d ,out_back %d\n",tstate.front,tstate.in_back,tstate.out_back);
}

int main(unsigned long long speid, unsigned long long argp, unsigned long long envp)
{
    int i;

    printf("spu: argp 0x%08x%08x\n",(uint32_t)(argp>>32),(uint32_t)argp);
    printf("spu: envp 0x%08x%08x\n",(uint32_t)(envp>>32),(uint32_t)envp);
    allocate_pipe_entries(&pipe,STAGES,TASK_SIZE);
    fifo = memalign(128, envp);
    printf("fifo_addr 0x%08x\n",fifo);

    dmaGetnWait(fifo, argp,(uint32_t)envp, 0);
    printf("spu: ack fifo begins at EA 0x%08x%08x\n",(int)fifo->ack_fifo>>32,(int)fifo->ack_fifo);
    printf("active %d\n",fifo->active);
    tstate.front = tstate.in_back= tstate.out_back = fifo->max_entries -1;
    current_state=WAIT;



    //allocate_pipe(&pipe,TASK_SIZE);

   // cycle=0;
    do {
        switch (current_state) {
            case WAIT:

                 while(!check_for_message());
                 pipeline();
//                 handle_message(wait_for_message_poll());
                break;
            case SNOOZE:
                send_message(0x05050505);
                spu_stop(READ_MBOX);
                break;
        }
    } while (current_task != 0xCAFEBABE);

    return 0;
}