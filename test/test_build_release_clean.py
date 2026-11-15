"""Verify generation/cleanup of the committed effect outputs in a temporary project, without changing the checkout.

The effect generator outputs are committed and listed in src/EffectParameters.fingerprint. Checked here:
the committed state is current (-Check), Build-Release -Clean leaves them untouched, Build-EffectParameters -Clean
empties/deletes them (idempotent, -Check then reports outdated), one generator run restores every output
byte-identical. Help file restore and orphan removal: test/Test-EffectChanges.ps1.

Requires PowerShell and the checked-out lib modules. Pass --full to include the
complete -SkipFirmware packaging flow (also requires OpenKNXproducer).
"""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET

REPO = Path(__file__).resolve().parent.parent
FINGERPRINT = 'src/EffectParameters.fingerprint'


def run(fixture, *args, check=True):
    result = subprocess.run(
        ['pwsh', '-NoProfile', *args], cwd=fixture, capture_output=True, text=True
    )
    if check and result.returncode:
        raise AssertionError(result.stdout + result.stderr)
    return result


def effect_check(fixture):
    return run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-Check', check=False).returncode


def listed_outputs(fixture):
    lines = (fixture / FINGERPRINT).read_text(encoding='utf-8').splitlines()
    assert lines[1].startswith('sha256 '), 'fingerprint without input hash'
    return [line.split(' ', 1)[1] for line in lines[2:] if line]


def snapshot(fixture, paths):
    return {p: (fixture / p).read_bytes() if (fixture / p).exists() else None for p in paths}


with tempfile.TemporaryDirectory(prefix='neopixel-build-clean-') as directory:
    fixture = Path(directory)
    for folder in ('scripts', 'src', 'doc', 'include', 'web'):
        shutil.copytree(REPO / folder, fixture / folder)
    (fixture / 'lib').symlink_to(REPO / 'lib', target_is_directory=True)
    for source in REPO.iterdir():
        if source.is_file():
            shutil.copy2(source, fixture / source.name)

    dependencies = (REPO / 'dependencies.txt').read_bytes()
    outputs = listed_outputs(fixture)
    assert outputs, 'fingerprint lists no outputs'
    baseline = snapshot(fixture, outputs + [FINGERPRINT])
    assert all(value is not None for value in baseline.values()), 'a listed output is missing'
    studio = fixture / 'src/NeoPixel.script.src.js'
    assert b'var NEO_StudioEffectParameters = {};' not in studio.read_bytes(), 'committed Studio block is empty'
    assert effect_check(fixture) == 0, 'committed effect outputs are not current'

    def generate():
        run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-SkipOpenKNXproducer')

    def edit(path, old, new):
        target = fixture / path
        text = target.read_bytes()
        assert text.count(old) == 1, f'{path}: {old!r} not found once'
        target.write_bytes(text.replace(old, new))

    # an edit outside the generated blocks leaves the outputs current
    share = fixture / 'src/NeoPixel.share.xml'
    edit('src/NeoPixel.share.xml', b'</KNX>', b'<!-- hand note --></KNX>')
    assert effect_check(fixture) == 0, 'an edit outside the generated blocks counts as outdated'
    share.write_bytes(baseline['src/NeoPixel.share.xml'])

    # an edit inside a generated block is outdated; one run restores it
    edit('src/NeoPixel.Segment.templ.xml', b'Text="Speed (Wipe)" Value="25"', b'Text="Speed (Wipe)" Value="26"')
    assert effect_check(fixture) == 1, 'an edit inside a generated block counts as current'
    generate()
    assert snapshot(fixture, outputs + [FINGERPRINT]) == baseline, 'the run did not restore the generated block'
    assert effect_check(fixture) == 0, 'outputs not current after restoring a block'

    # a run that rewrites the lock file (EN name differs from the header) stores the hash of the rewritten lock
    edit('src/EffectIds.lock', b'\tEffectWipe\tWipe\n', b'\tEffectWipe\tWipeRenamed\n')
    generate()
    assert (fixture / 'src/EffectIds.lock').read_bytes() == (REPO / 'src/EffectIds.lock').read_bytes(), 'lock not rewritten'
    assert effect_check(fixture) == 0, '-Check outdated right after a run that rewrote the lock'
    assert snapshot(fixture, outputs + [FINGERPRINT]) == baseline, 'the lock rewrite changed an output'

    # -Clean -Check reports only and writes nothing
    result = run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-Clean', '-Check', '-Force', check=False)
    assert result.returncode == 0, '-Clean -Check did not report current'
    assert snapshot(fixture, outputs + [FINGERPRINT]) == baseline, '-Clean -Check changed a file'

    if '--full' in sys.argv:
        run(fixture, '-File', 'scripts/Build-Release.ps1', '-SkipFirmware')
        # The local producer may lack the KNX XSDs. Still enforce the schema's
        # 255-character separator Text limit on the fully expanded product.
        product = ET.parse(fixture / 'src/NeoPixel-Dev.debug.xml')
        separators = [element for element in product.iter()
                      if element.tag.split('}')[-1] == 'ParameterSeparator']
        assert separators, 'generated product contains no parameter separators'
        for separator in separators:
            text = separator.get('Text', '')
            assert len(text) <= 255, (
                f"ETS separator {separator.get('Id')} exceeds the 255-character Text limit "
                f"({len(text)}): {text}")
        print(f'Validated the ETS Text limit on {len(separators)} generated separators.')
        assert snapshot(fixture, outputs + [FINGERPRINT]) == baseline, 'Build-Release changed a committed output'
        assert effect_check(fixture) == 0, 'effect outputs not current after Build-Release'
        assert (fixture / 'dependencies.txt').read_bytes() == dependencies, 'dependency snapshot changed'

    # Build-Release -Clean: the effect outputs are committed and stay as they are
    run(fixture, '-File', 'scripts/Build-Release.ps1', '-Clean')
    assert (fixture / 'dependencies.txt').read_bytes() == dependencies, 'cleanup changed dependency snapshot'
    assert snapshot(fixture, outputs + [FINGERPRINT]) == baseline, 'Build-Release -Clean changed an effect output'
    assert effect_check(fixture) == 0, 'effect outputs not current after Build-Release -Clean'

    # Build-EffectParameters -Clean: empties/deletes the outputs and the fingerprint
    run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-Clean', '-Force')
    assert not (fixture / FINGERPRINT).exists(), 'cleanup kept the fingerprint'
    assert not (fixture / 'src/EffectParameterMapping.h').exists()
    assert not list((fixture / 'src').glob('NeoPixel.Effects.*.generated.xml'))
    assert b'var NEO_StudioEffectParameters = {};' in studio.read_bytes(), 'cleanup kept the Studio block'
    assert b'Name="NEO%C%WipeSpeed"' not in (fixture / 'src/NeoPixel.Segment.templ.xml').read_bytes(), 'cleanup kept effect parameters'
    assert b'Name="NEOEM%C%Cue%CUE_NR%WipeSpeed"' not in (fixture / 'src/NeoPixel.Cue.part.xml').read_bytes(), 'cleanup kept cue parameters'
    assert effect_check(fixture) == 1, '-Check reports current after cleanup'
    cleaned = snapshot(fixture, outputs)
    assert cleaned != {p: baseline[p] for p in outputs}, 'cleanup changed nothing'

    run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-Clean', '-Force')
    assert snapshot(fixture, outputs) == cleaned, 'cleanup is not idempotent'

    run(fixture, '-File', 'scripts/Build-EffectParameters.ps1', '-SkipOpenKNXproducer')
    restored = snapshot(fixture, outputs + [FINGERPRINT])
    differ = [p for p in baseline if restored[p] != baseline[p]]
    assert not differ, f'rebuild differs from the committed outputs: {differ}'
    assert effect_check(fixture) == 0, 'effect outputs not current after the rebuild'
    assert (fixture / 'dependencies.txt').read_bytes() == dependencies, 'rebuild changed dependency snapshot'
    print(f'{len(outputs)} committed effect outputs: current, cleaned (idempotent) and restored byte-identical.')
