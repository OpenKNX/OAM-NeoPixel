#!/usr/bin/env python3
"""Local GUI preview: production HTML/JS/CSS, simulated device API, no dependencies."""
import argparse
import copy
import html
import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import re
import threading
import tempfile
import time
from urllib.parse import parse_qs, urlsplit

ROOT = Path(__file__).resolve().parents[1]
NETWORK_ASSETS = ROOT / 'lib/OFM-Network/web/assets'
SCENARIOS = {
    'normal': 'RGBCCT · normal', 'rgb': 'RGB only', 'rgbw': 'RGBW',
    'locked': 'KNX locked', 'off': 'Output off', 'em': 'Effect manager running',
    'empty': 'No segments', 'startup': 'Starting up', 'unconfigured': 'Not programmed in ETS',
    'programming': 'ETS download', 'offline': 'API unavailable',
}


def parameter(index, name, kind, value, low=0, high=255, options=()):
    return dict(id=index, name=name, type=kind, value=value, min=low, max=high, options=list(options))


# Representative fixtures, not an emulation of the C++ effect engine.
EFFECTS = {
    0: ('Solid', []),
    1: ('Wipe', [parameter(0, 'Speed', 'number', 25, 1),
                 parameter(1, 'Direction', 'enum', 0, 0, 5,
                           ['Links nach rechts', 'Rechts nach links', 'Oben nach unten',
                            'Unten nach oben', 'Mitte nach aussen', 'Aussen nach Mitte']),
                 parameter(2, 'Loop', 'bool', 1, 0, 1)]),
    7: ('Channel test', [parameter(0, 'PhaseDuration', 'number', 2, 1, 60), parameter(1, 'Mode', 'enum', 0, 0, 1, ['RGBW','RGBCCT'])]),
    19: ('Scroll Text', [parameter(0, 'Speed', 'number', 100, 1),
                         parameter(1, 'Gap', 'number', 1),
                         parameter(2, 'Loop', 'bool', 1, 0, 1),
                         parameter(3, 'Text', 'text', 'OpenKNX · Grüße', 0, 240),
                         parameter(4, 'Font', 'enum', 0, 0, 2, ['5x7', '4x6', '3x5'])]),
}


def asset(token):
    source = (ROOT / 'src/WebUi/NeoPixelStudioAssets.h').read_text()
    match = re.search(r'R"' + token + r'\((.*?)\)' + token + '"', source, re.S)
    if not match:
        raise RuntimeError('Production asset not found: ' + token)
    return match.group(1)


class Device:
    def __init__(self, scenario, state_file=None):
        self.lock = threading.Lock()
        self.state_file = Path(state_file) if state_file else Path(tempfile.gettempdir()) / 'neopixel-studio-preview-saved.json'
        self.saved = json.loads(self.state_file.read_text()) if self.state_file.exists() else {}
        self.next_id = 0
        self.reset(scenario)

    def reset(self, scenario):
        self.scenario = scenario
        self.pending = None
        channels = {'rgb': 3, 'rgbw': 4}.get(scenario, 5)
        colour = dict(rgb='#449841', ww=40 if channels >= 4 else 0, cw=60 if channels == 5 else 0)
        segment = dict(id=1, locked=scenario == 'locked', power=scenario != 'off',
                       em=scenario == 'em', brightness=160, effect=1,
                       primary=colour, secondary=dict(rgb='#172644', ww=0, cw=0),
                       parameters=copy.deepcopy(EFFECTS[1][1]))
        segment['ownPower'] = segment['power']
        segment['activeScene'] = 0
        scene = dict(id=1, effect=1, brightness=160, primary=copy.deepcopy(colour), secondary=copy.deepcopy(segment['secondary']), slots=[25,0,1,0,0,0,0,0,0,0], text='')
        segment['scenes'] = [scene]
        second = copy.deepcopy(segment)
        second.update(id=2, locked=scenario == 'locked', em=False, effect=0, parameters=[])
        self.state = dict(ready=scenario not in ('startup', 'unconfigured', 'programming'),
                          configured=scenario != 'unconfigured', programming=scenario == 'programming',
                          globalPower=scenario != 'off',
                          completedId=self.next_id, error='',
                          channels=channels,
                          segments=[] if scenario in ('empty', 'startup', 'unconfigured', 'programming') else [segment, second])

        for s in self.state['segments']:
            s['testing'] = False
            s['commissioningPending'] = False
            s['commissioning'] = dict(start=1,end=30,strips=[dict(id=1,length=30,skip=0),dict(id=2,length=30,skip=0)])
        self.test_restore = None
        self.test_until = 0
        self.ets_segments = copy.deepcopy(self.state['segments'])

    def apply(self, command):
        segment = next((s for s in self.state['segments'] if s['id'] == command.get('segment')), None)
        if not segment:
            raise ValueError('Unknown segment')
        action, value = command.get('action'), command.get('value')
        if action == 'batch':
            if not isinstance(value, list) or not 1 <= len(value) <= 32 or any(not isinstance(c,dict) or c.get('segment') != segment['id'] or c.get('action') in ('batch','save','load','restoreEts','commissioning','testStart','testStop') for c in value):
                raise ValueError('Invalid batch')
            original = copy.deepcopy(self.state)
            try:
                for item in value: self.apply(item)
            except (ValueError, TypeError):
                self.state = original
                raise
            return
        if action == 'load':
            return
        if action == 'save' and self.test_restore:
            raise ValueError('Stop the commissioning test before saving')
        if action == 'save':
            if segment['em']:
                raise ValueError('Choose a direct effect before saving; effect manager is running')
            if any(p['type'] == 'text' and len(p['value'].encode('iso8859-15')) > 14 for p in segment['parameters']):
                raise ValueError('ETS supports at most 14 bytes of effect text; shorten the text before saving')
            self.saved[str(segment['id'])] = copy.deepcopy(segment)
            temporary = self.state_file.with_suffix('.tmp')
            temporary.write_text(json.dumps(self.saved))
            temporary.replace(self.state_file)
            return
        if segment['locked']:
            raise ValueError('Segment is locked by KNX')
        if action == 'commissioning':
            if not isinstance(value,dict) or len(value.get('strips',[])) != 2: raise ValueError('Invalid strips')
            if any(type(s.get('length')) is not int or not 0<=s['length']<=16383 or type(s.get('skip')) is not int or not 0<=s['skip']<min(256,max(1,s['length'])) for s in value['strips']): raise ValueError('Invalid LED count or skipped LEDs')
            if not 1<=value['start']<=value['end']<=sum(s['length'] for s in value['strips']): raise ValueError('Invalid segment range')
            for other in self.state['segments']:
                if other['id'] != segment['id'] and other['commissioning']['end'] > sum(s['length'] for s in value['strips']): raise ValueError('Another segment exceeds the new LED count')
                if other['locked']: raise ValueError('A segment is locked by KNX')
            self.stop_test()
            for other in self.state['segments']:
                other['commissioning']['strips'] = copy.deepcopy(value['strips'])
                other['commissioningPending'] = True
                other['commissioningApplied'] = True
            segment['commissioning']=copy.deepcopy(value)
            segment['commissioningPending']=value!=next(s['commissioning'] for s in self.ets_segments if s['id']==segment['id'])
            return
        if action == 'testStop':
            self.stop_test()
            return
        if action == 'testStart':
            if not self.state['globalPower'] or segment['em']: raise ValueError('Switch global power on and stop the effect manager before testing')
            self.stop_test()
            pattern=value['pattern']
            if pattern not in ('red','green','blue','white','warm','cool','chase','cycle') or not 1<=value['level']<=64: raise ValueError('Invalid test')
            self.test_restore=copy.deepcopy(segment); self.test_until=time.monotonic()+60
            segment.update(testing=True,ownPower=True,power=True,brightness=value['level'],effect=1 if pattern=='chase' else 0)
            segment['primary']=dict(rgb={'red':'#ff0000','green':'#00ff00','blue':'#0000ff'}.get(pattern,'#ffffff'),ww=0,cw=0)
            return
        if action == 'restoreEts':
            defaults = next(s for s in self.ets_segments if s['id'] == segment['id'])
            for key in ('effect', 'brightness', 'primary', 'secondary', 'parameters', 'scenes'):
                segment[key] = copy.deepcopy(defaults[key])
            segment['em'] = False
            segment['activeScene'] = 0
            return
        if action == 'power':
            if type(value) is not int or value not in (0,1): raise ValueError('Invalid power value')
            segment['ownPower'] = bool(value)
            segment['power'] = bool(value) and self.state['globalPower']
            return
        if action in ('sceneRecall','sceneStore','sceneAdd','sceneDelete'):
            scenes = segment['scenes']
            scene = next((s for s in scenes if s['id'] == value), None)
            if action == 'sceneAdd':
                if len(scenes) >= 10: raise ValueError('Maximum scene count reached')
            elif not scene: raise ValueError('Unknown scene')
            if action == 'sceneDelete':
                if value != len(scenes): raise ValueError('Only the highest scene can be deleted')
                scenes.pop()
            elif action == 'sceneRecall':
                segment.update(effect=scene['effect'], brightness=scene['brightness'], primary=copy.deepcopy(scene['primary']), secondary=copy.deepcopy(scene['secondary']), activeScene=value)
                segment['parameters'] = copy.deepcopy(EFFECTS[scene['effect']][1])
                for p in segment['parameters']: p['value'] = scene['text'] if p['type']=='text' else scene['slots'][p['id']]
                segment['ownPower'] = scene['brightness'] != 0
                segment['power'] = segment['ownPower'] and self.state['globalPower']
            else:
                text = next((p['value'] for p in segment['parameters'] if p['type']=='text'), '')
                if len(text.encode('iso8859-15')) > 14: raise ValueError('Scenes support at most 14 bytes of text')
                data = dict(id=len(scenes)+1 if action=='sceneAdd' else value, effect=segment['effect'], brightness=segment['brightness'] if segment['ownPower'] else 0, primary=copy.deepcopy(segment['primary']), secondary=copy.deepcopy(segment['secondary']), slots=[0]*10, text=text)
                for p in segment['parameters']:
                    if p['type'] != 'text' and p['id'] < 10: data['slots'][p['id']] = p['value']
                if action=='sceneAdd': scenes.append(data)
                else: scenes[value-1] = data
            return
        if action == 'effect':
            if value not in EFFECTS:
                raise ValueError('Effect unavailable in preview')
            segment.update(effect=value, parameters=copy.deepcopy(EFFECTS[value][1]))
        elif action == 'parameter':
            if command.get('effect') != segment['effect']:
                raise ValueError('Effect changed; refresh the page')
            p = next((p for p in segment['parameters'] if p['id'] == command.get('parameter')), None)
            if not p:
                raise ValueError('Unknown parameter')
            if p['type'] == 'text':
                try:
                    encoded = value.encode('iso8859-15')
                except (AttributeError, UnicodeEncodeError):
                    raise ValueError('Text must fit 240 Latin-15 characters') from None
                if len(encoded) > 240 or '\0' in value:
                    raise ValueError('Text must fit 240 Latin-15 characters')
            elif type(value) is not int or not p['min'] <= value <= p['max']:
                raise ValueError('Parameter outside allowed range')
            p['value'] = value
        elif action in ('primaryChannels', 'secondaryChannels'):
            if not isinstance(value, dict) or not isinstance(value.get('rgb'), str) or not re.fullmatch(r'#[0-9a-fA-F]{6}', value['rgb']):
                raise ValueError('Expected #RRGGBB')
            if any(type(value.get(channel)) is not int or not 0 <= value[channel] <= 255 for channel in ('ww', 'cw')):
                raise ValueError('Expected RGB and white channels')
            if (self.state['channels'] < 4 and value['ww']) or (self.state['channels'] < 5 and value['cw']):
                raise ValueError('White channel unavailable')
            segment['primary' if action.startswith('primary') else 'secondary'] = dict(value)
        elif action in ('primary', 'secondary'):
            if not isinstance(value, str) or not re.fullmatch(r'#[0-9a-fA-F]{6}', value):
                raise ValueError('Expected #RRGGBB')
            segment[action]['rgb'] = value
        elif action in ('brightness', 'primaryWhite', 'primaryCool', 'secondaryWhite', 'secondaryCool'):
            if type(value) is not int or not 0 <= value <= 255:
                raise ValueError('Value must be between 0 and 255')
            if action == 'brightness':
                segment['brightness'] = value
            else:
                cool = action.endswith('Cool')
                if self.state['channels'] < (5 if cool else 4):
                    raise ValueError('White channel unavailable')
                segment['primary' if action.startswith('primary') else 'secondary']['cw' if cool else 'ww'] = value
        else:
            raise ValueError('Unknown control action')
        if action != 'brightness':
            segment['em'] = False

    def stop_test(self):
        if self.test_restore:
            saved=self.test_restore
            for segment in self.state['segments']:
                if segment['id']==saved['id']: segment.update(saved)
            self.test_restore=None

    def complete(self):
        if self.test_restore and time.monotonic()>=self.test_until: self.stop_test()

        if self.pending and time.monotonic() >= self.pending[0]:
            _, request_id, command = self.pending
            try:
                self.apply(command)
                self.state['error'] = ''
            except (ValueError, TypeError, OSError) as error:
                self.state['error'] = str(error)
            self.state['completedId'] = request_id
            self.pending = None


def page(scenario):
    options = ''.join(f'<option value="{key}" {"selected" if key == scenario else ""}>{html.escape(label)}</option>'
                      for key, label in SCENARIOS.items())
    return f'''<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>OpenKNX · local Studio preview</title><link rel="stylesheet" href="/assets/base.css">
<link rel="icon" href="/assets/favicon.svg"></head><body>
<nav><div class="logo"><a href="/neopixel"><img src="/assets/logo.svg" alt="OpenKNX"></a></div>
<div class="menu-container"><a class="menu active" href="/neopixel">NeoPixel Studio</a></div>
<div class="nav-footer"><div class="nav-firm">OAM-NeoPixel · local preview</div>
<div class="nav-info"><span class="prog-dot dot-off"></span>Simulated device</div></div></nav>
<main><details style="font-size:12px;margin-bottom:12px"><summary>Local preview · simulated device</summary>
<form method="get" action="/neopixel" style="margin-top:8px"><label for="preview-scenario">Scenario </label>
<select id="preview-scenario" name="scenario">{options}</select> <button>Load / reset</button></form>
</details>{asset('HTML')}</main></body></html>'''


class Handler(BaseHTTPRequestHandler):
    def send(self, status, body, mime='application/json; charset=utf-8'):
        body = body if isinstance(body, bytes) else body.encode('utf-8')
        self.send_response(status)
        self.send_header('Content-Type', mime)
        self.send_header('Content-Length', str(len(body)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        url = urlsplit(self.path)
        device = self.server.device
        with device.lock:
            if url.path in ('/', '/neopixel', '/neopixel/'):
                scenario = parse_qs(url.query).get('scenario', [device.scenario])[0]
                if scenario not in SCENARIOS:
                    return self.send(400, '{"error":"Unknown preview scenario"}')
                if 'scenario' in parse_qs(url.query):
                    device.reset(scenario)
                return self.send(200, page(scenario), 'text/html; charset=utf-8')
            if url.path == '/neopixel/studio.js':
                return self.send(200, asset('JS'), 'application/javascript; charset=utf-8')
            if url.path in ('/assets/base.css', '/assets/logo.svg', '/assets/favicon.svg'):
                mime = 'text/css; charset=utf-8' if url.path.endswith('.css') else 'image/svg+xml'
                return self.send(200, (NETWORK_ASSETS / Path(url.path).name).read_bytes(), mime)
            if device.scenario == 'offline' and url.path.startswith('/neopixel/api/'):
                return self.send(503, '{"error":"Simulated API outage"}')
            if url.path == '/neopixel/api/effects':
                return self.send(200, json.dumps([dict(id=i, name=e[0], parameters=copy.deepcopy(e[1])) for i, e in EFFECTS.items()]))
            if url.path == '/neopixel/api/state':
                device.complete()
                return self.send(200, json.dumps(device.state))
        self.send(404, '{"error":"Preview route not found"}')

    def do_POST(self):
        if urlsplit(self.path).path != '/neopixel/api/control':
            return self.send(404, '{"error":"Preview route not found"}')
        device = self.server.device
        try:
            length = int(self.headers.get('Content-Length', '0'))
            if not 0 < length <= 4096:
                return self.send(413, '{"error":"Invalid command size"}')
            command = json.loads(self.rfile.read(length))
            if not isinstance(command, dict):
                raise ValueError('Expected object')
        except (ValueError, UnicodeDecodeError):
            return self.send(400, '{"error":"Invalid JSON command"}')
        with device.lock:
            if device.scenario == 'offline':
                return self.send(503, '{"error":"Simulated API outage"}')
            device.complete()
            if device.pending:
                return self.send(409, '{"error":"Another command is pending"}')
            device.next_id += 1
            device.pending = (time.monotonic() + 0.1, device.next_id, command)
            self.send(202, json.dumps(dict(id=device.next_id)))

    def log_message(self, message, *args):
        if self.command != 'GET' or not self.path.startswith('/neopixel/api/'):
            super().log_message(message, *args)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', type=int, default=8765)
    parser.add_argument('--state-file', help='Path for simulated persistent Studio saves')
    parser.add_argument('--scenario', choices=SCENARIOS, default='normal')
    args = parser.parse_args()
    with ThreadingHTTPServer(('127.0.0.1', args.port), Handler) as server:
        server.device = Device(args.scenario, args.state_file)
        print(f'NeoPixel Studio preview: http://127.0.0.1:{server.server_port}/neopixel', flush=True)
        print('Production page assets, simulated API. Ctrl+C to stop.', flush=True)
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            pass


if __name__ == '__main__':
    main()
