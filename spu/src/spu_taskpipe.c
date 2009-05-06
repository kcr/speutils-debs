#include "spu_taskpipe.h"


void allocate_pipe_entries(struct pipe_entry_s *entrylist, int entries, int tasksize)
{
    int i;
    uint8_t *addr=memalign(128,entries*tasksize);
    printf("spu: addr 0x%08x\n",addr);
    for (i = 0 ; i < entries;i++)
    {
        entrylist[i].stage=-1;
        entrylist[i].id=i;
        entrylist[i].task=&addr[i*tasksize];
        entrylist[i].active=0;
        entrylist[i].ack=memalign(128,sizeof(struct ack_s));
        printf("spu: id %d, addr 0x%08x\n",i,entrylist[i].task);
    }
}

//int