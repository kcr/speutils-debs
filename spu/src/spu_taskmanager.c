
#define SPU_DMALIB_STATIC 1

#include <speutils/spefifo.h>
#include <speutils/spu_dmalib.h>
#include <speutils/spu_message.h>
#include <speutils/messages.h>

#include <stdio.h>

#include "spu_taskmanager.h"

static int current_task=0;
static int current_state=WAIT;

static struct task_state_s tstate;

static struct fifo_s *fifo;



//static struct task_buffer_s tasks;

// static int task_front;
// static int task_back;

void new_task(int task_id)
{
    task_front = increment(task_front,fifo->max_entries);
    //printf("task\n");
}

void handle_message(int message)
{
        printf("message 0x%x\n",message);
        switch(message)
        {
            case SLEEP:
                spu_stop(0x0000);
                break;
            case STOP:
                break;
            case INVALID:
                break;
            default:
                new_task(message);
                break;

        }
    //} while(message != STOP);
}

int main(unsigned long long speid, unsigned long long argp, unsigned long long envp)
{
    int i;

    printf("spu: argp 0x%08x%08x\n",(uint32_t)(argp>>32),(uint32_t)argp);
    printf("spu: envp 0x%08x%08x\n",(uint32_t)(envp>>32),(uint32_t)envp);

    fifo = memalign(128, envp);
    printf("fifo_addr 0x%08x\n",fifo);
  //  dmaGet(fifo,argp,(uint32_t)envp,4);
  //  dmaWaitTag(4);

    dmaGetnWait(fifo, argp,(uint32_t)envp, 0);

    printf("active %d\n",fifo->active);
    tstate.task_front=tstate.task_back = fifo->max_entries -1;
    current_state=WAIT;


    do {
        switch (current_state) {
            case WAIT:
                while(!message_check());
                handle_message(wait_for_message_poll());
                break;
            case SNOOZE:
                send_message(0x05050505);
                spu_stop(READ_MBOX);
                break;
        }
    } while (current_task != 0xCAFEBABE);

    return 0;
}