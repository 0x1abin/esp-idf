# SPDX-License-Identifier: Apache-2.0

import json
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools.sf32lb57 import image


class TestAppFlash(unittest.TestCase):
    def setUp(self) -> None:
        workspace = tempfile.TemporaryDirectory()
        self.addCleanup(workspace.cleanup)
        self.directory = Path(workspace.name)
        self.files = {
            image.FLASH_BASE: b'flash table',
            image.BOOTLOADER: b'bootloader',
            image.APP: b'HCPU application',
        }
        manifest_files = []
        for address, content in self.files.items():
            path = self.directory / f'{address:#x}.bin'
            path.write_bytes(content)
            manifest_files.append({'path': path.name, 'address': f'{address:#x}'})
        self.manifest = self.directory / 'sftool_param.json'
        self.manifest.write_text(
            json.dumps(
                {
                    'chip': 'SF32LB57',
                    'memory': 'NOR',
                    'write_flash': {'files': manifest_files},
                }
            )
        )
        self.command = ['sftool', '-p', 'test-uart', '-b', '1000000']
        self.installed_layout = {address: self.files[address] for address in (image.FLASH_BASE, image.BOOTLOADER)}

    def run_sftool(self, args: list[str], *, cwd: Path, check: bool) -> None:
        self.assertTrue(check)
        self.assertEqual(args[:9], [*self.command, '-c', 'SF32LB57', '-m', 'nor'])
        if args[9] == 'read_flash':
            self.assertEqual(len(args[10:]), len(self.installed_layout))
            for entry in args[10:]:
                name, region = entry.split('@')
                address_text, length = region.split(':')
                address = int(address_text, 0)
                self.assertEqual(int(length), len(self.files[address]))
                (cwd / name).write_bytes(self.installed_layout[address])
        else:
            self.assertEqual(args[9:], ['write_flash', '--verify', 'app.bin@0x14020000'])
            self.assertEqual((cwd / 'app.bin').read_bytes(), self.files[image.APP])

    def test_matching_layout_writes_only_application(self) -> None:
        with patch.object(image.subprocess, 'run', side_effect=self.run_sftool) as run:
            image.app_flash(self.command, self.manifest)
        self.assertEqual([call.args[0][9] for call in run.call_args_list], ['read_flash', 'write_flash'])

    def test_mismatched_layout_stops_before_write(self) -> None:
        for address in self.installed_layout:
            with self.subTest(address=address):
                self.installed_layout[address] = b'\xff' * len(self.files[address])
                with patch.object(image.subprocess, 'run', side_effect=self.run_sftool) as run:
                    with self.assertRaisesRegex(ValueError, rf'does not match NOR at {address:#x}'):
                        image.app_flash(self.command, self.manifest)
                run.assert_called_once()
                self.assertEqual(run.call_args.args[0][9], 'read_flash')
                self.installed_layout[address] = self.files[address]

    def test_read_failure_stops_before_write(self) -> None:
        with patch.object(image.subprocess, 'run', side_effect=subprocess.CalledProcessError(1, 'sftool')) as run:
            with self.assertRaises(subprocess.CalledProcessError):
                image.app_flash(self.command, self.manifest)
        run.assert_called_once()
        self.assertEqual(run.call_args.args[0][9], 'read_flash')
