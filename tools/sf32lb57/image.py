# SPDX-License-Identifier: Apache-2.0
"""Build and program the SiFli reference board's flash manifest."""

import argparse
import json
import os
import shutil
import struct
import subprocess
from pathlib import Path

FLASH_BASE = 0x14000000
BOOTLOADER = FLASH_BASE + 0x10000
APP = FLASH_BASE + 0x20000
APP_END = APP + 0x700000
HCPU_RAM_START = 0x20000000
HCPU_RAM_END = HCPU_RAM_START + 0x6FC00
FLASH_FILES = {
    FLASH_BASE: ('ftab/ftab.bin', BOOTLOADER),
    BOOTLOADER: ('bootloader/bootloader.bin', APP),
    APP: ('main.bin', APP_END),
}


def package(bsp: Path, binary: Path, manifest_file: Path) -> None:
    with binary.open('rb') as app:
        vectors = app.read(8)
    if len(vectors) != 8:
        raise ValueError(f'{binary} has a truncated Cortex-M vector table')
    stack, reset = struct.unpack('<II', vectors)
    if stack % 8 or not HCPU_RAM_START < stack <= HCPU_RAM_END:
        raise ValueError(f'{binary} has an invalid HCPU stack pointer {stack:#x}')
    if not reset & 1 or not APP <= reset - 1 < APP + binary.stat().st_size:
        raise ValueError(f'{binary} has an invalid Thumb reset vector {reset:#x}')

    board_manifest = json.loads((bsp / 'sftool_param.json').read_text())
    if board_manifest['chip'] != 'SF32LB57' or board_manifest['memory'].lower() != 'nor':
        raise ValueError('The BSP manifest is not for the SF32LB57 reference board NOR flash')
    files = {}
    for item in board_manifest['write_flash']['files']:
        address = int(item['address'], 0)
        if address not in FLASH_FILES or address in files or item['path'] != FLASH_FILES[address][0]:
            raise ValueError(f'Unexpected entry in BSP flash manifest: {item}')
        image = binary if address == APP else bsp / item['path']
        if not 0 < image.stat().st_size <= FLASH_FILES[address][1] - address:
            raise ValueError(f'{image} does not fit the flash region at {address:#x}')
        files[address] = {
            'path': os.path.relpath(image, manifest_file.parent),
            'address': f'{address:#x}',
        }

    if files.keys() != FLASH_FILES.keys():
        raise ValueError('The BSP manifest must specify the flash table, bootloader, and HCPU application')
    board_manifest['write_flash'] = {
        'verify': True,
        'files': [files[address] for address in (BOOTLOADER, APP, FLASH_BASE)],
    }
    temporary = manifest_file.with_suffix('.tmp')
    temporary.write_text(json.dumps(board_manifest, indent=2) + '\n')
    temporary.replace(manifest_file)


def flash(manifest_file: Path) -> None:
    port = os.environ.get('ESPPORT')
    if not port:
        raise ValueError('Select the SF32LB57 UART with idf.py -p PORT flash.')
    sftool = shutil.which('sftool')
    if not sftool:
        raise ValueError('Install the SiFli sftool CLI and add it to PATH.')
    baud = os.environ.get('ESPBAUD', '1000000')
    subprocess.run([sftool, '-p', port, '-b', baud, 'config', manifest_file.name],
                   cwd=manifest_file.parent, check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest='command', required=True)
    pack = commands.add_parser('package')
    pack.add_argument('bsp', type=Path)
    pack.add_argument('binary', type=Path)
    pack.add_argument('manifest', type=Path)
    commands.add_parser('flash').add_argument('manifest', type=Path)
    args = parser.parse_args()
    try:
        if args.command == 'package':
            package(args.bsp, args.binary, args.manifest)
        else:
            flash(args.manifest)
    except (OSError, subprocess.CalledProcessError, ValueError) as exc:
        parser.exit(1, f'{exc}\n')


if __name__ == '__main__':
    main()
