/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern char __sf32lb57_heap_start[], __HeapLimit[];

static const uint32_t supported_caps = MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_32BIT;
static char *heap_break = __sf32lb57_heap_start;
static size_t minimum_free = SIZE_MAX;
static unsigned int malloc_lock_depth;
static bool sampling;

void *heap_caps_malloc(size_t size, uint32_t caps)
{
    return (caps & ~supported_caps) == 0 ? malloc(size) : NULL;
}

void heap_caps_free(void *ptr)
{
    free(ptr);
}

void *_sbrk(ptrdiff_t increment)
{
    const uintptr_t start = (uintptr_t)__sf32lb57_heap_start;
    const uintptr_t limit = (uintptr_t)__HeapLimit;
    const uintptr_t current = (uintptr_t)heap_break;
    if ((increment >= 0 && (uintptr_t)increment > limit - current) ||
            (increment < 0 && (uintptr_t)(-(increment + 1)) + 1 > current - start)) {
        errno = ENOMEM;
        return (void *)-1;
    }
    heap_break = (void *)(current + (uintptr_t)increment);
    return (void *)current;
}

/* Called with scheduling suspended. Include unclaimed space and free chunks
 * in the newlib arena. mallinfo() itself takes the recursive malloc lock. */
static size_t sample_free_size(void)
{
    sampling = true;
    size_t free_size = (uintptr_t)__HeapLimit - (uintptr_t)heap_break + (size_t)mallinfo().fordblks;
    sampling = false;
    if (free_size < minimum_free) {
        minimum_free = free_size;
    }
    return free_size;
}

void __malloc_lock(struct _reent *context)
{
    (void)context;
    vTaskSuspendAll();
    ++malloc_lock_depth;
}

void __malloc_unlock(struct _reent *context)
{
    (void)context;
    if (malloc_lock_depth == 1 && !sampling) {
        sample_free_size();
    }
    --malloc_lock_depth;
    xTaskResumeAll();
}

size_t heap_caps_get_free_size(uint32_t caps)
{
    if (caps & ~supported_caps) {
        return 0;
    }
    vTaskSuspendAll();
    size_t free_size = sample_free_size();
    xTaskResumeAll();
    return free_size;
}

size_t heap_caps_get_minimum_free_size(uint32_t caps)
{
    if (caps & ~supported_caps) {
        return 0;
    }
    vTaskSuspendAll();
    if (minimum_free == SIZE_MAX) {
        sample_free_size();
    }
    size_t free_size = minimum_free;
    xTaskResumeAll();
    return free_size;
}
