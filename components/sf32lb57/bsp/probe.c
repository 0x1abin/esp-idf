/* SPDX-License-Identifier: Apache-2.0 */

/* Vendor SCons links a minimal application to produce its normal board images
 * and sftool manifest. This translation unit is excluded from the BSP archive.
 */
#include "sf32lb57_board.h"

void entry(void)
{
    sf32lb57_board_init();
    for (;;) {
    }
}
