# What is TTMS? 

TTMS 全称Timer Task Management System(定时器任务管理系统)

写这个是因为没有一个比较好的框架在裸机环境下编写程序.
兼顾程序的可读性和实用性, 所以写了这个.

基于定时器的时间片调度框架

适合小资源裸跑的机器

配合状态机食用效果更佳


# 整体架构

通过硬件定时器的中断进行调度, 对链表中的任务进行时间++, 如果时间达到设置的周期时间, 触发回调函数, 恢复标志位.

# 移植

主要使用的是stm32单片机, 后续对硬件定时器部分抽象出一层可进一步简化其他平台的移植工作.

1. 配置好定时器中断(默认1ms)
2. 定时器中断处理函数中更新计数


``` c
/* 移植:硬件定时器选择 */
#define USING_TIMERx htim7

/**
 * @brief stm32 定时器中断处理函数:周期结束回调函数，在定时器更新事件中调用
 * @brief 这里移植需要修改
 * @param  htim
 * @return * void
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == USING_TIMERx.Instance)
    {
        software_timer_update();
    }
}
```
