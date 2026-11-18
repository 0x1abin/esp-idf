/* SPDX-License-Identifier: Apache-2.0 */
#pragma once

#include "portmacrocommon.h"

#define portARCH_NAME "Cortex-M33"
#define portDONT_DISCARD __attribute__((used))
#define portDISABLE_INTERRUPTS() ulSetInterruptMask()
#define portENABLE_INTERRUPTS() vClearInterruptMask(0)
#define portCHECK_IF_IN_ISR() xPortIsInsideInterrupt()
#define portASSERT_IF_IN_ISR() configASSERT(!xPortIsInsideInterrupt())

/* A single HCPU uses the interrupt mask to protect all kernel critical sections. */
typedef unsigned int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0U
#define portMUX_INITIALIZE(mux) (*(mux) = portMUX_INITIALIZER_UNLOCKED)
#undef portENTER_CRITICAL
#undef portEXIT_CRITICAL
#define portENTER_CRITICAL(mux) do { (void)(mux); vPortEnterCritical(); } while (0)
#define portEXIT_CRITICAL(mux) do { (void)(mux); vPortExitCritical(); } while (0)

#define portVALID_STACK_MEM(ptr) ((ptr) != NULL)
#define portVALID_LIST_MEM(ptr) ((ptr) != NULL)
#define portVALID_TCB_MEM(ptr) ((ptr) != NULL)
