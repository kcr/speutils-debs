#ifndef __RING_H
#define __RING_H

#include <stdint.h>

struct ring_context_s
{
    uint32_t front;
    uint32_t back;
    uint32_t size;
};

typedef struct ring_context_s ring_context_t;

ring_context_t *ringInitContext(int size);
void ringFreeContext(ring_context_t*);
int ringHasFree(ring_context_t*);
int ringHasRemaining(ring_context_t*);
void ringIncFront(ring_context_t*);
void ringIncBack(ring_context_t*);
uint32_t ringGetFront();
uint32_t ringGetBack();
uint32_t ringReset();

#endif