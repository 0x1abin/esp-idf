/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sf32lb57_board.h"

extern void __libc_init_array(void);
extern void app_main(void);
extern void sf32lb57_freertos_systick(void);

static uint32_t hal_tick_fraction;
static bool scheduler_timebase;

static void main_task(void *arg)
{
    (void)arg;
    app_main();
    vTaskDelete(NULL);
}

void entry(void)
{
    sf32lb57_board_init();
    __libc_init_array();
    if (xTaskCreate(main_task, "main", CONFIG_SF32LB57_MAIN_TASK_STACK_SIZE, NULL, 1, NULL) == pdPASS) {
        vTaskStartScheduler();
    }
    sf32lb57_board_restart();
}

void sf32lb57_start_scheduler_timebase(void)
{
    scheduler_timebase = true;
}

void SysTick_Handler(void)
{
    if (!scheduler_timebase) {
        sf32lb57_board_tick();
        return;
    }

    hal_tick_fraction += 1000;
    while (hal_tick_fraction >= configTICK_RATE_HZ) {
        sf32lb57_board_tick();
        hal_tick_fraction -= configTICK_RATE_HZ;
    }
    sf32lb57_freertos_systick();
}

int _write(int fd, const void *data, int size)
{
    if ((fd != 1 && fd != 2) || size < 0) {
        errno = EBADF;
        return -1;
    }

    int written = 0;
    const uint8_t *bytes = data;
    while (written < size) {
        uint16_t chunk = size - written > 512 ? 512 : size - written;
        int count = sf32lb57_console_write(bytes + written, chunk);
        if (count < 0) {
            errno = EIO;
            return written ? written : -1;
        }
        written += count;
    }
    return written;
}
