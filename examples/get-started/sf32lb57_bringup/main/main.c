/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "sf32lb57_board.h"

static void timer_elapsed(TimerHandle_t timer)
{
    TaskHandle_t task = pvTimerGetTimerID(timer);
    xTaskNotifyGive(task);
}

void app_main(void)
{
    puts("SF32LB57 HCPU: app_main running");
    uint16_t vendor, device;
    int result = sf32lb57_probe_sdio(&vendor, &device);
    if (result == 0) {
        printf("SF32LB57 SDIO: vendor=%04x device=%04x\n", (unsigned)vendor, (unsigned)device);
    } else {
        printf("SF32LB57 SDIO: probe failed at stage %d\n", -result);
    }
    TimerHandle_t timer = xTimerCreate("heartbeat", pdMS_TO_TICKS(1000), pdTRUE,
                                      xTaskGetCurrentTaskHandle(), timer_elapsed);
    if (timer == NULL || xTimerStart(timer, portMAX_DELAY) != pdPASS) {
        puts("SF32LB57 HCPU: failed to start heartbeat timer");
        return;
    }

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        printf("SF32LB57 HCPU: timer tick %lu\n", (unsigned long)xTaskGetTickCount());
    }
}
