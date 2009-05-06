/**
 * @file spu_message.h
 * @brief This header defines inline functions for sending spu->ppu and recieving from ppu->spu
 * @brief this file is a part of speutils
 * @author Kristian jerpetjøn
 *
 * Copyright (c) 2008, Kristian Jerpetjøn <kristian.jerpetjoen@gmail.com>
 *
 * All rights reserved.
 *
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
 *
 */

#ifndef __SPU_MESSAGE_H
#define __SPU_MESSAGE_H

#include <spu_mfcio.h>

#include <speutils/messages.h>

static inline void send_message(int *message) {
    while (!spu_stat_out_mbox());
    spu_writech(SPU_WrOutMbox,&message);
}

static inline void send_message_int(int *message){

	while (spu_stat_out_intr_mbox() == 0);
	spu_writech(SPU_WrOutIntrMbox, &message); //generates interrupt
}

static inline int wait_for_message_poll(){

	while (spu_stat_in_mbox() == 0);
	return spu_read_in_mbox();
}

static inline int check_for_message()
{
    return spu_stat_in_mbox();
}

#endif