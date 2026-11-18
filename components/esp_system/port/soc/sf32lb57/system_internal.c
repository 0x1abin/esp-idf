/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <stdlib.h>
#include "esp_heap_caps.h"
#include "esp_private/system_internal.h"
#include "sf32lb57_board.h"

void esp_restart_noos(void)
{
    sf32lb57_board_restart();
}

uint32_t esp_get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

uint32_t esp_get_free_internal_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
}

uint32_t esp_get_minimum_free_heap_size(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
}

const char *esp_get_idf_version(void)
{
    return IDF_VER;
}

void esp_system_abort(const char *details)
{
    fprintf(stderr, "abort: %s\n", details);
    abort();
}

void _esp_error_check_failed_without_abort(esp_err_t rc, const char *file, int line,
                                          const char *function, const char *expression)
{
    fprintf(stderr, "%s:%d: %s: %s failed: %s (0x%x)\n",
            file, line, function, expression, esp_err_to_name(rc), (unsigned int)rc);
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line,
                            const char *function, const char *expression)
{
    _esp_error_check_failed_without_abort(rc, file, line, function, expression);
    abort();
}
