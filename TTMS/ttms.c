/**
 * @file ttms.c
 * @brief 定时器任务管理系统
 * @author Tengfei Zhang (izhangtengfei@163.com)
 * @version 1.0
 * @date 2025-01-09 11:59
 *
 * @copyright Copyright (c) 2025  Tengfei Zhang All rights reserved
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-01-09 11:59 <td>1.0     <td>wangh     <td>内容
 * </table>
 */
#include "ttms.h"
#include "tim.h"
#include <string.h>

/* 移植:硬件定时器选择 */
#define USING_TIMERx htim7

// 任务链表头
static KLIST_HEAD(TTMS_llist);

void ttms_start(void);
void ttms_create(task_t task, char *name, uint32_t period_ms, task_entry_fn callback);
void ttms_open(task_t task);
void ttms_close(task_t task);
void ttms_update(void);

struct TTMS ttms_obj = {
    .id_counter = 0,
    .create = ttms_create,
    .close = ttms_close,
    .open = ttms_open,
    .start_schedule = ttms_start,
};

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
        ttms_update();
    }
}

/**
 * @brief 打开硬件定时器中断,开始计时.
 * @return * void
 */
void ttms_start(void)
{
    HAL_TIM_Base_Start_IT(&USING_TIMERx);
}

/**
 * @brief 每1ms进中断一次, 检查时间是否到达, 到达则调用回调函数,复位时间
 * @return * void
 */
void ttms_update(void)
{
    task_t task;
    list_for_each_entry(task, &TTMS_llist, self)
    {
        task->elapsed_ms += 1;
         if (task->elapsed_ms >= task->period_ms)
        {
						task->is_run=1;
        }
    }
}


/**
 * @brief 调度需要执行的任务
 * @return * void
 */
void ttms_dispatch_tasks (void)
{
    task_t task;
    list_for_each_entry(task, &TTMS_llist, self)
    {
        if (task->is_run == 1)
        {
            task->callback();
            task->is_run = 0;
            task->elapsed_ms = 0;
        }
    }
}

/**
 * @brief 创建新的任务, 并绑定回调函数
 * @param  task_t     任务句柄
 * @param  name             任务名称,最长8个字符
 * @param  period_ms        任务触发周期
 * @param  callback         周期触发回调函数
 * @return * void
 */
void ttms_create(task_t task, char *name, uint32_t period_ms, task_entry_fn callback)
{
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0'; // 确保字符串以 '\0' 结尾
    task->period_ms = period_ms;
    task->elapsed_ms = 0;
    task->callback = callback;
    task->id = ttms_obj.id_counter++;

    LINK_INIT(&(task->self));
    /* 创建后自动调用 */
    ttms_open(task);
}

/**
 * @brief 关闭任务, 将任务移除链表
 * @param  timerx           任务指针
 * @return * void
 */
void ttms_close(task_t task)
{
    list_del(&task->self);
}

/**
 * @brief 打开任务, 将任务添加到链表
 * @param  timerx           任务指针
 * @return * void
 */
void ttms_open(task_t task)
{
    list_add_tail(&task->self, &TTMS_llist);
}

#include <stdio.h>
void task1_enrty(void)
{
    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    printf("123\n");
}

void task2_enrty(void)
{
    HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    printf("\n456\n");
}
void task3_enrty(void)
{
    HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
}

void ttms_demo(void)
{
    static struct task task1, task2, task3;
    ttms_obj.create(&task1, "task_1", 100, task1_enrty);
    ttms_obj.create(&task2, "task_2", 1000, task2_enrty);
    ttms_obj.create(&task3, "task_3", 1500, task3_enrty);
    ttms_obj.close(&task3);
    ttms_obj.open(&task3);
    ttms_obj.start_schedule();
}
