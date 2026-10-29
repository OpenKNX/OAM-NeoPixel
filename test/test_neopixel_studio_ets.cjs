// Run with Node.js: node test/test_neopixel_studio_ets.cjs
// Executes the production ETS functions with an in-memory device and paged online transport.
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../src/NeoPixel.script.js'), 'utf8');
const context = vm.createContext({
    JSON: undefined,
    encodeFixtureJson: value => JSON.stringify(value)
});
vm.runInContext(`
['indexOf','lastIndexOf','map','filter','forEach','every','some','reduce','reduceRight'].forEach(function(name) {
    Array.prototype[name] = undefined;
});
JSON = undefined;
Array.isArray = undefined;
String.prototype.trim = undefined;
Object.keys = Object.create = Object.defineProperty = undefined;
Function.prototype.bind = undefined;
Date.now = undefined;
`, context);
vm.runInContext(source, context);
const generatedEffectNames = context.NEO_StudioEffectParameters;
// The tracked ETS script has empty generated maps after -Clean. These names are
// transport fixtures; the build-cycle regression checks generation of the real map.
context.NEO_StudioEffectParameters = {
    '1': ['WipeSpeed', 'WipeDirection', 'WipeLoop'],
    '19': ['LaufschriftSpeed', 'LaufschriftGap', 'LaufschriftLoop', 'LaufschriftText', 'LaufschriftFont'],
    '25': ['StarfieldWarpSpeed', 'StarfieldWarpDensity', 'StarfieldWarpColorMode', 'StarfieldWarpWarpPulse']
};
vm.runInContext(`
var settings = {version:1, segment:1, channels:5, effect:19, parameters:[
 {name:'NEOEffectType',value:19}, {name:'SegmentStartupBrightness',value:180},
 {name:'SegmentStartupColor',value:'#123456'}, {name:'SegmentStartupW',value:75},
 {name:'SegmentStartupCW',value:105}, {name:'SegmentStartupSeparateWhite',value:1},
 {name:'SegmentStartupSecondaryColor',value:'#AABBCC'},
 {name:'SegmentStartupSecondaryWW',value:0}, {name:'SegmentStartupSecondaryCW',value:70},
 {name:'LaufschriftSpeed',value:100}, {name:'LaufschriftGap',value:1},
 {name:'LaufschriftLoop',value:1}, {name:'LaufschriftText',value:'Grüße €'}, {name:'LaufschriftFont',value:2}]};
var parameters = {}, connected = false, lastSource=-1, pageCount=0, changingToken=false, savedSettings=null, savedReads=0;
for(var pi=0;pi<settings.parameters.length;pi++) {var p=settings.parameters[pi];parameters['NEO_NEO1'+p.name] = {value:p.value};}
parameters.NEO_NEO1NEOEffectTypeNo2D={value:19};
parameters.NEO_NEO1SegmentStart={value:12};
var device={getParameterByName:function(name) {return parameters[name] || null;}};
var online={connect:function(){connected=true;},disconnect:function(){connected=false;}};
var progress={texts:[],percentages:[],setText:function(text){this.text=text;this.texts.push(text);},setProgress:function(n){this.percent=n;this.percentages.push(n);}};
var eventContext={segment:1};
function BASE_invokeFunctionPropertyWrapper(object,property,data,device,online,transferProgress) {
 transferProgress.setText("Übertrage Sequenz 1/2");transferProgress.setProgress(10);
 transferProgress.setText("Empfange Sequenz 1/2");transferProgress.setProgress(85);
 if(object!==158 || property!==10 || data[0]!==32) throw new Error('wrong protocol');
 if(!connected) throw new Error('not connected');
 lastSource=data[2]; pageCount++; if(data[2]===1)savedReads++;
 var encoded=unescape(encodeURIComponent(encodeFixtureJson(data[2]===1 && savedSettings ? savedSettings : settings))), bytes=[];
 for(var i=0;i<encoded.length;i++)bytes.push(encoded.charCodeAt(i));
 var offset=data[3]*256+data[4],token=changingToken&&offset>0?2:1;
 return [0,1,0,token,bytes.length>>8,bytes.length&255].concat(bytes.slice(offset,offset+42));
}
`, context);
vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)', context);
assert.match(context.progress.text, /Identisch/);
assert.equal(context.connected, false);
assert.equal(context.lastSource, 0);
assert.ok(context.pageCount > 1);
assert.ok(Array.from(context.progress.texts).some(text=>/Block [2-9][0-9]*\/[0-9]+/.test(text)));
assert.ok(Array.from(context.progress.texts).some(text=>/Bytes/.test(text)));
assert.ok(Array.from(context.progress.texts).every(text=>!/Sequenz/.test(text)),'page-local sequence counters must be suppressed');
assert.ok(Array.from(context.progress.percentages).every((value,i,all)=>i===0||value>=all[i-1]),'readback progress must never go backwards');
assert.equal(context.progress.percent,100);
vm.runInContext('parameters.NEO_NEO1LaufschriftGap.value=7', context);
assert.doesNotThrow(() => vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)', context));
assert.equal(context.parameters.NEO_NEO1LaufschriftGap.value, 7, 'comparison must not mutate ETS');
assert.match(context.progress.text, /Segment 1: 1 abweichende/);
assert.match(context.progress.text, /\n- LaufschriftGap: ETS = 7; Gerät = 1/);
vm.runInContext('parameters.NEO_NEO1SegmentStartupBrightness.value=99', context);
assert.doesNotThrow(() => vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)', context));
assert.match(context.progress.text, /2 abweichende/);
assert.match(context.progress.text, /\n- SegmentStartupBrightness: ETS = 99; Gerät = 180/);
assert.match(context.progress.text, /\n- LaufschriftGap: ETS = 7; Gerät = 1/);
assert.equal(context.parameters.NEO_NEO1SegmentStartupBrightness.value, 99);
vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)', context);
assert.equal(context.parameters.NEO_NEO1LaufschriftGap.value, 1);
assert.equal(context.parameters.NEO_NEO1LaufschriftText.value, 'Grüße €');
assert.equal(context.parameters.NEO_NEO1SegmentStart.value, 12, 'geometry must stay unchanged');
assert.ok(context.savedReads > 0);
assert.match(context.progress.text, /stimmen überein/);
vm.runInContext('progress.texts=[]; progress.percentages=[]; NEO_importStudioSettings(device,online,progress,eventContext)',context);
assert.ok(Array.from(context.progress.percentages).every((value,i,all)=>i===0||value>=all[i-1]),'saved import and subsequent live verification must have monotonic progress');
assert.ok(Array.from(context.progress.texts).some(text=>/Studio-Speicherstand: Block/.test(text)));
assert.ok(Array.from(context.progress.texts).some(text=>/aktuelle Gerätewerte: Block/.test(text)));
assert.match(context.progress.text,/Nur Segment 1/);

vm.runInContext('parameters.NEO_NEO1NEOEffectType.value=0; delete parameters.NEO_NEO1LaufschriftFont', context);
assert.throws(() => vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)', context), /ETS-Parameter fehlt/);
assert.equal(context.parameters.NEO_NEO1NEOEffectType.value, 0, 'incompatible imports must not partially change ETS');
vm.runInContext('parameters.NEO_NEO1LaufschriftFont={value:2}; changingToken=true', context);
assert.throws(() => vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)', context), /Übertragung geändert/);
assert.equal(context.connected, false);
vm.runInContext('changingToken=false; settings.parameters[2].name="SegmentStart"', context);
assert.throws(() => vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)', context), /Ungültige Studio-Parameterliste/);
assert.equal(context.parameters.NEO_NEO1SegmentStart.value, 12);
vm.runInContext(`
settings.effect=25;
settings.parameters=settings.parameters.slice(0,9);
settings.parameters[0].value=25;
settings.parameters[2].name='SegmentStartupColor';
var names=NEO_StudioEffectParameters['25'];
for(var ni=0;ni<names.length;ni++) {var name=names[ni];
    settings.parameters.push({name:name,value:1});
    parameters['NEO_NEO1'+name]={value:0};
}
NEO_importStudioSettings(device,online,progress,eventContext);
`, context);
assert.equal(context.parameters.NEO_NEO1StarfieldWarpColorMode.value, 1, 'numeric ColorMode is not an RGB string');
vm.runInContext(`
settings.scenes=[1];for(var i=0;i<36;i++)settings.scenes.push(0);
settings.scenes[1]=19;settings.scenes[2]=1;settings.scenes[3]=2;settings.scenes[4]=3;
settings.scenes[12]=150;settings.scenes[13]=33;settings.scenes[14]=2;settings.scenes[15]=1;
var text=[71,114,252,223,101];for(var i=0;i<text.length;i++)settings.scenes[23+i]=text[i];
parameters.NEO_NEO1SceneCount={value:0};
var sceneNames=['EffectType','PrimaryColor','PrimaryWW','PrimaryCW','SecondaryColor','SecondaryWW','SecondaryCW','Brightness','EffectText'];
for(var si=0;si<sceneNames.length;si++) parameters['NEO_NEO1SceneA'+sceneNames[si]]={value:0};
for(var i=0;i<10;i++)parameters['NEO_NEO1SceneASlot'+i]={value:0};
parameters.NEO_NEO1SceneASlot0.parameterRefId='slot0_01';
parameters.NEO_NEO1SceneAEffectText.parameterRefId='text_01';
var sceneAlias={value:0},textAlias={value:''};device.getParameterById=function(id){return id==='slot0_21'?sceneAlias:id==='text_21'?textAlias:null;};
NEO_importStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(context.parameters.NEO_NEO1SceneCount.value,1);
assert.equal(context.parameters.NEO_NEO1SceneAEffectType.value,19);
assert.equal(context.NEO_studioRgb(context.parameters.NEO_NEO1SceneAPrimaryColor.value),'#010203');
assert.equal(context.parameters.NEO_NEO1SceneAEffectText.value,'Grüße');
assert.equal(context.sceneAlias.value,33,'scene slot aliases must refresh');
assert.equal(context.textAlias.value,'Grüße','scene text aliases must refresh');
vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context);
vm.runInContext('parameters.NEO_NEO1SceneABrightness.value=9',context);
assert.doesNotThrow(()=>vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context));
assert.equal(context.parameters.NEO_NEO1SceneABrightness.value,9);
assert.match(context.progress.text, /SceneABrightness: ETS = 9; Gerät = 150/);
// The visible effect-specific scene reference can differ from its generic backing slot.
vm.runInContext('parameters.NEO_NEO1SceneABrightness.value=150; sceneAlias.value=77; textAlias.value="visible change"; NEO_compareStudioSettings(device,online,progress,eventContext)',context);
assert.match(context.progress.text,/SceneASlot0: ETS = 77; Gerät = 33/);
assert.match(context.progress.text,/SceneAEffectText: ETS = "visible change"/);
assert.equal(context.parameters.NEO_NEO1SceneASlot0.value,33,'visible alias must be checked even when the generic value matches');
vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)',context);
assert.equal(context.sceneAlias.value,33);
assert.equal(context.textAlias.value,'Grüße');
// A silently rejected active alias is an import failure and triggers rollback.
let sceneSlot=33;
Object.defineProperty(context.sceneAlias,'value',{configurable:true,get:()=>sceneSlot,set:value=>{if(value!==55)sceneSlot=value;}});
vm.runInContext('settings.scenes[13]=55',context);
assert.throws(()=>vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)',context),/SceneASlot0/);
assert.equal(context.parameters.NEO_NEO1SceneASlot0.value,33,'rejected alias must roll back its generic slot');
Object.defineProperty(context.sceneAlias,'value',{configurable:true,writable:true,value:33});
vm.runInContext('settings.scenes[13]=33',context);
// Real ETS TypeColor values use signed ARGB, including black -16777216.
assert.equal(context.NEO_studioRgb(-65536), '#FF0000');
assert.equal(context.NEO_studioRgb(-16777216), '#000000');
assert.equal(context.NEO_studioRgb(0xff0011ff), '#0011FF');
vm.runInContext(`
parameters.NEO_NEO1SceneABrightness.value=150;
parameters.NEO_NEO1SegmentStartupColor.value=0xff123456|0;
parameters.NEO_NEO1SegmentStartupSecondaryColor.value=0xffaabbcc|0;
parameters.NEO_NEO1SceneAPrimaryColor.value=0xff010203|0;
parameters.NEO_NEO1SceneASecondaryColor.value=0xff000000|0;
NEO_compareStudioSettings(device,online,progress,eventContext);
`,context);
assert.match(context.progress.text, /Identisch/);
vm.runInContext(`
parameters.NEO_NEO1SegmentStartupColor.value=-65536;
NEO_importStudioSettings(device,online,progress,eventContext);
NEO_compareStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(context.parameters.NEO_NEO1SegmentStartupColor.value, 0xff123456|0);
assert.match(context.progress.text, /Identisch/);

// Reproduce the screenshots: import saved brightness 106 while live has moved to 255.
vm.runInContext(`
savedSettings=NEO_parseStudioJson(encodeFixtureJson(settings));
savedSettings.parameters[1].value=106;
settings.parameters[1].value=255;
NEO_importStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(context.parameters.NEO_NEO1SegmentStartupBrightness.value,106);
assert.match(context.progress.text, /vollständig.*übernommen und geprüft/);
assert.match(context.progress.text, /SegmentStartupBrightness: ETS = 106; Gerät = 255/);
assert.match(context.progress.text, /erneut in Studio speichern/);
assert.doesNotThrow(()=>vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context));
assert.match(context.progress.text, /SegmentStartupBrightness: ETS = 106; Gerät = 255/);
// A fresh Save followed by Import and Compare must match every exported field.
vm.runInContext(`savedSettings=null; NEO_importStudioSettings(device,online,progress,eventContext);`,context);
assert.match(context.progress.text, /stimmen überein/);
vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context);
assert.match(context.progress.text, /Identisch/);

// Model an ETS setter that silently rejects/clamps brightness: no false success,
// and other project values must be restored if verification fails.
let etsBrightness=106;
Object.defineProperty(context.parameters.NEO_NEO1SegmentStartupBrightness,'value',{
    configurable:true,get:()=>etsBrightness,set:value=>{if(value!==255)etsBrightness=value;}
});
const oldColor=context.parameters.NEO_NEO1SegmentStartupColor.value;
vm.runInContext("settings.parameters[2].value='#ABCDEF'",context);
assert.throws(()=>vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)',context),/nicht übernommen/);
assert.equal(etsBrightness,106);
assert.equal(context.parameters.NEO_NEO1SegmentStartupColor.value,oldColor,'failed import must roll back other ETS values');
console.log('ETS import/compare, ARGB colours, verified brightness, stale saves and rollback regressions passed');

// The legacy ETS runtime has no JSON global. Validate strict data-only parsing,
// escaped text and malformed payload rejection in that same environment.
for (const value of [null, true, false, -123, 1.25e-3, 'Grüße €\n"\\', [], {a:[1,null,{b:'\t'}]}]) {
    const parsed = context.NEO_parseStudioJson(JSON.stringify(value));
    assert.equal(JSON.stringify(parsed), JSON.stringify(value));
}
assert.equal(context.NEO_parseStudioJson('"\\u20ac"'), '€');
for (const invalid of ['', '{"a":1,}', '[1,]', '01', '1e', '1e999', 'true false',
    '{"a":1,"a":2}', '{"__proto__":{}}', '"\\x41"', '"line\nbreak"',
    '(function(){throw 1;})()', '['.repeat(33) + '0' + ']'.repeat(33)]) {
    assert.throws(() => context.NEO_parseStudioJson(invalid), /Studio-JSON/);
}
assert.equal(context.NEO_studioValueText('Grüße "\n\\'), JSON.stringify('Grüße "\n\\').replace('\\n', '\\u000a'));
assert.equal(vm.runInContext('typeof JSON', context), 'undefined');
console.log('Legacy ETS runtime without JSON, Array.indexOf or Array.map passed');
// Run the same ES3-only smoke harness supplied for native Windows cscript.
vm.runInNewContext(fs.readFileSync(path.join(__dirname, 'test_neopixel_studio_ets_wsh.js'), 'utf8'), {
    NEO_TestSource: source,
    WScript: {Echo: text => console.log(text)}
});

// When run on a generated product, exercise every compiled effect's complete
// parameter name list, not only the representative protocol fixtures above.
const effectCatalog = Object.keys(generatedEffectNames).length ? generatedEffectNames : context.NEO_StudioEffectParameters;
context.NEO_StudioEffectParameters = effectCatalog;
let checkedParameters = 0;
for (const effectId of Object.keys(effectCatalog)) {
    const values = [
        {name:'NEOEffectType',value:Number(effectId)}, {name:'SegmentStartupBrightness',value:106},
        {name:'SegmentStartupColor',value:'#0011FF'}, {name:'SegmentStartupW',value:51},
        {name:'SegmentStartupCW',value:93}, {name:'SegmentStartupSeparateWhite',value:1},
        {name:'SegmentStartupSecondaryColor',value:'#020304'},
        {name:'SegmentStartupSecondaryWW',value:29}, {name:'SegmentStartupSecondaryCW',value:71}
    ];
    for (const [i,name] of effectCatalog[effectId].entries()) values.push({name,value:name.includes('Text') ? 'Grüße €' : (i+1)%3});
    context.settings = context.NEO_parseStudioJson(JSON.stringify({version:1,segment:1,channels:5,effect:Number(effectId),parameters:values,scenes:[0]}));
    vm.runInContext(`
    savedSettings=null; parameters={};
    for(var n=0;n<settings.parameters.length;n++) {
        var entry=settings.parameters[n];
        parameters['NEO_NEO1'+entry.name]={value:entry.name==='SegmentStartupColor'||entry.name==='SegmentStartupSecondaryColor' ? -16777216 : typeof entry.value==='string' ? '' : 0};
    }
    parameters.NEO_NEO1SceneCount={value:0};
    var expectedDifferences=NEO_studioDifferences(device,eventContext,settings,NEO_studioBindings(device,eventContext,settings),false);
    NEO_compareStudioSettings(device,online,progress,eventContext);
    for(var di=0;di<expectedDifferences.length;di++) {
        if(progress.text.indexOf(expectedDifferences[di])<0)throw new Error('Missing difference: '+expectedDifferences[di]);
    }
    if(expectedDifferences.length<=2)throw new Error('Coverage fixture must change more than two fields');
    NEO_importStudioSettings(device,online,progress,eventContext);
    NEO_compareStudioSettings(device,online,progress,eventContext);
    `,context);
    assert.match(context.progress.text,/Identisch/,'round-trip failed for effect '+effectId);
    checkedParameters += values.length;
}
console.log('Verified complete ETS round-trip for '+Object.keys(effectCatalog).length+' effects / '+checkedParameters+' visual and effect values');

const reports=[];
context.Log={info:report=>reports.push(report)};
vm.runInContext('parameters.NEO_NEO1SegmentStartupBrightness.value=0; NEO_compareStudioSettings(device,online,progress,eventContext)',context);
assert.match(reports.join("\n"),/SegmentStartupBrightness: ETS = 0; Gerät = 106/);
context.Log={info:()=>{throw new Error('log unavailable');}};
assert.doesNotThrow(()=>vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context));

// Commissioning plans synchronise only the explicitly allowed strip/range fields.
vm.runInContext(`
Log=undefined;
settings.commissioning={start:1,end:30,strips:[{id:1,length:30,skip:0},{id:2,length:30,skip:0}]};
parameters.NEO_NEO1SegmentStart={value:1}; parameters.NEO_NEO1SegmentEnd={value:30};
parameters.NEOSTRIP_NEO1Length={value:30}; parameters.NEOSTRIP_NEO1SkipFirstLEDs={value:0};
parameters.NEOSTRIP_NEO2Length={value:30}; parameters.NEOSTRIP_NEO2SkipFirstLEDs={value:0};
parameters.NEOSTRIP_NEO1GPIO={value:12};
savedSettings=NEO_parseStudioJson(encodeFixtureJson(settings));
savedSettings.commissioning={start:4,end:500,strips:[{id:1,length:1000,skip:3},{id:2,length:0,skip:0}]};
NEO_importStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(context.parameters.NEOSTRIP_NEO1Length.value,1000);
assert.equal(context.parameters.NEOSTRIP_NEO1SkipFirstLEDs.value,3);
assert.equal(context.parameters.NEOSTRIP_NEO2Length.value,0);
assert.equal(context.parameters.NEO_NEO1SegmentStart.value,4);
assert.equal(context.parameters.NEO_NEO1SegmentEnd.value,500);
assert.equal(context.parameters.NEOSTRIP_NEO1GPIO.value,12);
assert.match(context.progress.text,/ETS-Download legt den dauerhaften Startzustand fest/);
vm.runInContext('savedSettings.commissioning.strips[1].skip=1',context);
assert.throws(()=>vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)',context),/Ungültige LED-Anzahl/);
assert.equal(context.parameters.NEOSTRIP_NEO1Length.value,1000,'invalid commissioning import must not change ETS');
console.log('Commissioning import: 16-bit LED counts, skip, segment range, disabled strips and GPIO isolation passed');


// Exercise production Common packetisation too: APDU fragments restart for each
// JSON page, but none of those local 1/N counters may reach the ETS progress UI.
const commonSource=fs.readFileSync(path.join(__dirname,'../lib/OGM-Common/src/Common.script.js'),'utf8');
const wrapperStart=commonSource.indexOf('function BASE_invokeFunctionPropertyWrapper(');
const wrapperSource=commonSource.slice(wrapperStart,commonSource.indexOf('\n}',wrapperStart)+2);
assert.ok(wrapperStart>=0 && wrapperSource.endsWith('}'));
const transport=vm.createContext({payload:JSON.stringify({version:1,segment:1,channels:5,effect:1,parameters:[
    {name:'NEOEffectType',value:1},{name:'SegmentStartupBrightness',value:200},
    {name:'SegmentStartupColor',value:'#123456'},{name:'SegmentStartupW',value:20},
    {name:'SegmentStartupCW',value:30},{name:'SegmentStartupSeparateWhite',value:1},
    {name:'SegmentStartupSecondaryColor',value:'#654321'},
    {name:'SegmentStartupSecondaryWW',value:40},{name:'SegmentStartupSecondaryCW',value:50},
    {name:'WipeSpeed',value:88},{name:'WipeDirection',value:1},{name:'WipeLoop',value:1}
],scenes:[0]})});
vm.runInContext(source+'\n'+wrapperSource,transport);
vm.runInContext(`
NEO_StudioEffectParameters={'1':['WipeSpeed','WipeDirection','WipeLoop']};
var values=NEO_parseStudioJson(payload), parameters={}, fragments=0;
for(var i=0;i<values.parameters.length;i++)parameters['NEO_NEO1'+values.parameters[i].name]={value:0};
parameters.NEO_NEO1SceneCount={value:1};
var device={getParameterByName:function(name){return parameters[name]||null;}};
var progress={texts:[],percents:[],setText:function(s){this.texts.push(s);this.text=s;},setProgress:function(n){this.percents.push(n);}};
var Log={info:function(s){}};
var request=[], response=[], readAt=0;
var connected=false,online={
 connect:function(){connected=true;},disconnect:function(){connected=false;},getMaxApduLength:function(){return 15;},
 invokeFunctionProperty:function(object,property,data){
    if(!connected||object!==158||property!==3)throw new Error('Unexpected Common transport');
    fragments++;
    if(data[0]===0){request=[];return [0,2];}
    if(data[0]===2){request=data.slice(1);return [0,0];}
    if(data[0]===1){
        var encoded=unescape(encodeURIComponent(payload)),bytes=[];
        for(var i=0;i<encoded.length;i++)bytes.push(encoded.charCodeAt(i));
        var offset=request[3]*256+request[4];
        response=[0,1,0,1,bytes.length>>8,bytes.length&255].concat(bytes.slice(offset,offset+42));
        readAt=0;return [0,response.length>>8,response.length&255,255];
    }
    if(readAt>=response.length)return [0];
    var packet=[data[0]].concat(response.slice(readAt,readAt+14));readAt+=14;return packet;
 }
};
NEO_compareStudioSettings(device,online,progress,{segment:1});
`,transport);
assert.ok(transport.fragments>60,'fixture must traverse many real APDU fragments');
assert.ok(Array.from(transport.progress.texts).every(s=>!/Sequenz/.test(s)));
assert.ok(Array.from(transport.progress.percents).every((n,i,all)=>i===0||n>=all[i-1]));
assert.match(transport.progress.text,/12 abweichende/);
assert.match(transport.progress.text,/Nur Segment 1: 12 Werte geprüft/);
assert.equal(transport.connected,false);
console.log('Production Common APDU transport: cumulative blocks/bytes, monotonic progress and complete multi-field comparison passed');

// Change every persistent Studio field together: both colours and white channels,
// all text-effect parameters, all ten scenes, eight strip counts/skips and range.
const coverageFields=[];
context.coverageFields=coverageFields;
vm.runInContext(`
savedSettings=null;parameters={};eventContext.segment=1;
settings={version:1,segment:1,channels:5,effect:19,parameters:[],scenes:[10],commissioning:{start:2,end:100,strips:[]}};
var baseNames=['NEOEffectType','SegmentStartupBrightness','SegmentStartupColor','SegmentStartupW','SegmentStartupCW','SegmentStartupSeparateWhite','SegmentStartupSecondaryColor','SegmentStartupSecondaryWW','SegmentStartupSecondaryCW'];
function fixtureField(fullName,value,compare){
    parameters[fullName]={value:typeof value==='number'?value+1:typeof value==='string'&&value.charAt(0)==='#'?-16777216:'different'};
    if(compare!==false)coverageFields.push(fullName);
}
for(var i=0;i<baseNames.length;i++){
    var name=baseNames[i],value=i===0?19:i===2?'#123456':i===6?'#654321':i===5?1:20+i;
    settings.parameters.push({name:name,value:value});fixtureField('NEO_NEO1'+name,value,name!=='SegmentStartupSeparateWhite');
}
parameters.NEO_NEO1SegmentStartupSeparateWhite.value=1;
var effectNames=NEO_StudioEffectParameters['19'];
for(var i=0;i<effectNames.length;i++){
    var name=effectNames[i],value=name.indexOf('Text')>=0?'Grüße €':i+1;
    settings.parameters.push({name:name,value:value});fixtureField('NEO_NEO1'+name,value);
}
fixtureField('NEO_NEO1SceneCount',10);
var sceneFields=['EffectType','PrimaryColor','PrimaryWW','PrimaryCW','SecondaryColor','SecondaryWW','SecondaryCW','Brightness','EffectText'];
for(var scene=0;scene<10;scene++){
    var name='Scene'+String.fromCharCode(65+scene),bytes=[19,1,2,3,4,5,6,7,8,9,10,111];
    for(var slot=0;slot<10;slot++)bytes.push(12+slot);
    var text='Scene '+String.fromCharCode(65+scene);
    for(var c=0;c<14;c++)bytes.push(c<text.length?text.charCodeAt(c):0);
    settings.scenes=settings.scenes.concat(bytes);
    var values=[19,'#010203',4,5,'#060708',9,10,111,text];
    for(var f=0;f<sceneFields.length;f++)fixtureField('NEO_NEO1'+name+sceneFields[f],values[f]);
    for(var slot=0;slot<10;slot++)fixtureField('NEO_NEO1'+name+'Slot'+slot,12+slot);
}
for(var strip=1;strip<=8;strip++){
    settings.commissioning.strips.push({id:strip,length:30+strip,skip:strip});
    fixtureField('NEOSTRIP_NEO'+strip+'Length',30+strip);fixtureField('NEOSTRIP_NEO'+strip+'SkipFirstLEDs',strip);
}
fixtureField('NEO_NEO1SegmentStart',2);fixtureField('NEO_NEO1SegmentEnd',100);
parameters.NEO_NEO2SegmentStartupBrightness={value:123};
parameters.NEO_NEO1SegmentStartupBehavior={value:2};
Log={info:function(text){}};
NEO_compareStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(coverageFields.length,222);
assert.match(context.progress.text,/222 abweichende Studio-Wert/);
assert.match(context.progress.text,/222 Werte geprüft/);
const fullReports=[];
context.Log={info:line=>fullReports.push(line)};
vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context);
assert.equal(fullReports.filter(line=>line.includes('] - ')).length,222,'ETS log must contain every difference as a separate record');
for(let i=0;i<fullReports.length;i++)assert.ok(fullReports[i].startsWith(`[${i+1}/${fullReports.length}] `));
vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext); NEO_compareStudioSettings(device,online,progress,eventContext)',context);
assert.match(context.progress.text,/Identisch/);
assert.equal(context.parameters.NEO_NEO2SegmentStartupBrightness.value,123,'selected-segment import must not change another segment');
assert.equal(context.parameters.NEO_NEO1SegmentStartupBehavior.value,2,'live On/Off must not alter ETS startup behaviour');
console.log('Complete website audit: 222 simultaneous differences, ten scenes, eight strips, complete per-line log, verified import and selected-segment isolation passed');

// On a generated product, validate the names against the actual merged XML too,
// rather than relying only on in-memory ParameterByName fixtures.
const productXml=path.join(__dirname,'../src/NeoPixel-Dev.debug.xml');
if(fs.existsSync(productXml)){
    const xml=fs.readFileSync(productXml,'utf8');
    const names=new Set(Array.from(xml.matchAll(/<Parameter\b[^>]*\bName="([^"]+)"/g),match=>match[1]));
    for(const name of coverageFields)assert.ok(names.has(name),'Studio field missing from generated ETS product: '+name);
    for(const effectNames of Object.values(generatedEffectNames)){
        for(const name of effectNames)assert.ok(names.has('NEO_NEO1'+name),'Effect readback name missing from generated ETS product: '+name);
    }
    console.log('Every Studio readback binding resolves to a real parameter in the generated ETS product');
}
