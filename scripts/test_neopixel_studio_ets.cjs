// Run with Node.js: node scripts/test_neopixel_studio_ets.cjs
// Executes the production ETS functions with an in-memory device and paged online transport.
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../src/NeoPixel.script.js'), 'utf8');
const context = vm.createContext({});
vm.runInContext(source, context);
vm.runInContext(`
var settings = {version:1, segment:1, channels:5, effect:19, parameters:[
 {name:'NEOEffectType',value:19}, {name:'SegmentStartupBrightness',value:180},
 {name:'SegmentStartupColor',value:'#123456'}, {name:'SegmentStartupW',value:75},
 {name:'SegmentStartupCW',value:105}, {name:'SegmentStartupSeparateWhite',value:1},
 {name:'SegmentStartupSecondaryColor',value:'#AABBCC'},
 {name:'SegmentStartupSecondaryWW',value:0}, {name:'SegmentStartupSecondaryCW',value:70},
 {name:'LaufschriftSpeed',value:100}, {name:'LaufschriftGap',value:1},
 {name:'LaufschriftLoop',value:1}, {name:'LaufschriftText',value:'Grüße €'}, {name:'LaufschriftFont',value:2}]};
var parameters = {}, connected = false, lastSource=-1, pageCount=0, changingToken=false;
settings.parameters.forEach(function(p) {parameters['NEO_NEO1'+p.name] = {value:p.value};});
parameters.NEO_NEO1NEOEffectTypeNo2D={value:19};
parameters.NEO_NEO1SegmentStart={value:12};
var device={getParameterByName:function(name) {return parameters[name] || null;}};
var online={connect:function(){connected=true;},disconnect:function(){connected=false;}};
var progress={setText:function(text){this.text=text;},setProgress:function(n){this.percent=n;}};
var eventContext={segment:1};
function BASE_invokeFunctionPropertyWrapper(object,property,data) {
 if(object!==158 || property!==10 || data[0]!==32) throw new Error('wrong protocol');
 if(!connected) throw new Error('not connected');
 lastSource=data[2]; pageCount++;
 var encoded=unescape(encodeURIComponent(JSON.stringify(settings))), bytes=[];
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
vm.runInContext('parameters.NEO_NEO1LaufschriftGap.value=7', context);
assert.throws(() => vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)', context), /LaufschriftGap/);
assert.equal(context.parameters.NEO_NEO1LaufschriftGap.value, 7, 'comparison must not mutate ETS');
vm.runInContext('NEO_importStudioSettings(device,online,progress,eventContext)', context);
assert.equal(context.parameters.NEO_NEO1LaufschriftGap.value, 1);
assert.equal(context.parameters.NEO_NEO1LaufschriftText.value, 'Grüße €');
assert.equal(context.parameters.NEO_NEO1SegmentStart.value, 12, 'geometry must stay unchanged');
assert.equal(context.lastSource, 1);
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
NEO_StudioEffectParameters['25'].forEach(function(name) {
    settings.parameters.push({name:name,value:1});
    parameters['NEO_NEO1'+name]={value:0};
});
NEO_importStudioSettings(device,online,progress,eventContext);
`, context);
assert.equal(context.parameters.NEO_NEO1StarfieldWarpColorMode.value, 1, 'numeric ColorMode is not an RGB string');
vm.runInContext(`
settings.scenes=[1];for(var i=0;i<36;i++)settings.scenes.push(0);
settings.scenes[1]=1;settings.scenes[2]=1;settings.scenes[3]=2;settings.scenes[4]=3;
settings.scenes[12]=150;settings.scenes[13]=33;settings.scenes[14]=2;settings.scenes[15]=1;
var text=[71,114,252,223,101];for(var i=0;i<text.length;i++)settings.scenes[23+i]=text[i];
parameters.NEO_NEO1SceneCount={value:0};
['EffectType','PrimaryColor','PrimaryWW','PrimaryCW','SecondaryColor','SecondaryWW','SecondaryCW','Brightness','EffectText'].forEach(function(name){parameters['NEO_NEO1SceneA'+name]={value:0};});
for(var i=0;i<10;i++)parameters['NEO_NEO1SceneASlot'+i]={value:0};
parameters.NEO_NEO1SceneASlot0.parameterRefId='slot0_01';
parameters.NEO_NEO1SceneAEffectText.parameterRefId='text_01';
var sceneAlias={value:0},textAlias={value:''};device.getParameterById=function(id){return id==='slot0_03'?sceneAlias:id==='text_21'?textAlias:null;};
NEO_importStudioSettings(device,online,progress,eventContext);
`,context);
assert.equal(context.parameters.NEO_NEO1SceneCount.value,1);
assert.equal(context.parameters.NEO_NEO1SceneAEffectType.value,1);
assert.equal(context.parameters.NEO_NEO1SceneAPrimaryColor.value,'#010203');
assert.equal(context.parameters.NEO_NEO1SceneAEffectText.value,'Grüße');
assert.equal(context.sceneAlias.value,33,'scene slot aliases must refresh');
assert.equal(context.textAlias.value,'Grüße','scene text aliases must refresh');
vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context);
vm.runInContext('parameters.NEO_NEO1SceneABrightness.value=9',context);
assert.throws(()=>vm.runInContext('NEO_compareStudioSettings(device,online,progress,eventContext)',context),/SceneABrightness/);
assert.equal(context.parameters.NEO_NEO1SceneABrightness.value,9);
console.log('ETS paged readback, import, comparison, UTF-8 and configuration-scope regressions passed');
