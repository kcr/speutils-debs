/**
 * @file spu_dmalib.h
 * @brief This header defines functions for spu dma operations
 * @brief To use the inline functions you must define SPU_DMALIB_STATIC
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


#ifndef __SPU_DMALIB_H
#define __SPU_DMALIB_H

#include <spu_mfcio.h>

typedef unsigned int uint32_t;

/**
 * Function performs a dma get from spe then waits for the dma to complete
 * @param localstore local address (destination)
 * @param extern_adr external address (source)
 * @param size size of the dma job in bytes
 * @param tag dma tag
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetnWait(void *localstore, unsigned long long extern_adr, uint32_t size, int tag)
{
	int uiMask=1<<tag;
	mfc_get(localstore, extern_adr, size, tag, 0, 0);
	mfc_write_tag_mask(uiMask);
	mfc_read_tag_status_any();
}

/**
 * Function does dma out from spe then wait for the dma to complete
 * @param localstore local address (source)
 * @param extern_adr external address (destination)
 * @param size size of the dma job in bytes
 * @param tag dma tag
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutnWait(void *localstore, unsigned long long extern_adr, uint32_t size, int tag)
{
	int uiMask=1<<tag;
	mfc_put(localstore, extern_adr, size, tag, 0, 0);
	mfc_write_tag_mask(uiMask);
	mfc_read_tag_status_any();
}

/**
 * Function only usable for waiting for a single tag
 * @param tag dma tag
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaWaitTag(int tag)
{
	int uiMask=1<<tag;
	mfc_write_tag_mask(uiMask);
	mfc_read_tag_status_all();
}

/**
 * Function waits for all the given dma tags in uiMask to be done"
 * @param uiMask dma tag mask
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
int dmaWaitAll(unsigned int uiMask) {

	mfc_write_tag_mask(uiMask);
	mfc_read_tag_status_all();

}

/**
 * Function waits for any of the tags given is Mask to appear
 * @param uiMask dma tag mask
 * @return returns the tag status
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaWaitAny(unsigned int uiMask) {

	mfc_write_tag_mask(uiMask);
	return mfc_read_tag_status_any();
}

/**
 * Dma get
 * @param localstore local address (destination)
 * @param extern_adr external address (source)
 * @param size size of the dma job in bytes
 * @param tag the tag for the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGet(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_get(localstore, extern_adr,size,tag, 0, 0);
}

/**
 * Dma get with barrier
 * @param localstore local address (destination)
 * @param extern_adr external address (source)
 * @param size size of the dma job in bytes
 * @param tag the tag for the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetb(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_getb(localstore, extern_adr, size, tag, 0, 0);
}


/**
 * Dma get with fence
 * @param localstore local address (destination)
 * @param extern_adr external address (source)
 * @param size size of the dma job in bytes
 * @param tag the tag for the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetf(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_getf(localstore, extern_adr, size, tag, 0, 0);
}

/**
 * Dma list get
 * @brief Fetches a list of data from external address to localstore \n
 * @brief Each fetch in the list is defined by mfc_list_element_t \n
 * @brief All external addresses must be within the\n 32 upper address bits given by the ea
 * @param ls local address (destination)
 * @param ea external address only upper 32 bit counts(source ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetl (void *ls, unsigned long long ea, mfc_list_element_t *list, uint32_t list_size, int tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_getl(ls, ea, list, list_size, tag,0,0);
}

/**
 * Dma list get with barrier
 * @brief Fetches a list of data from external address to localstore \n
 * @brief Each fetch in the list is defined by mfc_list_element_t \n
 * @brief All external addresses must be within the \n 32 upper address bits given by the ea
 * @param ls local address (destination)
 * @param ea external address only upper 32 bit counts(source ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetlb (void *ls, unsigned long long ea, mfc_list_element_t *list, uint32_t list_size, int tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_getlb(ls, ea, list, list_size, tag,0,0);
}

/**
 * Dma list get with fence
 * @brief Fetches a list of data from external address to localstore
 * @brief Each fetch in the list is defined by mfc_list_element_t
 * @brief All external addresses must be within the \n 32 upper address bits given by the ea
 * @param ls local address (destination)
 * @param ea external address only upper 32 bit counts(source ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaGetlf (void *ls, unsigned long long ea, mfc_list_element_t *list, uint32_t list_size, int tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_getlf(ls, ea, list, list_size, tag,0,0);
}

/**
 * Atomically get cache line to localstore
 * @param localstore local address
 * @param extern_adr external address of the cache line
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void atomicGet(void *localstore, unsigned long long extern_adr) {

	/**start fetch*/
	mfc_getllar(localstore,extern_adr,0,0);
	/**wait for completion*/
	spu_readch(MFC_RdAtomicStat);
}

/**
 * Dma put
 * @param localstore local address (source)
 * @param extern_adr external address (destination)
 * @param size size of the dma job in bytes
 * @param tag the tag for the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPut(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_put(localstore, extern_adr, size, tag, 0, 0);
}

/**
 * Dma put with barrier
 * @param localstore local address (source)
 * @param extern_adr external address (destination)
 * @param size size of the dma job in bytes
 * @param tag the tag for of the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutb(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_putb(localstore, extern_adr, size, tag, 0, 0);
}

/**
 * Dma put with fence
 * @param localstore local address (source)
 * @param extern_adr external address (destination)
 * @param size size of the dma job in bytes
 * @param tag the tag for of the job 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutf(void *localstore, unsigned long long extern_adr, uint32_t size, int tag) {

	mfc_putf(localstore, extern_adr, size, tag, 0, 0);
}

/**
 * Dma list put
 * @brief Stores a list of data from localstore to external address \n
 * @brief Each store in the list is defined by mfc_list_element_t \n
 * @brief All external addresses must be within the \n 32 upper address bits given by the ea
 * @param ls local address (source start)
 * @param ea external address only upper 32 bit counts(destination ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutl (void *ls, uint64_t ea, mfc_list_element_t *list, uint32_t list_size, uint32_t tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_putl(ls,ea,list,list_size,tag,0,0);
}

/**
 * Dma list put with barrier
 * @brief Stores a list of data from localstore to external address \n
 * @brief Each store in the list is defined by mfc_list_element_t \n
 * @brief All external addresses must be within the \n 32 upper address bits given by the ea
 * @param ls local address (source start)
 * @param ea external address only upper 32 bit counts(destination ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutlb (void *ls, uint64_t ea, mfc_list_element_t *list, uint32_t list_size, uint32_t tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_putlb(ls,ea,list,list_size,tag,0,0);
}

/**
 * Dma list put with fence
 * @brief Stores a list of data from localstore to external address \n
 * @brief Each store in the list is defined by mfc_list_element_t \n
 * @brief All external addresses must be within the \n 32 upper address bits given by the ea
 * @param ls local address (source start)
 * @param ea external address only upper 32 bit counts(destination ea_high)
 * @param list the list to be transfered
 * @param list_size the size of the list in entries
 * @param tag the dma tag for the list 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void dmaPutlf (void *ls, uint64_t ea, mfc_list_element_t *list, uint32_t list_size, uint32_t tag) {

	list_size=list_size*sizeof(mfc_list_element_t);
	mfc_putlf(ls,ea,list,list_size,tag,0,0);
}


/**
 * Atomically write with conditional (puttlc)
 * @param localstore local address
 * @param extern_addr external adress of the cache line
 * @return returns 1 if successful
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
int atomicPutc(void *localstore, unsigned long long extern_adr) {

	/**perform store */
	mfc_putllc(localstore,extern_adr,0,0);
	/**wait for status and check if ok */
	return spu_readch(MFC_RdAtomicStat) & MFC_PUTLLC_STATUS;
}

/**
 * Atomically Puts lock line unconditional
 * @param localstore local address
 * @param extern_adr external adress of the cache line
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void atomicPutuc(void *localstore, unsigned long long extern_adr) {

	/**perform store */
	mfc_putlluc(localstore,extern_adr,0,0);
	/**wait for completion*/
	spu_readch(MFC_RdAtomicStat);
}

/**
 * Stores the Atomic cache line using the MFC queue (normal dma)
 * @param localstore local address (source)
 * @param extern_adr external address (destination)
 * @param tag dma tag 1<<(0-31)
 */
#ifdef SPU_DMALIB_STATIC
static inline
#endif
void atomicqPutuc(void *localstore, unsigned long long extern_adr, uint32_t tag ) {
	/**perform store */
	mfc_putqlluc(localstore,extern_adr,tag,0,0);
	/**wait for completion*/
}

#endif

