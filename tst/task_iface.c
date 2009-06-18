#include <speutils/spefifo.h>
#include <speutils/spethread.h>
#include <speutils/spemessage.h>

int main (int narg,char *argc[])
{
    int i;
    int spes = 1;
    int ops=1;
    int type = 0;
    //default arg is 100
    int arg = 1000;
    if (narg >= 2)
        spes=atoi(argc[1]);
    if (narg >= 3)
        ops=atoi(argc[2]);
    if (narg >= 4)
        type =atoi(argc[3]);
    if (narg >= 5)
        arg = atoi(argc[4]);


    fifoInit(128*128,spes,ROUND_ROBIN);
    printf("spes %d\n",spes);
    printf("operations %d\n",ops);
    switch(type)
    {
        case 0:
            printf("Type = NOOP\n");
            for (i = 0; i < spes*ops ; i++)
            {
                fifoNoop();
            }
            break;

        case 1:
            printf("TYPE = ADDS\n");
            printf("ADDS = %d\n",arg);
            for (i = 0; i < spes*ops ; i++)
            {
                fifo_addition(arg);
            }
            break;
    }

    //spu_thread_t *thread = spu_thread_allocate();

    //sleep(20);
    while(!fifoIsEmpty());
    //waitFifo()
    //free(thread);

}
