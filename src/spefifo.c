//implementation
#include <speutils/spefifo.h>
//external
#include <malloc.h>
#include <stdint.h>
//internal
#include <speutils/spethread.h>
#include <speutils/speutils.h>


//anyone know of anything with more spes ?
#define MAX_SPES 16

#define ACK_SIZE 16

extern spe_program_handle_t spu_taskmanager_handle;

struct host_fifo_s {
    int spes;
    struct fifo_s *fifos[MAX_SPES];
    spu_thread_t spu_thread[MAX_SPES];

};

static struct host_fifo_s hostfifo;

#ifndef __SPU__



struct fifo_s *allocate_fifo(int entries,int entry_size)
{
    struct fifo_s *fifo = memalign(128, sizeof(struct fifo_s));
    fifo->task_fifo =(uint64_t)(unsigned long) memalign(128, entries*entry_size);
    fifo->ack_fifo = (uint64_t)(unsigned long) memalign(128, entries*ACK_SIZE);
    return fifo;
}


void *fifothread(void *argp)
{

}

void fifo_callback(int value)
{
    printf("callback %d\n",value);
}

void init_fifo(int entries, int entry_size, int spes) {
    int i;

    hostfifo.spes=spes;
  //  hostfifo.fifos=malloc(sizeof(void)*spes);


    for (i = 0; i < spes; i++) {
        hostfifo.fifos[i] = allocate_fifo(entries, entry_size);
        hostfifo.fifos[i]->fifo_pos = 0;
        hostfifo.fifos[i]->ack_pos = 0;
        hostfifo.fifos[i]->max_entries = entries;
        hostfifo.fifos[i]->active = 1;
        hostfifo.fifos[i]->id = i;
        hostfifo.spu_thread[i].type = THREAD_LOOP;
        hostfifo.spu_thread[i].argp = hostfifo.fifos[i];
        hostfifo.spu_thread[i].envp = sizeof(struct fifo_s);
        hostfifo.spu_thread[i].callback=fifo_callback;
        printf("ppu: argp 0x%08x \n",hostfifo.spu_thread[i].argp);
        printf("ppu: envp 0x%08x \n",hostfifo.spu_thread[i].envp);
        hostfifo.spu_thread[i].handle = spu_taskmanager_handle;
        if (start_spu_thread(&hostfifo.spu_thread[i]) < 0 ) {
            hostfifo.fifos[i]->active = 0;
        }
    }

}

void free_fifo(fifo_t* fifo) {
    free(fifo->task_fifo);
    free(fifo->ack_fifo);
    free(fifo);
}

#endif

