#include <speutils/spefifo.h>
#include <speutils/spethread.h>
#include <speutils/spemessage.h>




int main (int narg,char *argc[])
{
    init_fifo(128,128,1);
    //spu_thread_t *thread = spu_thread_allocate();

    sleep(20);

    //free(thread);

}
