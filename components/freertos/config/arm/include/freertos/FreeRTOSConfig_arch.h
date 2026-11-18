/* SPDX-License-Identifier: Apache-2.0 */
#pragma once

#define configENABLE_FPU                         1
#define configENABLE_MPU                         0
#define configENABLE_TRUSTZONE                   0
#define configENABLE_MVE                         0
#define configRUN_FREERTOS_SECURE_ONLY           1

/* The SiFli CMSIS headers specify three implemented NVIC priority bits. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     (5U << 5)
#define configMAX_API_CALL_INTERRUPT_PRIORITY    configMAX_SYSCALL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY          (7U << 5)
#define configMINIMAL_STACK_SIZE                 1024
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  0
#define configUSE_NEWLIB_REENTRANT                1
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H 1
#define configTOTAL_HEAP_SIZE                    (96 * 1024)
#define INCLUDE_xTaskDelayUntil                  1
#define INCLUDE_uxTaskGetStackHighWaterMark2      1

#ifndef __ASSEMBLER__
#include <stdint.h>
extern uint32_t sf32lb57_cpu_hz(void);
#define configCPU_CLOCK_HZ                  sf32lb57_cpu_hz()
#define portGET_RUN_TIME_COUNTER_VALUE()  xTaskGetTickCount()
#endif
