/*
*  C Implementation: ring_test
*
* Description:
*
*
* Author: kristian jerpetjøn <kristian.jerpetjoen@gmail.com>, (C) 2009
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <../shared/ring.h>

int main(int narg,char *argc[])
{
    int i;
    ring_context_t *context=ringInitContext(100);
    printf("filling fifo\n");
    while(ringHasFree(context))
    {
        printf("front %d, back %d\n",ringGetFront(context),ringGetBack(context));
        ringIncFront(context);
    }

    printf("rolling fifo\n");
    for (i = 0 ; i < 100; i++)
    {
        printf("front %d, back %d\n",ringGetFront(context),ringGetBack(context));
        ringIncBack(context);
        ringIncFront(context);
    }

    printf("emptying fifo\n");
    while(ringHasRemaining(context))
    {
        printf("front %d, back %d\n",ringGetFront(context),ringGetBack(context));
        ringIncBack(context);
    }

    printf("final status\n");
    printf("front %d, back %d\n",ringGetFront(context),ringGetBack(context));
    ringFreeContext(context);
    printf("free\n");


}
