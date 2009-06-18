//implementation
#include <speutils/spefifo.h>
//external
#include <malloc.h>
#include <stdint.h>
//internal
#include <speutils/spethread.h>
#include <speutils/speutils.h>
#include <speutils/messages.h>
#include <ppu_intrinsics.h>

// #include <speutils/spemessage.h>

#include <../shared/ring.h>

#define ACK_CLEAR 0xc1ee7000
//anyone know of anything with more spes ?
#define MAX_SPES 16

#define ACK_SIZE 16

extern spe_program_handle_t spu_taskmanager_handle;

struct host_fifo_s {
    int spes;
    int scheme;
    ring_context_t *ringCTX[MAX_SPES];
    struct fifo_s *fifos[MAX_SPES];
    uint32_t *entry_p[MAX_SPES];
    uint32_t entry_count[MAX_SPES];

   // struct fifo_entries_s entry[MAX_SPES];
//     struct ack_entries_s ack[MAX_SPES];
    spu_thread_t spu_thread[MAX_SPES];
    pthread_t pts;
    pthread_mutex_t mutex[MAX_SPES];
    int isKicked[MAX_SPES];

};

static struct host_fifo_s hostfifo;

static int current_spe;

void send_front(int spe, int front)
{
     int message = ringGetFront(hostfifo.ringCTX[spe]);
//     spe_in_mbox_write(hostfifo.spu_thread[spe].ctx,&message,1,SPE_MBOX_ANY_NONBLOCKING);

    int result;
    struct atomic_s *atomic;
    do {
        uint32_t count = __lwarx(hostfifo.fifos[spe]->atomic);
        count = message;

        atomic=hostfifo.fifos[spe]->atomic;
        if (atomic->state != 1)
            atomic->state =1;
        result= __stwcx(hostfifo.fifos[spe]->atomic,count);

    } while(!result);
}

struct fifo_s *allocate_fifo(int entries,int entry_size)
{
    int i;
//     int j;
    struct fifo_s *fifo = memalign(128, sizeof(struct fifo_s));
    fifo->task_fifo =(uint64_t)(unsigned long) memalign(128, entries*entry_size);
   // printf("task_fifo is at 0x%08x%08x\n",(int)(fifo->task_fifo>>32),(int)fifo->task_fifo);
    fifo->ack_fifo = (uint64_t)(unsigned long) memalign(128, entries*ACK_SIZE);

    //printf("size of struct atomic_s %d\n",sizeof(struct atomic_s));
    fifo->atomic = (uint64_t)(unsigned long)memalign(128,sizeof(struct atomic_s));

    struct atomic_s *atomic = fifo->atomic;
    atomic->count=0;
    atomic->state=0;
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
//             printf("ppu: round_robin\n");
        default:
            current_spe++;
            if ( current_spe >= hostfifo.spes )
                current_spe=0;
            // We are not stalling here it seems
            while(!ringHasFree(hostfifo.ringCTX[current_spe])) {
             //   usleep(100);
                current_spe++;
                if ( current_spe >= hostfifo.spes )
                    current_spe=0;
                //repeat untill there is a free slot
            }

            retval=current_spe;
            break;
    }
    return retval;
}

void *fifothread(void *argp)
{
    int spe = (int) argp;
    printf("spe %d\n",spe);
    int i=spe;
    int32_t *ack;
    while(1){
       // shed_yield();
        usleep(25);
        //usleep(50);
        for (i=0; i< hostfifo.spes;i++)
        {

                ack = (uint32_t *)((char *)(hostfifo.fifos[i]->ack_fifo +ringGetBack(hostfifo.ringCTX[i])*ACK_SIZE));

            while (ack[0] != ACK_CLEAR)
            {
                ack[0]=ACK_CLEAR;
//                 printf("retval %d\n", ack[1]);
               // (uint32_t *)((char *)(ringGetBack(hostfifo.ringCTX[i])*ACK_SIZE))[0] = ACK_CLEAR;
                ringIncBack(hostfifo.ringCTX[i]);

                ack = (uint32_t *)((char *)(hostfifo.fifos[i]->ack_fifo +ringGetBack(hostfifo.ringCTX[i])*ACK_SIZE));

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
        //asume we start at a clean point
        hostfifo.isKicked[i]=1;

        hostfifo.fifos[i] = allocate_fifo(fifo_size/TASK_SIZE, TASK_SIZE);

        hostfifo.fifos[i]->max_entries = fifo_size/TASK_SIZE;
        hostfifo.fifos[i]->active = 1;
        hostfifo.fifos[i]->id = i;

        hostfifo.ringCTX[i]=ringInitContext(fifo_size/TASK_SIZE);

//         hostfifo.ack[i].ack_pos=0;

        //start the spu
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
    int retval=0;
   // for (i = 0 ; i < spes; i++) {

        retval += pthread_create(&hostfifo.pts,0,fifothread,0);
    //}
    if (retval < 0 )
        printf("ppu: failed to start pthread\n");

}

int fifoBegin(int command, int handle)
{
    int spe = selectfifo();

 //   pthread_mutex_lock(&hostfifo.mutex[spe]);
    //if not kicked increment
    if (!hostfifo.isKicked[spe])
        ringIncFront(hostfifo.ringCTX[spe]);
    //ring front is not kicked
    hostfifo.isKicked[spe]=0;
    hostfifo.entry_count[spe]=0;
    hostfifo.entry_p[spe] =(uint32_t *) ((char *)(hostfifo.fifos[spe]->task_fifo) + ringGetFront(hostfifo.ringCTX[spe])*(TASK_SIZE));
 //   hostfifo.count[i]=0;
  /*  if (hostfifo.entry[spe].task_entry >= hostfifo.fifos[spe]->max_entries)
        hostfifo.entry[spe].task_entry=0;*/
        //TODO rewind
 //   printf("ppu fifobegin at entry point %d\n",hostfifo.entry[spe].entry_point);

 //   hostfifo.entry[spe].entry_point =((TASK_SIZE/4)-1);
//     printf("ppu fifobegin at entry point rounded up %d\n",hostfifo.entry[spe].entry_point);
//    hostfifo.entry[spe].entry_point = (hostfifo.entry[spe].entry_point &(~0x1F));

//     printf("ppu fifobegin at entry point %x\n",&hostfifo.entry_p[spe][0]);
    //TODO fix le bugg
    hostfifo.entry_p[spe][0]=command;
    hostfifo.entry_count[spe]++;
    hostfifo.entry_p[spe][1]=handle;
    hostfifo.entry_count[spe]++;
  //  hostfifo.entry[spe].task_fifo[hostfifo.entry[spe].entry_point] = command;

  //  hostfifo.entry[spe].task_fifo[hostfifo.entry[spe].entry_point+1] = handle;
  //  hostfifo.entry[spe].entry_point+=2;
    return spe;
//     hostfifo.fifos[fifo]->fifo_pos+=2;
}

void fifoAdd(int spe, int arg)
{
    if (hostfifo.entry_count[spe]+1 != TASK_SIZE/4) {
        hostfifo.entry_p[spe][hostfifo.entry_count[spe]]=arg;
        hostfifo.entry_count[spe]++;
    }
}

void fifoKick(int spe)
{

    //send_message(&hostfifo.spu_thread[spe],ringGetFront(hostfifo.ringCTX[spe]));
    //spu->ctx,&message,1,SPE_MBOX_ALL_BLOCKING);
    //int message = ringGetFront(hostfifo.ringCTX[spe]);
    send_front(spe,ringGetFront(hostfifo.ringCTX[spe]));


    hostfifo.isKicked[spe]=1;
    ringIncFront(hostfifo.ringCTX[spe]);
//    pthread_mutex_unlock(&hostfifo.mutex[spe]);
}



void fifoNoop()
{
    int spe = fifoBegin(NOOP,0x1ee7C0DE);
    fifoKick(spe);
  // int spe=selectfifo();


//    printf("ppu: sending noop to spe %d\n",spe);

 //  pthread_mutex_unlock(&hostfifo.mutex[spe]);

}

void fifo_addition(int adds)
{
    int spe = fifoBegin(ADD,0x1ee7C0DE);
    fifoAdd(spe, adds);
    fifoAdd(spe,0x01);
    fifoAdd(spe,0x02);
    fifoKick(spe);
}

void fifoStop()
{


}

int fifoIsEmpty()
{
    int i;
    int res=1;
    for (i = 0 ; i < hostfifo.spes; i++)
    {
        if (ringHasRemaining(hostfifo.ringCTX[i])) {
            return 0;

         //   printf("fifo %d has remaining\n",i);
        }
    }
    return 1;
}

void fifoWait()
{
    int i;
    int res;
    do {
        res=0;
        for (i = 0 ; i < hostfifo.spes; i++)
        {
            if (ringHasRemaining(hostfifo.ringCTX[i])) {
                res+=1;
                printf("fifo %d has remaining\n",i);
            }
        }
    } while(res != 0);
}
/*
void fifoFree(fifo_t* fifo) {
    free(fifo->task_fifo);
    free(fifo->ack_fifo);
    free(fifo);
}
*/

