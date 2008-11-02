/**
 * @file messages.h
 * @brief This header includes classes and methods for starting and operating general purpose spu oriented pthreads \n
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

#ifndef __SPU_THREAD_H
#define __SPU_THREAD_H

#include <pthread.h>

#include <stdint.h>

#include <libspe2.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Various actions to issue to a looping thread */
enum actions {THREAD_EXIT, THREAD_CONTINUE};

/** Types of threads we can run */
enum thread_type{THREAD_LOOP,THREAD_RUN};

/** spu thread structure is a generalized structure \n
 * and should be extended to include any future extensions
 */
struct spu_thread_s {

	/** pthread context pointer*/
	pthread_t pts;

	/** FIXME thread id could be used for something useful */
	int thread_id;

	/** spe context pointer */
	struct spe_context * ctx;

	/** Typically this is SPE_EVENTS_ENABLE */
	unsigned int createflags;

	/** Typically this is 0 */
	unsigned int runflags;

	/** entry point on the spe.. updated on si_stop so that it can be re started where it "exits" */
	unsigned int entry;

	/**spe stop information */
	spe_stop_info_t stop_info;

	/**pointer to spe binary in .o file*/
	spe_program_handle_t handle;
	/**memory pointer to the first piece to dma into the task*/
	void * argp;
	/**size of the first task task*/
	void * envp;

	/** spe event unit */
	struct spe_event_unit event;

	/** spe event handler*/
	spe_event_handler_ptr_t spe_event;

	/** callback issued by returning thread */
	void (*callback)(int arg);

	/** condition variable used to sleep thread */
	pthread_cond_t condvar;

	/** mutex to go with the cond variable */
	pthread_mutex_t mutex;

	int epfd;

	struct epoll_event *events;

	int pipe[2];


	/** type of thread you are running see thread_type */
	int type;

	/** The action issued after a callback */
	int action;


};

typedef struct spu_thread_s spu_thread_t;

int start_spu_thread(spu_thread_t *spu);



int stop_spu_thread(spu_thread_t *spu);

void send_action(spu_thread_t *spu, int action);

// void send_action(spu_thread_t *spu);

// void *spu_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif
