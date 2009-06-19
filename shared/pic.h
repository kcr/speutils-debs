#ifndef __PIC_H
#define __PIC_H

#include <stdint.h>
typedef void (*pic_t)(void *arguments,void* buffers, void *buildtin, void* ack);

struct program_s {
    void *LS;
    int id;
    uint64_t EA;
    int size;
};

typedef struct program_s program_t;

#endif