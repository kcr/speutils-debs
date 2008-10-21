#include <stdio.h>

#include <speutils/spu_message.h>
#include <speutils/spu_dmalib.h>


#include "example2.h"

int main (unsigned long long speid, unsigned long long argp, unsigned long long envp) {

	struct example2_s *ex2;

	ex2=(struct example2_s *)memalign(128,sizeof(struct example2_s));

	int tag=0;

	dmaGetnWait(ex2,argp,envp,tag);

	int msg;
	do {
		printf("SPU: The value in example1 data is %d\n",ex2->myint);
/*
		msg=RDY;*/
// 		uint8_t a = ex2->myint;
		spu_stop(0x0000);

// 		send_message_int(&msg);
//
// 		msg=wait_for_message_poll();
		//if (msg != STOP) {
		printf("SPU: Incrementing example data value\n");
		ex2->myint += 1;
		//}

	} while (1);

// 	return 0;
}