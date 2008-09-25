/**
 * @file spemessages.c
 * @brief This source is functions for sending ppu->spu and recieving from spu->ppu speutils \n
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

#include "spemessage.h"

/**
 * Sends the message to the spu_thread specified
 * Funtion waits untill there is room to write the message before returning
 * @param spu pointer to spu_thread_s
 * @param message the message to be sendt to the spu
 */
void send_message(spu_thread_t *spu, int message) {
	//TODO make this return something sane
	while (spe_in_mbox_status(spu->ctx) == 0); // switch this to a nice little interupt based one
	//FIXME SPE_MBOX_ALL_NONBLOCKING perhaps
	spe_in_mbox_write(spu->ctx,&message,1,SPE_MBOX_ALL_BLOCKING);
}

/**
 * Function goes into a epoll wait state while waiting form interrupt from the spu
 * @param spu pointer to spu_thread_s
 * @return returns the recieved message from the spu STOP if still failed after 10 events
 */
int recieve_message_int(spu_thread_t *spu) {

	unsigned int message;

	int retries = 10;

	while(retries) {
		if(spe_event_wait(spu->spe_event, &spu->event, 1, -1) <= 0 || !(spu->event.events & SPE_EVENT_OUT_INTR_MBOX)) {
			retries--;
		} else {
			break;
		}
	}

	if(retries == 0) {
		perror("Failed to recive result from spe");
		return STOP;
	}

	spe_out_intr_mbox_read(spu->ctx,&message,1,SPE_MBOX_ANY_NONBLOCKING);

	return message;

}
