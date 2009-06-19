
#define SPU_DMALIB_STATIC 1

#include <speutils/spefifo.h>
#include <speutils/spu_dmalib.h>
#include <speutils/spu_message.h>
#include <speutils/messages.h>

#include <stdio.h>

#include "spu_taskmanager.h"
#include "spu_taskpipe.h"

//WTF
#include "../../shared/pic.h"
#include "../../shared/linkedlist.h"

#define STAGES 4

static int current_task=0;
static int current_state=WAIT;

static struct task_state_s tstate;
static unsigned int cycle=0;

static struct pipe_entry_s pipe[STAGES];

static struct fifo_s *fifo;

static struct atomic_s *atomic;

static struct list_s *programs;


void check_tasks()
{
    atomicGet(atomic,fifo->atomic);

    if (atomic->state)
        tstate.front = atomic->count;
}

void stage1()
{
    check_tasks();
    if (in_tasks_remaining(&tstate)){
        int context=cycle%STAGES;

        dmaGet(pipe[context].task,fifo->task_fifo+(tstate.in_back*TASK_SIZE),TASK_SIZE,pipe[context].id);

        pipe[context].active=1;
        tstate.in_back = increment(tstate.in_back, fifo->max_entries);
        pipe[context].entry=tstate.in_back;
    }
}

void stage2()
{
    int i;
    int context=(cycle+1)%STAGES;
    struct program_s *prog;
    //fetch program and data ????
    //TODO if so define how data is specified in the command structure
    if (pipe[context].active) {
        dmaWaitTag(pipe[context].id);

        int *task =pipe[context].task;
        switch (task[0])
        {
            case REGISTER_PROGRAM:
                printf("registering program...\n");
                struct program_s *program = malloc(sizeof(program));
                program->size = ((task[1]+15)&(~0xF)); /**round off in case fuckup*/
                program->id = task[3];
                program->LS = NULL;
                program->EA = (task[2]<<32) | task[3];
                break;

            case ADD:
                break;

            default:


                prog = listFirstEntry(programs);

                while (prog->id != task[0])
                {
                    prog = listNextEntry(program);
                    if (prog == NULL)
                        break;
                }

                if (prog==NULL){
                    printf("missing program 0x%08x\n",task[0]);
                    break;
                }

                if (prog->LS == NULL)
                {
                    //! attempt to allocate local store
                    prog->LS == memalign(128,prog->size);

                    //! get if success
                    if (prog->LS != NULL)
                        dmaGet(prog->LS,prog->EA,prog->size,pipe[context].id);
                }
                break;
        }

//         int *task = pipe[con.text].task;
//         printf("stage2 task command 0x%08x\n",task[0]);
//         printf("stage2 task handle 0x%08x\n",task[1]);
        //here we will fetch program and data
    }
}


void stage3()
{
    int context=(cycle + 2)%STAGES;
    program_t *prog;
    int res;
    volatile int i;
    if (pipe[context].active){
        //here we will execute the program with the data
        //if(TYPE==ADD)

        //TODO HACK REMOVE THIS
        int *task = pipe[context].task;
        switch(task[0])
        {

            case ADD:
                //int adds = task[2];
                //int arg1 = task[3];
                //int arg2 = task[4];

                for (i = 0 ; i < task[2] ; i++)
                {
                    res +=task[3]+i + task[4];
                }

                pipe[context].ack->retval=res;
                break;
        //TODO I SAID REMOVE THIS HACK
            case NOOP:
                break;

            default:

                prog = listFirstEntry(programs);

                while (prog->id != task[0])
                {
                    prog = listNextEntry(programs);
                    if (prog == NULL)
                        break;
                }

                if (prog == NULL){
                    printf("missing program 0x%08x\n",task[0]);
                    break;
                }

                if (prog->LS == NULL)
                {
                    prog->LS == memalign(128,prog->size);

                    if (prog->LS != NULL)
                        dmaGetnWait(prog->LS,prog->EA,prog->size,pipe[context].id);
                    else
                        printf("BAAAAD robot\n");
                }

                //todo fix this
                pic_t pic = (pic_t*)prog->LS;
                pic(pipe[context].task,NULL,NULL,pipe[context].ack);

                break;


        }
    }
}

void stage4()
{
    int context=(cycle + 3)%STAGES;

    //TODO add if done.. do
    if (pipe[context].active) {
        tstate.out_back = increment(tstate.out_back, fifo->max_entries);
        pipe[context].active=0;
        //this will probably end up in stage 3 and here we will check its completion
        dmaPut(pipe[context].ack,fifo->ack_fifo+(pipe[context].entry*sizeof(struct ack_s)),sizeof(struct ack_s),pipe[context].id);
        pipe[context].active=0;
    }
}

void pipeline()
{
    int i;
    do {
        stage1();
        stage2();
        stage3();
        stage4();
        cycle--;
    } while (tstate.front != tstate.out_back);
}

int main(unsigned long long speid, unsigned long long argp, unsigned long long envp)
{
    int i;

    printf("spu: argp 0x%08x%08x\n",(uint32_t)(argp>>32),(uint32_t)argp);
    printf("spu: envp 0x%08x%08x\n",(uint32_t)(envp>>32),(uint32_t)envp);
    allocate_pipe_entries(&pipe,STAGES,TASK_SIZE);
    atomic = memalign(128,sizeof(struct atomic_s));
    fifo = memalign(128, envp);
    printf("fifo_addr 0x%08x\n",fifo);

    dmaGetnWait(fifo, argp,(uint32_t)envp, 0);
    printf("spu: ack fifo begins at EA 0x%08x%08x\n",(int)fifo->ack_fifo>>32,(int)fifo->ack_fifo);
    printf("active %d\n",fifo->active);
    tstate.front = tstate.in_back= tstate.out_back = fifo->max_entries -1;
    current_state=WAIT;


    programs == listInit();

    do {
        switch (current_state) {
            case WAIT:
                 pipeline();
                break;
            case SNOOZE:
                send_message(0x05050505);
                spu_stop(READ_MBOX);
                break;
        }
    } while (current_task != 0xCAFEBABE);

    return 0;
}