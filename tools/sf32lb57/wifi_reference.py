# SPDX-License-Identifier: Apache-2.0
"""Build the vendor RT-Thread/AIC8800 reference for the SF32LB57 LCD board."""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

BOARD = 'sf32lb57-lcd_n16r8_dpi'
WIFI_CONFIG = {
    'CONFIG_BSP_USING_SDMMC1': 'n',
    'CONFIG_BSP_USING_SDMMC2': 'y',
    'CONFIG_SDIO2_CARD_MODE': '2',
    'CONFIG_BSP_WIFI_SWT6621': 'n',
    'CONFIG_PKG_USING_WPA_SUPPLICANT': 'n',
    'CONFIG_AIC_WLAN_AIC8800D40N': 'y',
    'CONFIG_AIC8800_USE_RT_WLAN': 'y',
    'CONFIG_AIC8800_RTOS_WIRELESS_PWR_VIA_IOEXP': 'y',
}


def build(sdk: Path, destination: Path, jobs: int) -> None:
    if (sdk / 'version.txt').read_text().strip() != 'v2.5.2':
        raise ValueError('Use SiFli SDK v2.5.2 for this reference.')
    compiler = shutil.which('arm-none-eabi-gcc')
    scons = shutil.which('scons')
    if not compiler or not scons:
        raise ValueError('Add the SiFli Arm GNU toolchain and SCons to PATH.')
    if jobs < 1:
        raise ValueError('The build job count must be positive.')
    if destination.is_relative_to(sdk):
        raise ValueError('Choose a destination outside the SiFli SDK checkout.')

    example = sdk / 'example/rt_device/wifi'
    shutil.copytree(example, destination, ignore=shutil.ignore_patterns('build_*', '__pycache__'))
    # The vendor Wi-Fi build writes object files and generated certificates
    # to its SDK tree. Keep them with this reference build.
    sdk_copy = destination / '.sifli-sdk'
    shutil.copytree(sdk, sdk_copy, symlinks=True, ignore=shutil.ignore_patterns('build_*', '__pycache__'))
    project = destination / 'project'
    config = project / 'proj.conf'
    upstream = config.read_text().splitlines()
    overrides = [f'{option}={value}' for option, value in WIFI_CONFIG.items()]
    config.write_text('\n'.join([line for line in upstream if line.split('=', 1)[0] not in WIFI_CONFIG] +
                                overrides) + '\n')
    env = os.environ.copy()
    env.update({
        'SIFLI_SDK': str(sdk_copy),
        'PYTHONPATH': os.pathsep.join(str(sdk_copy / entry) for entry in ('tools/build', 'tools')),
        'RTT_CC': 'gcc',
        'RTT_EXEC_PATH': str(Path(compiler).parent),
    })
    subprocess.run([scons, f'--board={BOARD}', f'-j{jobs}'], cwd=project, env=env, check=True)
    print(project / f'build_{BOARD}_hcpu' / 'sftool_param.json')


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk', type=Path, default=os.environ.get('SIFLI_SDK'), required=not os.environ.get('SIFLI_SDK'))
    parser.add_argument('-j', '--jobs', type=int, default=4)
    parser.add_argument('destination', type=Path, help='New directory for the reference project')
    args = parser.parse_args()
    try:
        build(args.sdk.resolve(), args.destination.resolve(), args.jobs)
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f'{exc}\n')
    return 0


if __name__ == '__main__':
    sys.exit(main())
