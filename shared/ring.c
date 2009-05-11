#include "ring.h"
#include <stdlib.h>
ring_context_t *ringInitContext(int size)
{
    if (size == 0)
        return NULL;
    ring_context_t *ringctx = malloc(sizeof(struct ring_context_s));//memalign(128,sizeof(struct ring_context_s));
    ringctx->back=0;
    ringctx->front=0;
    ringctx->size = size;
    return ringctx;
}

void ringFreeContext(ring_context_t *ctx)
{
    free(ctx);
}

int ringHasFree(ring_context_t *ctx)
{
    int fplus=ctx->front+1;

    if (fplus >= ctx->size)
        fplus=0;

    if (fplus == ctx->back)
        return 0;
    else
        return 1;
}
int ringHasRemaining(ring_context_t *ctx)
{
    return ctx->front == ctx->back ? 0 :1;
}

void ringIncFront(ring_context_t *ctx)
{
    ctx->front++;
    if (ctx->front >= ctx->size)
        ctx->front=0;
}
void ringIncBack(ring_context_t *ctx)
{
    ctx->back++;
    if (ctx->back >= ctx->size)
        ctx->back=0;
}
uint32_t ringGetFront(ring_context_t *ctx){return ctx->front;}
uint32_t ringGetBack(ring_context_t *ctx){return ctx->back;}
uint32_t ringReset(ring_context_t *ctx){ctx->back=ctx->front=0;}