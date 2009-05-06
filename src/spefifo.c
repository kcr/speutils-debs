//implementation
#include <speutils/spefifo.h>
//external
#include <malloc.h>
#include <stdint.h>
//internal
#include <speutils/spethread.h>
#include <speutils/speutils.h>
#include <speutils/messages.h>

#define ACK_CLEAR 0xc1ee7000
//anyone know of anything with more spes ?
#define MAX_SPES 16

#define ACK_SIZE 16

extern spe_program_handle_t spu_taskmanager_handle;


struct fifo_entries_s {
    uint32_t *task_fifo;
    uint32_t entry_point;
    uint32_t task_entry;
};
// struct fifo_entry_s {
//     uint32_t position;
// };

struct host_fifo_s {
    int spes;
    int scheme;
    struct fifo_s *fifos[MAX_SPES];
    struct fifo_entries_s entry[MAX_SPES];
    spu_thread_t spu_thread[MAX_SPES];
    pthread_t pts;
    pthread_mutex_t mutex[MAX_SPES];

};

static struct host_fifo_s hostfifo;

static int current_spe;

#ifndef __SPU__



struct fifo_s *allocate_fifo(int entries,int entry_size)
{
    int i;
//     int j;
    struct fifo_s *fifo = memalign(128, sizeof(struct fifo_s));
    fifo->task_fifo =(uint64_t)(unsigned long) memalign(128, entries*entry_size);
   // printf("task_fifo is at 0x%08x%08x\n",(int)(fifo->task_fifo>>32),(int)fifo->task_fifo);
    fifo->ack_fifo = (uint64_t)(unsigned long) memalign(128, entries*ACK_SIZE);
   // printf("ack_fifo is at 0x%08x%08x\n",(int)(fifo->ack_fifo>>32),(int)fifo->ack_fifo);

    //empty ack fifo
    uint32_t *ack=fifo->ack_fifo;
//     uint8_t *ack_byte=fifo->ack_fifo;
    for(i = 0; i < entries;i++)
    {
       ack[(i*ACK_SIZE)/sizeof(uint32_t)]=ACK_CLEAR;
//        printf("ack_ring\n");
//        for(j=0;j<ACK_SIZE;j++)
//        {
//             printf("%2x",ack_byte[i*ACK_SIZE + j]);
//        }
//        printf("\n");
    }
     //  printf("ack_fifo is at 0x%08x%08x\n",(int)(fifo->ack_fifo>>32),(int)fifo->ack_fifo);

    return fifo;
}


int selectfifo()
{
    int retval = 0;
    switch(hostfifo.scheme)
    {
        case ROUND_ROBIN:
            printf("ppu: round_robin\n");
        default:
            current_spe++;
            if ( current_spe >= hostfifo.spes )
                current_spe=0;
            retval=current_spe;
            break;
    }
    return retval;
}

void *fifothread(void *argp)
{
    int i;
    while(1){
        usleep(1000);
        for (i=0; i< hostfifo.spes;i++)
        {
            uint32_t *ack=hostfifo.fifos[i]->ack_fifo;
        //    printf("checking at pos 0x%08x\n",&ack[hostfifo.fifos[i]->ack_pos*(ACK_SIZE/sizeof(uint32_t))]);
            while (ack[hostfifo.fifos[i]->ack_pos*(ACK_SIZE/sizeof(uint32_t))] != ACK_CLEAR)
            {
                printf("ACK returned to fifo %d at pos %d\n",i,hostfifo.fifos[i]->ack_pos);
                hostfifo.fifos[i]->ack_pos++;
            }

        }
    }

}

void fifo_callback(int value)
{
    printf("callback %d\n",value);
}
/**
 * Initiates the spefifo interface
 * @param fifo_size in bytes will be rounded up
 * @param spes number of spes
 */
void fifoInit(int fifo_size, int spes, int scheme) {
    int i;

    hostfifo.spes=spes;
    hostfifo.scheme=scheme;
    current_spe=0;
  //  hostfifo.fifos=malloc(sizeof(void)*spes);


    for (i = 0; i < spes; i++) {

        hostfifo.fifos[i] = allocate_fifo(fifo_size/TASK_SIZE, TASK_SIZE);
        hostfifo.fifos[i]->fifo_pos = 0;
        hostfifo.fifos[i]->ack_pos = 0;
        hostfifo.fifos[i]->max_entries = fifo_size/TASK_SIZE;
        hostfifo.fifos[i]->active = 1;
        hostfifo.fifos[i]->id = i;
        hostfifo.entry[i].task_fifo = hostfifo.fifos[i]->task_fifo;
        hostfifo.entry[i].entry_point=0;
        hostfifo.entry[i].task_entry=0;
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
    //TODO start monitoring thread..
    int retval = pthread_create(&hostfifo.pts,0,fifothread,0);
    if (retval < 0 )
        printf("ppu: failed to start pthread\n");

}

int fifoBegin(int command, int handle)
{
    int spe=selectfifo();

    pthread_mutex_lock(&hostfifo.mutex[spe]);

    hostfifo.entry[spe].entry_point+=((TASK_SIZE/32)-1);

    hostfifo.entry[spe].entry_point&=~((TASK_SIZE/32))
            ;
    hostfifo.entry[spe].task_fifo[hostfifo.entry[spe].entry_point] = command;
    hostfifo.entry[spe].task_fifo[hostfifo.entry[spe].entry_point+1] = handle;
    hostfifo.entry[spe].entry_point+=2;
    return spe;
//     hostfifo.fifos[fifo]->fifo_pos+=2;
}

void fifoAdd(int spe, int arg)
{
    //if (hostfifo.entry[spe].entry_point == (TASK_SIZE/32))

    hostfifo.entry[spe].task_fifo[hostfifo.entry[spe].entry_point] = arg;
    hostfifo.entry[spe].entry_point++;
}

void fifoKick(int spe)
{
    printf("ppu: fifoKick: spe %d, entry %d, entry_point %d\n",spe,hostfifo.entry[spe].task_entry,hostfifo.entry[spe].entry_point);
    printf("ppu: divider %d\n",TASK_SIZE/4);
    hostfifo.entry[spe].task_entry = (hostfifo.entry[spe].entry_point / (TASK_SIZE/4));
    printf("ppu: spe %d, entry %d\n",spe,hostfifo.entry[spe].task_entry);
    //TODO add check if entry +1 > back..
    send_message(&hostfifo.spu_thread[spe],hostfifo.entry[spe].task_entry);

    hostfifo.entry[spe].entry_point+=((TASK_SIZE/32)-1);
    hostfifo.entry[spe].entry_point&=~((TASK_SIZE/32));

    pthread_mutex_unlock(&hostfifo.mutex[spe]);
}



void fifoNoop()
{
    int spe = fifoBegin(NOOP,0x1ee7C0DE);
    fifoKick(spe);
  // int spe=selectfifo();


   printf("ppu: sending noop to spe %d\n",spe);

 //  pthread_mutex_unlock(&hostfifo.mutex[spe]);

}

void fifoStop()
{


}
/*
void fifoFree(fifo_t* fifo) {
    free(fifo->task_fifo);
    free(fifo->ack_fifo);
    free(fifo);
}
*/
#endif

