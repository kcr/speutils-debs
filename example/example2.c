

#include <speutils/spethread.h>
#include <speutils/spemessage.h>
#include <malloc.h>

#include "example2.h"

extern spe_program_handle_t spu_example2_handle;

pthread_cond_t condvar=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void callback(int value) {

	printf("PPU: Callback from spu with value %d\n",value);

	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&condvar);
	pthread_mutex_unlock(&mutex);

}

int main (int narg, char *argv[]) {

	/** we need some example data to send to the spe */
	struct example2_s *ex2;

	/** allocate the example data in ram */
	ex2=memalign(128,sizeof(struct example2_s));

	/** We set some value to the example data */
	ex2->myint=1;

	/** we need a speutils thread structure */
	spu_thread_t sput;

	/** set the argument pointer in the thread structure to point to the example data */
	sput.argp = ex2;

	/** set the size of the argument to the thread structure to the size of the example data */
	sput.envp =(void *) (sizeof(struct example2_s));

	/** set the type of thread you want to run in the example */
	sput.type=THREAD_LOOP;

	/** set the callback */
	sput.callback=callback;

	/** set the program handle so that the thread has a program to execute */
	sput.handle=spu_example2_handle;

	/** initiate mutex and cond var for the thread */
	pthread_mutex_init(&sput.mutex,0);
	pthread_cond_init(&sput.condvar,0);


	/** start the spu thread and spu respectively */
	if (start_spu_thread(&sput) < 0 )
		return -1;


	char *cptr;
	char buffer[255];

	do {


		printf("PPU: Waiting for message from SPU\n");
		/** wait for a message from the spu */
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condvar, &mutex);
		pthread_mutex_unlock(&mutex);


	//	msg = recieve_message_int(&sput);
		printf("PPU: Message Recieved from SPU\nContinue y / n\n");
		cptr=fgets(buffer,255,stdin);

		/** send message back to the spu */
		if ((cptr[0] == 'y' ) || ( cptr[0] == 'Y')) {
			printf("PPU: Continue\n");
			sput.action=THREAD_CONTINUE;
			send_action(&sput);
		} else {
			printf("PPU: Exit\n");
			sput.action=THREAD_EXIT;
			send_action(&sput);
			break;
		}


	} while(1);
	/** wait for the spe thread to stop */
	stop_spu_thread(&sput);

	pthread_mutex_destroy(&sput.mutex);
	pthread_cond_destroy(&sput.condvar);
	/** free the allocated example structure */
	free(ex2);

	return 0;
}
