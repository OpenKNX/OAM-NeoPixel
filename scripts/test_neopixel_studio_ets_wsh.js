// Native Windows Active Scripting smoke test (ES3 syntax, ASCII source).
// Run from the project: cscript //nologo scripts\test_neopixel_studio_ets_wsh.js
// Optional first argument: path to a generated NeoPixel.script.js.
var neoSource;
if (typeof NEO_TestSource === 'string') {
    neoSource = NEO_TestSource; // used by the host regression runner
} else {
    var fso = new ActiveXObject('Scripting.FileSystemObject');
    var root = fso.GetParentFolderName(fso.GetParentFolderName(WScript.ScriptFullName));
    var sourcePath = WScript.Arguments.length ? WScript.Arguments(0) : fso.BuildPath(root, 'src\\NeoPixel.script.js');
    var stream = new ActiveXObject('ADODB.Stream');
    stream.Type = 2; stream.Charset = 'utf-8'; stream.Open();
    stream.LoadFromFile(sourcePath); neoSource = stream.ReadText(); stream.Close();
}
// Only the trusted local production script is evaluated; device JSON is never evaluated.
eval(neoSource);

function check(condition, message) { if (!condition) throw new Error(message); }
var unavailable = ['indexOf','lastIndexOf','map','filter','forEach','every','some','reduce','reduceRight'];
for (var i = 0; i < unavailable.length; i++) Array.prototype[unavailable[i]] = undefined;
var JSON = undefined;
Array.isArray = undefined;
String.prototype.trim = undefined;
Object.keys = Object.create = Object.defineProperty = undefined;
Function.prototype.bind = undefined;
Date.now = undefined;

NEO_StudioEffectParameters = {'19':['LaufschriftSpeed','LaufschriftGap','LaufschriftLoop','LaufschriftText','LaufschriftFont']};
var payload = '{"version":1,"segment":1,"channels":5,"effect":19,"scenes":[0],"parameters":[' +
    '{"name":"NEOEffectType","value":19},' +
    '{"name":"SegmentStartupBrightness","value":180},' +
    '{"name":"SegmentStartupColor","value":"#FF0000"},' +
    '{"name":"SegmentStartupW","value":75},' +
    '{"name":"SegmentStartupCW","value":105},' +
    '{"name":"SegmentStartupSeparateWhite","value":1},' +
    '{"name":"SegmentStartupSecondaryColor","value":"#0000FF"},' +
    '{"name":"SegmentStartupSecondaryWW","value":0},' +
    '{"name":"SegmentStartupSecondaryCW","value":70},' +
    '{"name":"LaufschriftSpeed","value":100},' +
    '{"name":"LaufschriftGap","value":1},' +
    '{"name":"LaufschriftLoop","value":1},' +
    '{"name":"LaufschriftText","value":"Gr\\u00fc\\u00dfe \\u20ac"},' +
    '{"name":"LaufschriftFont","value":2}]}';
var fixture = NEO_parseStudioJson(payload), parameters = {};
for (var j = 0; j < fixture.parameters.length; j++) {
    var entry = fixture.parameters[j]; parameters['NEO_NEO1'+entry.name] = {value:entry.value};
}
parameters.NEO_NEO1SceneCount = {value:0};
var device = {getParameterByName:function(name) { return parameters[name] || null; }};
var connected = false, source = -1, pages = 0, context = {segment:1};
var online = {connect:function() {connected=true;}, disconnect:function() {connected=false;}};
var progress = {text:'', setText:function(text) {this.text=text;}, setProgress:function(value) {this.percent=value;}};
function BASE_invokeFunctionPropertyWrapper(object, property, request) {
    check(connected && object===158 && property===10 && request[0]===32, 'Invalid online request');
    source=request[2]; pages++;
    var encoded=unescape(encodeURIComponent(payload)), bytes=[];
    for (var k=0;k<encoded.length;k++) bytes.push(encoded.charCodeAt(k));
    var offset=request[3]*256+request[4];
    return [0,1,0,1,bytes.length>>8,bytes.length&255].concat(bytes.slice(offset,offset+42));
}
NEO_compareStudioSettings(device, online, progress, context);
check(progress.text.indexOf('Identisch')===0 && !connected && pages>1, 'Equal comparison failed');
parameters.NEO_NEO1LaufschriftGap.value=7;
var difference=false;
try { NEO_compareStudioSettings(device, online, progress, context); }
catch (error) { difference=error.message.indexOf('LaufschriftGap: ETS = 7;')>=0; }
check(difference && source===0 && !connected, 'Difference report failed');
check(parameters.NEO_NEO1LaufschriftGap.value===7, 'Comparison changed ETS');
NEO_importStudioSettings(device, online, progress, context);
check(source===1 && !connected && parameters.NEO_NEO1LaufschriftGap.value===1, 'Saved import failed');
check(parameters.NEO_NEO1LaufschriftText.value==='Gr\u00fc\u00dfe \u20ac', 'Unicode text failed');
var rejected=false;
try { NEO_parseStudioJson('{"a":1,}'); } catch (error) { rejected=true; }
check(rejected, 'Invalid JSON was accepted');
if (typeof ActiveXObject !== 'undefined') {
    var dictionary = new ActiveXObject('Scripting.Dictionary');
    check(NEO_hasHostMethod(dictionary, 'Exists'), 'Callable COM method was rejected');
    dictionary.Add('studio', 1);
    check(dictionary.Exists('studio'), 'COM method call failed');
}
WScript.Echo('PASS: ES3 Studio import, comparison, paging, Unicode and JSON rejection' +
    (typeof ScriptEngineMajorVersion === 'function' ? ' (JScript '+ScriptEngineMajorVersion()+'.'+ScriptEngineMinorVersion()+')' : ' (host simulation)'));
