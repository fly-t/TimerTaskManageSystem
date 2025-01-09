/**
 * @file TTMS.h
 * @brief 软件定时器, 使用一个硬件定时器
 * @author Tengfei Zhang (izhangtengfei@163.com)
 * @version 1.0
 * @date 2024-06-13 18:16
 *
 * @copyright Copyright (c) 2024  Tengfei Zhang All rights reserved
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2024-06-13 18:16 <td>1.0     <td>wangh     <td>内容
 * </table>
 */
#ifndef __TTMS_H__
#define __TTMS_H__

#include <stdbool.h>
#include "stm32F1xx.h"
#include "klist.h"

typedef void (*task_entry_fn)(void);

struct task
{
    link_t self;            /// 链表
    uint8_t id;             /// 任务ID
    char name[8];           /// 任务名称
    uint32_t period_ms;     ///  周期ms
    uint32_t elapsed_ms;    ///  已用ms
    task_entry_fn callback; /// 回调函数
    uint8_t is_run;         /// 是否执行回调函数标记
};

typedef struct task *task_t;

struct TTMS
{
    uint8_t id_counter; /// 定时器ID计数
    /* 定时器创建 */
    void (*create)(task_t task, char *name, uint32_t period_ms, task_entry_fn callback);
    /* 定时器打开 */
    void (*open)(task_t task);
    /* 定时器关闭 */
    void (*close)(task_t task);
    /* 调度器启动 */
    void (*start_schedule)(void);
};
typedef struct TTMS *TTMS_t;
extern struct TTMS ttms_obj;

/**
 * @brief 测试用例
 * @return * void
 */
void ttms_demo(void);
void ttms_dispatch_tasks(void);
#endif /* __TTMS_H__ */
