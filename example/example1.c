#include <speutils/spethread.h>
#include <speutils/spemessage.h>

#include "example1.h"

extern spe_program_handle_t spu_example1_handle;

int main (int narg, char *argv[]) {

	/** we need some example data to send to the spe */
	struct example1_s *ex1;

	/** allocate the example data in ram */
	ex1=(struct example1_s *)memalign(128,sizeof(struct example1_s));

	/** We set some value to the example data */
	ex1->myint=1;

	/** we need a speutils thread structure */
	spu_thread_t sput;

	/** set the argument pointer in the thread structure to point to the example data */
	sput.argp = ex1;

	/** set the size of the argument to the thread structure to the size of the example data */
	sput.envp =(void *) (sizeof(struct example1_s));

	/** set the type of thread you want to run in the example */
	sput.type=THREAD_RUN;

	/** set the program handle so that the thread has a program to execute */
	sput.handle=spu_example1_handle;

	/** start the spu thread and spu respectively */
	if (start_spu_thread(&sput) < 0 )
		return -1;

	int msg;

	char *cptr;
	char buffer[255];

	do {
		printf("PPU: Waiting for message from SPU\n");
		/** wait for a message from the spu */
		msg = recieve_message_int(&sput);
		printf("PPU: Message Recieved from SPU\nContinue y / n\n");
		cptr=fgets(buffer,255,stdin);

		/** send message back to the spu */
		if ((cptr[0] == 'y' ) || ( cptr[0] == 'Y'))
			send_message(&sput,RUN);
		else {
			send_message(&sput,STOP);
			break;
		}


	} while(1);
	/** wait for the spe thread to stop */
	stop_spu_thread(&sput);

	/** free the allocated example structure */
	free(ex1);

	return 0;
}
