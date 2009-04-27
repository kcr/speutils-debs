#ifndef __SPU_TASKMANAGER_H
#define __SPU_TASKMANAGER_H

#define SPU_TASK_BUFFER_SIZE 32

#define FREE_TASK_BUFFER_SLOT 0x31415926

enum states{WAIT,FETCH,STORE,WORK,SNOOZE};

struct task_state_s {
   // int task[SPU_TASK_BUFFER_SIZE];
    int current_task;
    int task_back;
    int task_front;
};

static inline init_taskstate()
{
   // task_back=task_front=0;
}

static inline int increment(int value, int boundary)
{
    if ((value + 1) >= boundary)
        return 0;
    else
        return value+1;
}
/*
static inline int free_tasks(struct task_buffer_s *tasks)
{
    if (tasks->task_back > tasks->task_front)
    {
        return tasks->task_back - tasks->task_front - 1;
    } else {
        return tasks->task_back + (SPU_TASK_BUFFER_SIZE - 1) - tasks->task_front;
    }
}

static inline void increment_task_count(int *count)
{
    *count=*count + 1;
    if (count == SPU_TASK_BUFFER_SIZE)
        *count=0;
}

static inline init_tasks(struct task_buffer_s *tasks)
{
    tasks->task_back=SPU_TASK_BUFFER_SIZE-1;
    tasks->task_front=0;
}

static inline void clear_task(struct task_buffer_s *tasks)
{

    increment_task_count(&tasks->task_back);
    tasks->task[tasks->task_back] = FREE_TASK_BUFFER_SLOT;
}

static inline int check_task(struct task_buffer_s *tasks)
{
    int new_front=tasks->task_front;
    increment_task_count(&new_front);

    if (tasks->task[new_front] == FREE_TASK_BUFFER_SLOT)
        return 1;
    else
        return 0;

}

static inline int add_task(struct task_buffer_s *tasks,int task)
{
    if (free_tasks(tasks) > 0)
    {
        increment_task_count(&tasks->task_front);
        tasks->task[tasks->task_front]=task;
        return 0;
     //   tasks->
    } else {
        return 1;
    }

}
*/
#endif