#ifndef __SPU_TASKMANAGER_H
#define __SPU_TASKMANAGER_H

#define STAGES  4
#define SPU_TASK_BUFFER_SIZE 32

#define FREE_TASK_BUFFER_SLOT 0x31415926

enum states{WAIT,FETCH,STORE,WORK,SNOOZE};

/*
static inline int increment_stage(struct task_pipe_s *pipe, int stage)
{
    pipe->stage[stage]++;
    if (pipe->stage[stage] >= STAGES )
        pipe->stage[stage]=0;
}
*/

struct task_state_s {
   // int task[SPU_TASK_BUFFER_SIZE];
    int current;
    int in_back;
    int out_back;
    int front;
};

static inline init_taskstate()
{
   // task_back=task_front=0;
}

static inline int increment(int value, int boundary)
{
    value++;
    if (value  >= boundary)
        value = 0;
    return value;
}


static inline int in_tasks_remaining(struct task_state_s *state)
{
    if (state->in_back != state->front)
        return 1;
    else
        return 0;

}
#endif