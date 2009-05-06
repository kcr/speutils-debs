#ifndef __SPU_TASKPIPE_H
#define __SPU_TASKPIPE_H

#include <stdint.h>
#include <speutils/spefifo.h>
struct pipe_entry_s {
    uint8_t *task;
    uint32_t entry;
    uint32_t id;
    uint32_t stage;
    struct ack_s *ack;
    int active;
};

void allocate_pipe_entries(struct pipe_entry_s *entrylist, int entries,int tasksize);

#endif