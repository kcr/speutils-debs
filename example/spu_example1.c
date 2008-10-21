#include <stdio.h>

#include <speutils/spu_message.h>
#include <speutils/spu_dmalib.h>


#include "example1.h"

int main (unsigned long long speid, unsigned long long argp, unsigned long long envp) {

	struct example1_s *ex1;

	ex1=(struct example1_s *)memalign(128,sizeof(struct example1_s));

	int tag=0;

	dmaGetnWait(ex1,argp,envp,tag);



	int msg;
	do {
		printf("SPU: The value in example1 data is %d\n",ex1->myint);

		msg=RDY;

		send_message_int(&msg);

		msg=wait_for_message_poll();
		if (msg != STOP) {
			printf("SPU: Incrementing example data value\n");
			ex1->myint += 1;
		}

	} while (msg != STOP);

	return 0;
}