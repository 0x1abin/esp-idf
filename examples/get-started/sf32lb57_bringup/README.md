# SF32LB57 HCPU bring-up

This example builds an ESP-IDF v6.1.1 application for the SiFli
`sf32lb57-lcd_n16r8_dpi` board. The HCPU runs the ESP-IDF single-core FreeRTOS
kernel with the Arm Cortex-M33 port from FreeRTOS V10.5.1. The build uses the
SiFli v2.5.2 BSP, bootloader and NOR flash table.

The application prints an `app_main` message, reads the SDIO manufacturer and
device IDs from the Wi-Fi module, and exercises a FreeRTOS software timer.
The SDIO probe briefly powers the board's AIC8800 module via the AW9523,
enumerates it through SDMMC2, then switches it off. A module fitted with the
supported AIC8800D40-N family should report vendor `c8a1`. Use the board's
Wi-Fi pin routing and disconnect a camera or other peripheral sharing SDMMC2.

## Build on Linux

Install CMake 3.22 or later and Ninja. From the ESP-IDF fork, use the standard
installer and export script to install and activate SiFli Arm GNU 14.2.1 and
sftool 0.2.5. Set `SIFLI_SDK` to a SiFli SDK v2.5.2 checkout with submodules.

```bash
git clone --branch v2.5.2 --recurse-submodules https://github.com/OpenSiFli/SiFli-SDK ../SiFli-SDK
export SIFLI_SDK="$(realpath ../SiFli-SDK)"
./install.sh sf32lb57
. ./export.sh
python -m pip install -r "$SIFLI_SDK/tools/requirements/requirements.core.txt"
idf.py -C examples/get-started/sf32lb57_bringup set-target sf32lb57
idf.py -C examples/get-started/sf32lb57_bringup build
```

The upstream `examples/get-started/hello_world` also builds unchanged for
`sf32lb57`. It uses the HCPU chip information, external MPI3 NOR capacity,
newlib heap statistics, FreeRTOS delays, and board reset through the ESP-IDF
public APIs. The in-application NOR API currently supports capacity and reads.

The build downloads a checksum-pinned FreeRTOS V10.5.1 source archive for its
Arm port. For an offline build set `SF32LB57_FREERTOS_PATH` to a checkout of
FreeRTOS-Kernel at `V10.5.1` before configuring.

## Flash and observe

```bash
idf.py -C examples/get-started/sf32lb57_bringup -p /dev/ttyUSB0 flash monitor
```

Replace the example port with the board's UART. Flash writes the SiFli
bootloader, HCPU application and flash table with readback verification. Use
a development board set aside for this test because flashing replaces its
current firmware. The monitor uses the board's 1,000,000 baud console and
exits on Ctrl+]. After reset, look for `SF32LB57 HCPU: app_main running`,
the SDIO IDs, and one timer line per second. A probe stage of 1 identifies
AW9523 setup, 2 SDIO commands, 3 card power readiness, 4 card selection,
5 the manufacturer tuple, and 6 power-off.

After installing the reference boot layout with `flash`, use `app-flash` to
update the HCPU application. It reads and compares the installed bootloader
and flash table with the build output before writing the application with
readback verification. Use `flash` if the board has a different boot layout.

```bash
idf.py -C examples/get-started/sf32lb57_bringup -p /dev/ttyUSB0 app-flash monitor
```

`app-flash` writes the whole HCPU application image. Flashing only changed
sectors requires further work and validation on the reference board.

## Original RT-Thread Wi-Fi reference

```bash
python "$IDF_PATH/tools/sf32lb57/wifi_reference.py" /tmp/sf32lb57-wifi-reference
```

The helper builds SiFli's WLAN example with AIC8800D40-N, SDMMC2, and
AW9523 power control. It keeps a private SDK copy in the output directory
for the files SCons generates alongside the vendor sources. On the
reference firmware, use the SiFli shell to
scan, associate to a WPA2 access point, obtain an IPv4 address, and ping a
local host. Use the same board and access point for the FreeRTOS radio test.

The current fork has the ARM build and the SDIO enumeration probe. The
remaining milestones are the AIC8800 firmware/data path on FreeRTOS, NVS
on board NOR, and the ESP-IDF Station, HTTPS, and MQTT integration. Both
reference images have been cross-built. UART, SDIO, and network behavior
require a physical reference board.
