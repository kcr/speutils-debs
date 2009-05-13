/**
 * @file spefifo.h
 * @brief This header defines structures and functions related to the speutils \
 * @brief fifo interface
 * @brief this file is a part of speutils
 * @author Kristian jerpetjøn
 *
 * Copyright (c) 2008, Kristian Jerpetjøn <kristian.jerpetjoen@gmail.com>
 *
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __SPEFIFO_H
#define __SPEFIFO_H

#include <stdint.h>

#define TASK_SIZE 128

#define ROUND_ROBIN     0x0

struct fifo_s {
    uint64_t task_fifo;
    uint64_t ack_fifo;
    uint64_t atomic;
    uint32_t max_entries;
    uint32_t active;
    uint16_t id;
}__attribute((aligned(16)));


struct atomic_s {
    uint32_t count;
    uint32_t state;
    char unused[120];
}__attribute((aligned(128)));

struct ack_s {
    uint32_t task_id;
    uint32_t retval;
    uint64_t callback;

}__attribute((aligned(16)));

typedef struct fifo_s fifo_t;

#ifndef __SPU__
void fifoInit(int fifo_size, int spes,int scheme);

int fifoBegin(int command, int handle);

void fifoAdd(int spe, int arg);

void fifoKick(int spe);

void fifoNoop();

int fifoIsEmpty();

void fifoWait();

void fifoStop();


#endif

#endif