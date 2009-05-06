#include <speutils/spefifo.h>
#include <speutils/spethread.h>
#include <speutils/spemessage.h>




int main (int narg,char *argc[])
{
    int i;
    int spes = 1;
    int noops=1;
    if (narg >= 2)
        spes=atoi(argc[1]);
    if (narg >= 3)
        noops=atoi(argc[2]);
    fifoInit(128*128,spes,ROUND_ROBIN);
    printf("spes %d\n",spes);
    printf("noops %d\n",noops);
    //spu_thread_t *thread = spu_thread_allocate();
    for (i = 0; i < spes*noops ; i++)
    {
        fifoNoop();
    }
    sleep(20);

    //free(thread);

}
