# EpollScheduler
Scheduler based on Epoll

## The Scheduler

The Scheduler is based on the timerfd in the linux system. The scheduler will first save the task callback function with timestamp in a **TODO LIST **, and then set the timerfd to timeout when needed.


If the task is not a repeated one, then it will be thrown, otherwise, the scheduler will insert the task into the **TODO LIST**;