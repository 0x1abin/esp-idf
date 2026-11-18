/* SPDX-License-Identifier: Apache-2.0 */

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sf32lb57_board.h"

extern void sf32lb57_start_scheduler_timebase(void);

void vPortSetupTimerInterrupt(void)
{
    volatile uint32_t *const control = (uint32_t *)0xe000e010;
    volatile uint32_t *const reload = (uint32_t *)0xe000e014;
    volatile uint32_t *const current = (uint32_t *)0xe000e018;
    const uint32_t cycles = configCPU_CLOCK_HZ / configTICK_RATE_HZ;
    configASSERT(cycles && cycles <= 0x1000000);
    *control = 0;
    *reload = cycles - 1;
    *current = 0;
    sf32lb57_start_scheduler_timebase();
    *control = 7;
}

void *pvPortMalloc(size_t size)
{
    return malloc(size);
}

void vPortFree(void *ptr)
{
    free(ptr);
}

void esp_vApplicationIdleHook(void)
{
    __asm__ volatile("wfi");
}

void vApplicationGetIdleTaskMemory(StaticTask_t **task, StackType_t **stack, uint32_t *stack_size)
{
    static StaticTask_t idle_task;
    static StackType_t idle_stack[configMINIMAL_STACK_SIZE / sizeof(StackType_t)];
    *task = &idle_task;
    *stack = idle_stack;
    *stack_size = sizeof(idle_stack);
}

#if configUSE_TIMERS
void vApplicationGetTimerTaskMemory(StaticTask_t **task, StackType_t **stack, uint32_t *stack_size)
{
    static StaticTask_t timer_task;
    static StackType_t timer_stack[configTIMER_TASK_STACK_DEPTH / sizeof(StackType_t)];
    *task = &timer_task;
    *stack = timer_stack;
    *stack_size = sizeof(timer_stack);
}
#endif

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    (void)name;
    sf32lb57_board_restart();
}
