/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

extern void sf32lb57_start_scheduler_timebase(void);
extern void sf32lb57_board_restart(void);
extern char __sf32lb57_heap_start[], __HeapLimit[];

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

void *heap_caps_malloc(size_t size, uint32_t caps)
{
    const uint32_t supported = MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_32BIT;
    return (caps & ~supported) == 0 ? malloc(size) : NULL;
}

void heap_caps_free(void *ptr)
{
    free(ptr);
}

void *_sbrk(ptrdiff_t increment)
{
    static uintptr_t next;
    const uintptr_t start = (uintptr_t)__sf32lb57_heap_start;
    const uintptr_t limit = (uintptr_t)__HeapLimit;
    if (!next) {
        next = start;
    }

    if ((increment >= 0 && (uintptr_t)increment > limit - next) ||
            (increment < 0 && (uintptr_t)(-(increment + 1)) + 1 > next - start)) {
        errno = ENOMEM;
        return (void *)-1;
    }
    uintptr_t previous = next;
    next += (uintptr_t)increment;
    return (void *)previous;
}

void __malloc_lock(struct _reent *context)
{
    (void)context;
    vTaskSuspendAll();
}

void __malloc_unlock(struct _reent *context)
{
    (void)context;
    xTaskResumeAll();
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
