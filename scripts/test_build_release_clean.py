"""Verify real generation/cleanup in a temporary project, without changing the checkout.

Requires PowerShell and the checked-out lib modules. Pass --full to include the
complete -SkipFirmware packaging flow (also requires OpenKNXproducer).
"""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

REPO = Path(__file__).resolve().parent.parent


def run(fixture, *args):
    result = subprocess.run(
        ['pwsh', '-NoProfile', *args], cwd=fixture, capture_output=True, text=True
    )
    if result.returncode:
        raise AssertionError(result.stdout + result.stderr)
    return result.stdout


with tempfile.TemporaryDirectory(prefix='neopixel-build-clean-') as directory:
    fixture = Path(directory)
    for folder in ('scripts', 'src', 'doc', 'include'):
        shutil.copytree(REPO / folder, fixture / folder)
    (fixture / 'lib').symlink_to(REPO / 'lib', target_is_directory=True)
    for source in REPO.iterdir():
        if source.is_file():
            shutil.copy2(source, fixture / source.name)

    # These are the two previously missed round-trip cases. The manifest is a
    # checked-in snapshot; the Studio map follows the other empty generated blocks.
    dependencies = (REPO / 'dependencies.txt').read_bytes()
    script = fixture / 'src/NeoPixel.script.js'
    baseline = script.read_bytes()
    assert b'var NEO_StudioEffectParameters = {};' in baseline
    if '--full' in sys.argv:
        run(fixture, '-File', 'scripts/Build-Release.ps1', '-SkipFirmware')
    else:
        run(fixture, '-File', 'scripts/Build-HardwareConfig.ps1', '-UniversalBuild')
        run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-SkipOpenKNXproducer')
        run(fixture, '-Command',
            '. ./scripts/Initialize-BuildGit.ps1; '
            '& ./lib/OGM-Common/scripts/setup/reusable/Build-Dependencies.ps1')
    assert b'var NEO_StudioEffectParameters = {};' not in script.read_bytes()
    assert (fixture / 'dependencies.txt').read_bytes() == dependencies, 'dependency snapshot changed'

    run(fixture, '-File', 'scripts/Build-Release.ps1', '-Clean')
    assert script.read_bytes() == baseline, 'ETS script did not return to the clean baseline'
    assert (fixture / 'dependencies.txt').read_bytes() == dependencies, 'cleanup changed dependency snapshot'
    assert not (fixture / 'src/EffectParameterMapping.h').exists()
    assert not list((fixture / 'src').glob('NeoPixel.Effects.*.generated.xml'))
    run(fixture, '-File', 'scripts/Build-Release.ps1', '-Clean')
    assert script.read_bytes() == baseline, 'cleanup is not idempotent'
    print('Generation and repeated cleanup preserve the ETS script baseline and all dependency hashes.')
