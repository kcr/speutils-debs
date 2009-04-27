/**
 * @file messages.h
 * @brief This source file includes methods for starting and operating general purpose spu oriented pthreads \n
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

#include <speutils/spethread.h>
#include <speutils/messages.h>
#include <malloc.h>
#include <string.h>
#include <sys/epoll.h>

#define MAX_EVENTS 2

spu_thread_t *spu_thread_allocate()
{
    return memalign(128,sizeof(struct spu_thread_s));
}

void spu_thread_free(spu_thread_t *thread)
{
    free(thread);
}

/**
 * Function defines a general purpose spu thread
 * @param arg arguments from the thread in spu_thread_s
 * @return returns pthread_exit(NULL)
 */
void *spu_thread(void *arg){

	spu_thread_t *spu;

	spu=(spu_thread_t *)arg;

 	if (spe_program_load(spu->ctx,&spu->handle) < 0)
	{
		perror("error loading spu-elf program\n");
		pthread_exit(NULL);
	}

	if (spe_context_run(spu->ctx, &spu->entry, spu->runflags, spu->argp, spu->envp,0 ) < 0) {
		 perror("spe_context_run\n");
		pthread_exit(NULL);
	}
	printf("spe exit successfull\n");
	pthread_exit(NULL);
}

/**
 * Function defines a general purpose looping spu thread
 * which issues a callback with arguments from spe and waits
 * for a signal before continue or exit
 * @param arg arguments from the thread in spu_thread_s
 * @return returns pthread_exit(NULL)
 */
void *spu_loop_thread(void *arg) {

	spu_thread_t *spu;

	spu=(spu_thread_t *)arg;

	spu->epfd = epoll_create(MAX_EVENTS);

	spu->events = malloc(sizeof(struct epoll_event)*MAX_EVENTS);



	struct epoll_event ev;

	ev.data.fd=spu->pipe[0];

	ev.events=EPOLLIN|EPOLLERR;

	if (epoll_ctl(spu->epfd, EPOLL_CTL_ADD, spu->pipe[0], &ev) < 0) {
		fprintf(stderr, "epoll set insertion error: fd=%d\n",
			spu->pipe[0]);
		pthread_exit(NULL);
	}


	if (spe_program_load(spu->ctx,&spu->handle) < 0)
	{
		perror("error loading spu-elf program\n");
		pthread_exit(NULL);
	}
	do {
		if (spe_context_run(spu->ctx, &spu->entry, spu->runflags, spu->argp, spu->envp,&spu->stop_info ) < 0) {
			perror("spe_context_run\n");
			pthread_exit(NULL);
		}

		if (spu->stop_info.stop_reason != SPE_STOP_AND_SIGNAL ) {
			break;
		}

        if (spu->stop_info.result.spe_signal_code&READ_MBOX)
        {
          int message;
          spe_out_mbox_read(spu->ctx,&message,SPE_MBOX_ANY_BLOCKING);
          spu->callback(message);
        } else {
		  spu->callback(spu->stop_info.result.spe_signal_code);
        }
		// Look at how much better it looks and feels using epoll.. ps this can still stall
		int res ;

		int i;

		int waiting =1;

		while (waiting)
		{
			res = epoll_wait( spu->epfd, spu->events, MAX_EVENTS, -1);

			for (i = 0 ; i < res ; i++ ){

				if ( spu->events[i].data.fd == spu->pipe[0] ) {

					read( spu->pipe[0] , &spu->action, sizeof(int) );
					waiting=0;
					break;

				}
			}
		}

		if (spu->action ==  THREAD_EXIT ) {
			break;
		}



	} while(1);

	printf("spe exit successfull\n");
	pthread_exit(NULL);

}

/**
 * Function for starting a spu_thread and initiate events
 * @param spu spu_thread_s structure to the thread
 * @return returns 0 on success
 */
int start_spu_thread(spu_thread_t *spu) {

	spu->runflags = 0;

	spu->entry = SPE_DEFAULT_ENTRY;

	spu->createflags = SPE_EVENTS_ENABLE;
	spu->ctx=spe_context_create(spu->createflags, NULL);
	spu->spe_event = spe_event_handler_create();
	spu->event.spe = spu->ctx;
	spu->event.events = SPE_EVENT_OUT_INTR_MBOX | SPE_EVENT_SPE_STOPPED;
	spe_event_handler_register(spu->spe_event, &spu->event);

	switch (spu->type) {

		case THREAD_LOOP:
			pipe(spu->pipe); //event notification pipe :)
			spu->thread_id=pthread_create(&spu->pts,NULL,spu_loop_thread,spu);
			break;

		case THREAD_RUN:
		default:
			spu->thread_id=pthread_create(&spu->pts,NULL,spu_thread,spu);
			break;
	}
	if ( spu->thread_id != 0) {
		perror("Failed to start spe thread\n");
		exit(spu->thread_id);
	}

 	return 0;
}

/**
 * Waits for the thread to exit must not be issued without giving a command to the spe or
 * spe thread to actually exit
 * @param spu pointer to the spu_thread_s
 * @return returns 0 on success
 */
int stop_spu_thread(spu_thread_t *spu) {

	pthread_join(spu->pts,NULL);

	spe_event_handler_deregister( spu->spe_event, &spu->event);

	spe_event_handler_destroy(spu->spe_event);

	spe_context_destroy(spu->ctx);

	free(spu->events);

	close(spu->pipe);

	return 0;
}

void send_action(spu_thread_t *spu, int action) {

	write(spu->pipe[1],&action, sizeof(int));

}
