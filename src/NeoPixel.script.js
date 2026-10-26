// Colour-order enum values (match NEOColourOrder in NeoPixel.share.xml)
// 3-channel RGB
var CO_GRB = 0, CO_RGB = 1, CO_BRG = 2, CO_RBG = 3, CO_BGR = 4, CO_GBR = 5;
// 4-channel RGBW
var CO_RGBW = 6, CO_GRBW = 7;
// 5-channel RGBCCT
var CO_RGBCCT = 8, CO_GRBCCT = 9, CO_RGBCTW = 10, CO_GRBCTW = 11;
// 12 = WRGB (TM1814-Rahmenformat), 13 = Chip-Vorgabe: der Treiber setzt die native Reihenfolge
var CO_WRGB = 12, CO_CHIPDEFAULT = 13;

// Helper function for safe integer parsing with default value
function toInt(v, def) {
  var x = parseInt(v, 10);
  return isNaN(x) ? (def !== undefined ? def : 0) : x;
}

function NEO_Empty(input, output, context) {
  // no-op for reverse direction
  info("NEO_Empty called with RGBColourOrder: " + input.RGBColourOrder);
}

// Map LedType -> ColourOrder (full order including RGBW/RGBCCT)
function NEO_LedTypeToRGB_Impl(input, output, context, stripKey) {
  var ledType = toInt(input.LedType);
  var numericStripKey = toInt(stripKey, -1);
  var pendingLedTypeResetStrip = toInt(input.PendingLedTypeResetStrip, 0);
  var pendingLedTypeResetLedType = toInt(input.PendingLedTypeResetLedType, 255);
  var hasAnyPendingLedTypeReset = pendingLedTypeResetStrip > 0 &&
    pendingLedTypeResetLedType !== 255;
  var hasPendingLedTypeReset = numericStripKey > 0 &&
    hasAnyPendingLedTypeReset &&
    pendingLedTypeResetStrip === numericStripKey &&
    pendingLedTypeResetLedType === ledType;

  // Default fallback if a type has no explicit mapping
  var defaultOrder = CO_GRB; // most common default

  // Build mapping table [0..99] with default
  var map = [];
  var i;
  for (i = 0; i < 100; i++) map[i] = defaultOrder;

  // ---- 1-Wire RGB Protocols (3-channel) ----
  map[0]  = CO_GRB;    // WS2812B - GRB standard
  map[2]  = CO_RGB;    // WS2811 - RGB order
  map[3]  = CO_GRB;    // WS2813 - GRB standard
  map[4]  = CO_GRB;    // SK6812 (RGB mode) - GRB standard
  map[7]  = CO_GRB;    // WS281x (generic) - GRB standard
  map[10] = CO_GRB;    // WS2812_400kHz - GRB
  map[11] = CO_RGB;    // TM1829 - RGB
  map[12] = CO_RGB;    // UCS8903 - RGB
  map[13] = CO_RGB;    // APA106/PL9823 - RGB

  // ---- SPI Protocols (3-channel) ----
  map[5]  = CO_BGR;    // APA102 - BGR standard
  map[6]  = CO_BGR;    // SK9822 - BGR standard
  map[21] = CO_RGB;    // WS2801 - RGB
  map[22] = CO_GRB;    // LPD8806 - GRB
  map[23] = CO_RGB;    // LPD6803 - RGB
  map[24] = CO_BGR;    // P9813 - BGR
  map[25] = CO_BGR;    // APA102-Clone - BGR

  // ---- 1-Wire RGBW Protocols (4-channel) ----
  map[8]  = CO_GRBW;   // SK6812/WS2814 (RGBW) - GRBW standard
  map[9]  = CO_WRGB;   // TM1814 - Datenblatt-Rahmenformat ist W R G B, nicht GRBW
  map[14] = CO_GRBW;   // TM1914 - GRBW
  map[15] = CO_GRBW;   // FW1906 - GRBW
  map[16] = CO_RGBW;   // UCS8904 - RGBW

  // ---- 5-Channel RGBCCT Protocols ----
  map[1]  = CO_GRBCCT; // WS2805 RGBCCT - GRBCCT standard
  map[17] = CO_GRBCCT; // WS2805_RGBCW - GRBCCT
  map[18] = CO_RGBCTW; // SM16825 - RGB + cool white + warm white
  map[30] = CO_GRBCCT; // SK6812 RGBCCT (5ch) - GRBCCT
  map[31] = CO_GRBCCT; // WS2814 RGBCCT (5ch) - GRBCCT

  // ---- Special/Mono types ----
  map[19] = CO_RGB;    // WS2811_WHITE (mono, keep safe default)
  map[20] = CO_RGB;    // WS281x_WWA (WWA, keep safe default)

  // 99 = CUSTOM -> leave as user-defined; do not override
  if (ledType === 99) {
    info("NEO_LedTypeToRGB: Strip " + stripKey + " CUSTOM type -> keep current ColourOrder");
    return;
  }

  var ord = map[ledType];
  // Keep every ETS colour-order value, including WRGB and Chip-Vorgabe.
  if (ord < 0 || ord > 13 || isNaN(ord)) ord = defaultOrder;

  if (hasPendingLedTypeReset) {
    output.RGBColourOrder = ord;
    info("NEO_LedTypeToRGB: Strip " + stripKey + " pending LEDType reset marker matched for LedType " + ledType + " -> reset ColourOrder to " + ord);
    return;
  }

  if (hasAnyPendingLedTypeReset) {
    info("NEO_LedTypeToRGB: Strip " + stripKey + " pending LEDType reset marker belongs to Strip " + pendingLedTypeResetStrip + " with LedType " + pendingLedTypeResetLedType + " -> keep current ColourOrder");
    return;
  }

  output.RGBColourOrder = ord;
  info("NEO_LedTypeToRGB: Strip " + stripKey + " no pending LEDType reset marker active for LedType " + ledType + " -> reset ColourOrder to " + ord);
}
function NEO_LedTypeToRGB_1(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 1); }
function NEO_LedTypeToRGB_2(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 2); }
function NEO_LedTypeToRGB_3(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 3); }
function NEO_LedTypeToRGB_4(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 4); }
function NEO_LedTypeToRGB_5(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 5); }
function NEO_LedTypeToRGB_6(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 6); }
function NEO_LedTypeToRGB_7(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 7); }
function NEO_LedTypeToRGB_8(input, output, context) { NEO_LedTypeToRGB_Impl(input, output, context, 8); }

// Reset Clock GPIO when switching between SPI and 1-Wire LED types
function NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, stripKey) {
  var ledType = toInt(input.LedType);
  
  // Check if this is a SPI LED type
  var isSPI = (ledType === 5 || ledType === 6 || ledType === 21 || 
               ledType === 22 || ledType === 23 || ledType === 24 || ledType === 25);

  output.PendingLedTypeResetStrip = stripKey;
  output.PendingLedTypeResetLedType = ledType;
  
  if (!isSPI) {
    // LED type changed to 1-Wire → Reset all Clock Ports to Dummy (15)
    output.Strip1ClockPort = 15;
    output.Strip2ClockPort = 15;
    output.Strip3ClockPort = 15;
    output.Strip4ClockPort = 15;
    output.Strip5ClockPort = 15;
    output.Strip6ClockPort = 15;
    output.Strip7ClockPort = 15;
    output.Strip8ClockPort = 15;
    
    info("NEO_ResetClockGPIOOnLedTypeChange: Strip " + stripKey + " LED Type " + ledType + " (1-Wire) -> Clock Ports reset to 15, pending colour-order reset marker updated");
  } else {
    info("NEO_ResetClockGPIOOnLedTypeChange: Strip " + stripKey + " LED Type " + ledType + " (SPI) -> Clock Ports unchanged, pending colour-order reset marker updated");
  }
}
function NEO_ResetClockGPIOOnLedTypeChange_1(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 1); }
function NEO_ResetClockGPIOOnLedTypeChange_2(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 2); }
function NEO_ResetClockGPIOOnLedTypeChange_3(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 3); }
function NEO_ResetClockGPIOOnLedTypeChange_4(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 4); }
function NEO_ResetClockGPIOOnLedTypeChange_5(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 5); }
function NEO_ResetClockGPIOOnLedTypeChange_6(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 6); }
function NEO_ResetClockGPIOOnLedTypeChange_7(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 7); }
function NEO_ResetClockGPIOOnLedTypeChange_8(input, output, context) { NEO_ResetClockGPIOOnLedTypeChange_Impl(input, output, context, 8); }

// Calculate Start-LED indices for the virtual strip
function NEO_UpdateVirtualStripStartIndices(input, output, context) {
  info("NEO_UpdateVirtualStripStartIndices called");

  // Number of physical strips in use (1..8)
  var numStrips = toInt(input.NumberOfLEDStrips, 8);
  if (numStrips < 1) numStrips = 1;
  if (numStrips > 8) numStrips = 8;

  // ------------------------------------------------------------------
  // Build effective order array pos[] depending on NumberOfLEDStrips
  // pos[i] = physical strip index (1..8) at virtual position i+1
  // ------------------------------------------------------------------
  var pos = [0, 0, 0, 0, 0, 0, 0, 0];

  switch (numStrips) {
    case 1:
      pos[0] = toInt(input.Pos1_1);
      break;

    case 2:
      pos[0] = toInt(input.Pos1_2);
      pos[1] = toInt(input.Pos2_2);
      break;

    case 3:
      pos[0] = toInt(input.Pos1_3);
      pos[1] = toInt(input.Pos2_3);
      pos[2] = toInt(input.Pos3_3);
      break;

    case 4:
      pos[0] = toInt(input.Pos1_4);
      pos[1] = toInt(input.Pos2_4);
      pos[2] = toInt(input.Pos3_4);
      pos[3] = toInt(input.Pos4_4);
      break;

    case 5:
      pos[0] = toInt(input.Pos1_5);
      pos[1] = toInt(input.Pos2_5);
      pos[2] = toInt(input.Pos3_5);
      pos[3] = toInt(input.Pos4_5);
      pos[4] = toInt(input.Pos5_5);
      break;

    case 6:
      pos[0] = toInt(input.Pos1_6);
      pos[1] = toInt(input.Pos2_6);
      pos[2] = toInt(input.Pos3_6);
      pos[3] = toInt(input.Pos4_6);
      pos[4] = toInt(input.Pos5_6);
      pos[5] = toInt(input.Pos6_6);
      break;

    case 7:
      pos[0] = toInt(input.Pos1_7);
      pos[1] = toInt(input.Pos2_7);
      pos[2] = toInt(input.Pos3_7);
      pos[3] = toInt(input.Pos4_7);
      pos[4] = toInt(input.Pos5_7);
      pos[5] = toInt(input.Pos6_7);
      pos[6] = toInt(input.Pos7_7);
      break;

    case 8:
    default:
      // Full range – UI uses canonical params directly
      pos[0] = toInt(input.Pos1);
      pos[1] = toInt(input.Pos2);
      pos[2] = toInt(input.Pos3);
      pos[3] = toInt(input.Pos4);
      pos[4] = toInt(input.Pos5);
      pos[5] = toInt(input.Pos6);
      pos[6] = toInt(input.Pos7);
      pos[7] = toInt(input.Pos8);
      break;
  }

  // Physical strip lengths (per physical index 1..8)
  // Always use canonical length params; they are independent of NumberOfLEDStrips.
  var len = [
    toInt(input.Len1),
    toInt(input.Len2),
    toInt(input.Len3),
    toInt(input.Len4),
    toInt(input.Len5),
    toInt(input.Len6),
    toInt(input.Len7),
    toInt(input.Len8)
  ];

  // ------------------------------------------------------------------
  // Duplicate check → HasDuplicate flag + optional info log
  // ------------------------------------------------------------------
  var seen = {};        // physStrip -> first position
  var hasDup = 0;

  for (var i = 0; i < 8; i++) {
    var p = pos[i];
    if (p < 1 || p > numStrips) continue; // ignore invalid / unused

    if (seen[p]) {
      hasDup = 1;
      var firstPos  = seen[p];
      var secondPos = i + 1;

      var msg = "Virtuelle LED-Streifen: physikalischer Streifen " + p +
                " ist mehrfach zugeordnet (Position " + firstPos +
                " und Position " + secondPos + ").";

      if (NEO_hasHostMethod(context, "addInfo")) {
        context.addInfo(msg);
      } else {
        info("WARNUNG: " + msg);
      }
      // we keep going, in case there are more duplicates
    } else {
      seen[p] = i + 1;
    }
  }

  // Set the duplicate flag (0 = no duplicates, 1 = has duplicates)
  output.HasDuplicates = hasDup ? 1 : 0;

  // ------------------------------------------------------------------
  // Compute Start-LED and End-LED indices
  // ------------------------------------------------------------------
  var start = [0, 0, 0, 0, 0, 0, 0, 0];
  var end = [0, 0, 0, 0, 0, 0, 0, 0];
  var current = 1; // 1-based LED index

  for (var i = 0; i < 8; i++) {
    var p2 = pos[i];

    if (p2 < 1 || p2 > numStrips) {
      start[i] = 0;
      end[i] = 0;
      continue;
    }

    var l = len[p2 - 1];
    if (l < 0) l = 0;

    start[i] = current;
    end[i] = (l > 0) ? (current + l - 1) : 0;
    current += l;
  }

  output.Start1 = start[0];
  output.Start2 = start[1];
  output.Start3 = start[2];
  output.Start4 = start[3];
  output.Start5 = start[4];
  output.Start6 = start[5];
  output.Start7 = start[6];
  output.Start8 = start[7];

  output.End1 = end[0];
  output.End2 = end[1];
  output.End3 = end[2];
  output.End4 = end[3];
  output.End5 = end[4];
  output.End6 = end[5];
  output.End7 = end[6];
  output.End8 = end[7];

  info(
    "NumberOfLEDStrips=" + numStrips +
    " orderUsed=" + pos.join(",") +
    " lengths=" + len.join(",") +
    " starts=" + start.join(",") +
    " hasDuplicate=" + hasDup +
    " totalLEDs=" + (current - 1)
  );
}


function NEO_CheckSegmentStartEndRanges(input, output, context) {
  info("NEO_CheckSegmentStartEndRanges called");

  var numSeg = toInt(input.NumberOfSegments);
  if (numSeg < 0) numSeg = 0;
  if (numSeg > 16) numSeg = 16;

  // build arrays 1..16 (index 0 unused)
  var start = [
    0,
    toInt(input.Seg1Start),
    toInt(input.Seg2Start),
    toInt(input.Seg3Start),
    toInt(input.Seg4Start),
    toInt(input.Seg5Start),
    toInt(input.Seg6Start),
    toInt(input.Seg7Start),
    toInt(input.Seg8Start),
    toInt(input.Seg9Start),
    toInt(input.Seg10Start),
    toInt(input.Seg11Start),
    toInt(input.Seg12Start),
    toInt(input.Seg13Start),
    toInt(input.Seg14Start),
    toInt(input.Seg15Start),
    toInt(input.Seg16Start)
  ];

  var end = [
    0,
    toInt(input.Seg1End),
    toInt(input.Seg2End),
    toInt(input.Seg3End),
    toInt(input.Seg4End),
    toInt(input.Seg5End),
    toInt(input.Seg6End),
    toInt(input.Seg7End),
    toInt(input.Seg8End),
    toInt(input.Seg9End),
    toInt(input.Seg10End),
    toInt(input.Seg11End),
    toInt(input.Seg12End),
    toInt(input.Seg13End),
    toInt(input.Seg14End),
    toInt(input.Seg15End),
    toInt(input.Seg16End)
  ];

  var hasError = 0;

  for (var i = 1; i <= numSeg; i++) {
    if (start[i] > 0 && end[i] > 0 && start[i] > end[i]) {
      hasError = 1;
      if (NEO_hasHostMethod(context, "addInfo")) {
        context.addInfo("Segment " + i + ": Start LED (" + start[i] +
                        ") ist größer als Ende LED (" + end[i] + ").");
      } else {
        info("Segment " + i + ": Start LED (" + start[i] +
             ") > Ende LED (" + end[i] + ").");
      }
      // we continue to log all offending segments, but one flag is enough
    }
  }

  output.HasSegmentError = hasError;

  info("SegmentStartEndCheck: numSeg=" + numSeg +
       " hasError=" + hasError);
}

// EventHandler: Hardware auto-detection from firmware
// Called when user clicks "Hardware automatisch erkennen" button
// Reads DEVICE_HW_ID from firmware, maps it to the generated hardware index,
// and writes that index to the Hardware Selection parameter.
function NEO_detectHardware(device, online, progress, context) {
    progress.setText("NeoPixel: Lese Hardware-ID vom Gerät...");
    progress.setProgress(10);
    
    online.connect();
    progress.setProgress(30);
    
    // Function Property ID for hardware detection (to be implemented in firmware)
    // Returns: [0, hwId_high, hwId_low] where hwId = (high << 8) | low
    var data = [0]; // no input data
    var resp = online.invokeFunctionProperty(158, 10, data); // FP 158/10 = NeoPixel hardware detect
    
    online.disconnect();
    progress.setProgress(70);
    
    if (!resp || resp.length < 1 || resp[0] != 0) {
        throw new Error("NeoPixel: Keine Antwort vom Gerät!");
    }
    
    if (resp.length < 3) {
        throw new Error("NeoPixel: Ungültige Antwort vom Gerät!");
    }
    
    // Parse hardware ID from response (16-bit value)
    var hwId = (resp[1] << 8) | resp[2];
    
    progress.setText("NeoPixel: Hardware erkannt - ID: 0x" + hwId.toString(16).toUpperCase());
    progress.setProgress(90);
    
     // Check if Hardware-ID is known (validate using mapping)
    var hwIndex = hardwareIdMap[hwId];
    var isKnownHardware = (hwIndex !== undefined);
    var hwName = null;
    if (isKnownHardware) {
        hwName = hardwareNameMap[hwId];  // Direkt Hardware-Name holen
        progress.setText("NeoPixel: Erkannte Hardware: " + hwName + " (0x" + hwId.toString(16).toUpperCase() + ")");
    } else {
      // Unknown hardware - fall back to the ETS "Bitte wählen..." entry.
      progress.setText("NeoPixel: Unbekannte Hardware-ID: 0x" + hwId.toString(16).toUpperCase() + " - bitte manuell konfigurieren");
      hwIndex = 255;
    }
    
    // The ETS hardware selection stores the unique DEVICE_HW_ID (not a list index), so the
    // selection stays valid regardless of which other hardware exists or how the firmware
    // was built. Write the detected HW-ID directly; reset to 255 ("please select") if unknown.
    var hwSelectValue = isKnownHardware ? hwId : 255;
    try {
        // Method 1: ETS standard way using getParameterByName
        var param = device.getParameterByName('NEO_NeoPixelHardwareSelect');
        if (param) {
            param.value = hwSelectValue;
            if (isKnownHardware && hwName) {
                progress.setText("NeoPixel: " + hwName + " (0x" + hwId.toString(16).toUpperCase() + ") ausgewählt");
            } else {
          progress.setText("NeoPixel: Hardware-Auswahl zurückgesetzt - bitte Hardware manuell wählen");
            }
        } else {
            throw new Error("Parameter 'NEO_NeoPixelHardwareSelect' nicht gefunden");
        }
        
    } catch (e) {
        throw new Error("NeoPixel: Konnte Hardware-Auswahl nicht setzen: " + e.message);
    }
    
    progress.setProgress(100);
}

// ====================================================================
// GPIO Port Allocation and Conflict Detection
// ====================================================================

// BEGIN AUTO-GENERATED: Multi-Hardware GPIO Port Mapping
// Cleaned - Ready for regeneration
// END AUTO-GENERATED: Multi-Hardware GPIO Port Mapping

// BEGIN AUTO-GENERATED: Hardware ID Mapping for EventHandler
// Cleaned - Ready for regeneration
// END AUTO-GENERATED: Hardware ID Mapping for EventHandler

// REAL GPIO CONFLICT DETECTION
// Prüft ob ein Strip denselben GPIO Port wie ein anderer Strip verwendet
// Wenn Strip X denselben Port wie Strip Y hat → BEIDE bekommen Konflikt-Flag

// BEGIN AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection
// Cleaned - Ready for regeneration
// END AUTO-GENERATED: Multi-Hardware GPIO Conflict Detection

// BEGIN AUTO-GENERATED: Network Module Visibility
// Cleaned - Ready for regeneration
// END AUTO-GENERATED: Network Module Visibility

// BEGIN AUTO-GENERATED: Scene Effect Defaults

var NEO_SceneEffectDefaults = {};
// END AUTO-GENERATED: Scene Effect Defaults

// BEGIN AUTO-GENERATED: Scene Effect Defaults Function
function NEO_SetSceneEffectDefaults(input, output, context) { }
// END AUTO-GENERATED: Scene Effect Defaults Function

// Active Scripting exposes callable COM methods as object/unknown as well as function.
function NEO_hasHostMethod(object, name) {
    if (!object) return false;
    try {
        var type = typeof object[name];
        return type === "function" || type === "unknown" ||
            (type === "object" && object[name] !== null);
    } catch (error) { return false; }
}

// ETS parameter scripts can run without ES5's JSON object or Array helpers.
// Parse the device's bounded JSON response without evaluating executable script.
function NEO_parseStudioJson(text) {
    var at = 0, depth = 0;
    function fail() { throw new Error("NeoPixel: Ungültige Studio-JSON-Daten."); }
    function space() { while (at < text.length && /[ \t\r\n]/.test(text.charAt(at))) at++; }
    function string() {
        if (text.charAt(at++) !== '"') fail();
        var result = '';
        while (at < text.length) {
            var ch = text.charAt(at++);
            if (ch === '"') return result;
            if (ch === '\\') {
                ch = text.charAt(at++);
                var escapes = {'"':'"', '\\':'\\', '/':'/', b:'\b', f:'\f', n:'\n', r:'\r', t:'\t'};
                if (ch === 'u') {
                    var hex = text.substr(at, 4);
                    if (!/^[0-9a-fA-F]{4}$/.test(hex)) fail();
                    result += String.fromCharCode(parseInt(hex, 16)); at += 4;
                } else if (Object.prototype.hasOwnProperty.call(escapes, ch)) result += escapes[ch];
                else fail();
            } else {
                if (ch.charCodeAt(0) < 32) fail();
                result += ch;
            }
        }
        fail();
    }
    function value() {
        space();
        if (++depth > 32) fail();
        var ch = text.charAt(at), result, key, match;
        if (ch === '"') result = string();
        else if (ch === '{') {
            at++; result = {}; space();
            if (text.charAt(at) !== '}') {
                while (true) {
                    space(); key = string(); space();
                    if (key === '__proto__' || key === 'constructor' || key === 'prototype' || Object.prototype.hasOwnProperty.call(result, key)) fail();
                    if (text.charAt(at++) !== ':') fail();
                    result[key] = value(); space();
                    if (text.charAt(at) !== ',') break;
                    at++;
                }
            }
            if (text.charAt(at++) !== '}') fail();
        } else if (ch === '[') {
            at++; result = []; space();
            if (text.charAt(at) !== ']') {
                while (true) {
                    result.push(value()); space();
                    if (text.charAt(at) !== ',') break;
                    at++;
                }
            }
            if (text.charAt(at++) !== ']') fail();
        } else if (text.substr(at, 4) === 'true') { result = true; at += 4; }
        else if (text.substr(at, 5) === 'false') { result = false; at += 5; }
        else if (text.substr(at, 4) === 'null') { result = null; at += 4; }
        else {
            match = /^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?/.exec(text.slice(at));
            if (!match) fail();
            result = Number(match[0]); at += match[0].length;
            if (!isFinite(result)) fail();
        }
        depth--; return result;
    }
    var result = value(); space();
    if (at !== text.length) fail();
    return result;
}

function NEO_studioValueText(value) {
    if (typeof value !== 'string') return String(value);
    return '"' + value.replace(/["\\\x00-\x1f]/g, function(ch) {
        if (ch === '"' || ch === '\\') return '\\' + ch;
        return '\\u' + ('0000' + ch.charCodeAt(0).toString(16)).slice(-4);
    }) + '"';
}

function NEO_studioContains(values, value) {
    for (var i = 0; i < values.length; i++) if (values[i] === value) return true;
    return false;
}

// Read one selected segment, including its scenes and the allowed commissioning fields.
// Wrap page-local transport progress: the Common wrapper restarts its sequence counter
// for every 42-byte page, which is not the progress of the complete Studio readback.
function NEO_readStudioSettings(device, online, progress, context, source, progressStart, progressEnd) {
    var segment = toInt(context && context.segment, 0);
    if (segment < 1 || segment > 16) throw new Error("NeoPixel: Ungültiger Segmentkontext.");
    var bytes = [], offset = 0, token = -1, total = 0, page = 1;
    if (progressStart === undefined) progressStart = 0;
    if (progressEnd === undefined) progressEnd = 95;
    var label = "NeoPixel Segment " + segment + (source === 1 ? " · Studio-Speicherstand" : " · aktuelle Gerätewerte");
    function showProgress() {
        var count = total ? Math.ceil(total / 42) : 0;
        progress.setText(label + ": Block " + page + (count ? "/" + count : "/?") +
            " · " + offset + (total ? "/" + total : "") + " Bytes");
        progress.setProgress(progressStart + (total ? Math.floor((progressEnd-progressStart)*offset/total) : 0));
    }
    var transferProgress = {
        setText:function(text) { showProgress(); },
        setProgress:function(value) { } // Page-local percentages must not reset overall progress.
    };
    online.connect();
    try {
        do {
            page = Math.floor(offset / 42) + 1;
            showProgress();
            var response = BASE_invokeFunctionPropertyWrapper(158, 10,
                [32, segment, source, (offset >> 8) & 255, offset & 255], device, online, transferProgress, 0, 100);
            if (!response || response.length < 7 || response[0] !== 0 || response[1] !== 1)
                throw new Error(source === 1 ? "NeoPixel: Kein gültiger Studio-Speicherstand. In Studio erneut speichern; nach einem ETS-Download ist der alte Stand ungültig." : "NeoPixel: Gerät unterstützt den Studio-Vergleich nicht oder ist noch nicht bereit.");
            var nextToken = response[2] * 256 + response[3];
            var nextTotal = response[4] * 256 + response[5];
            if (offset === 0) { token = nextToken; total = nextTotal; }
            if (token !== nextToken || total !== nextTotal || total < 1 || total > 8192 || offset + response.length - 6 > total)
                throw new Error("NeoPixel: Segmentdaten haben sich während der Übertragung geändert. Erneut versuchen.");
            bytes = bytes.concat(response.slice(6));
            offset = bytes.length;
            showProgress();
        } while (offset < total);
    } finally { online.disconnect(); }
    var encoded = "";
    for (var i = 0; i < bytes.length; i++) encoded += "%" + ("0" + bytes[i].toString(16)).slice(-2);
    var settings = NEO_parseStudioJson(decodeURIComponent(encoded));
    if (!settings || typeof settings !== "object" || settings.version !== 1 || settings.segment !== segment || !settings.parameters || !(settings.parameters instanceof Array))
        throw new Error("NeoPixel: Unbekanntes Studio-Format.");
    return settings;
}

function NEO_studioBindings(device, context, settings) {
    var base = ["NEOEffectType", "SegmentStartupBrightness", "SegmentStartupColor", "SegmentStartupW", "SegmentStartupCW",
        "SegmentStartupSeparateWhite", "SegmentStartupSecondaryColor", "SegmentStartupSecondaryWW", "SegmentStartupSecondaryCW"];
    var effectNames = NEO_StudioEffectParameters[String(settings.effect)];
    if (!effectNames) throw new Error("NeoPixel: Effekt ist dieser ETS-Applikation nicht bekannt. Applikation aktualisieren.");
    var allowed = base.concat(effectNames), seen = {}, bindings = [];
    var prefix = "NEO_NEO" + toInt(context.segment, 0);
    if (settings.parameters.length !== allowed.length) throw new Error("NeoPixel: Unvollständige Studio-Parameterliste.");
    for (var i = 0; i < settings.parameters.length; i++) {
        var entry = settings.parameters[i];
        if (!entry || !NEO_studioContains(allowed, entry.name) || seen[entry.name]) throw new Error("NeoPixel: Ungültige Studio-Parameterliste.");
        seen[entry.name] = true;
        if (typeof entry.value !== "number" && typeof entry.value !== "string") throw new Error("NeoPixel: Ungültiger Parameterwert.");
        if (typeof entry.value === "number" && (entry.value < 0 || entry.value > 255 || Math.floor(entry.value) !== entry.value))
            throw new Error("NeoPixel: Parameterwert außerhalb des Wertebereichs.");
        if (entry.name === "SegmentStartupColor" || entry.name === "SegmentStartupSecondaryColor") {
            if (typeof entry.value !== "string" || !/^#[0-9a-fA-F]{6}$/.test(entry.value)) throw new Error("NeoPixel: Ungültige RGB-Farbe.");
        } else if (entry.name.indexOf("Text") >= 0) {
            if (typeof entry.value !== "string") throw new Error("NeoPixel: Ungültiger Effekt-Text.");
        } else if (typeof entry.value !== "number") throw new Error("NeoPixel: Numerischer Parameter erwartet.");
        if (entry.name === "NEOEffectType" && entry.value !== settings.effect) throw new Error("NeoPixel: Widersprüchlicher Effekt.");
        var parameter = device.getParameterByName(prefix + entry.name);
        if (!parameter) throw new Error("NeoPixel: ETS-Parameter fehlt: " + prefix + entry.name + ". Applikation aktualisieren.");
        bindings.push({ parameter: parameter, name: entry.name, value: entry.value });
    }
    var reducedEffect = device.getParameterByName(prefix + "NEOEffectTypeNo2D");
    if (reducedEffect) bindings.push({parameter:reducedEffect, name:"NEOEffectTypeNo2D", value:settings.effect, compare:false});
    return bindings.concat(NEO_studioSceneBindings(device, context, settings), NEO_studioCommissioningBindings(device, context, settings));
}

// Commissioning edits are a plan for the next ETS download, never writes to KNX tables.
function NEO_studioCommissioningBindings(device, context, settings) {
    var plan = settings.commissioning;
    if (plan === undefined) return []; // compatibility with existing saved snapshots
    function integer(value, low, high) { return typeof value === "number" && Math.floor(value) === value && value >= low && value <= high; }
    if (!plan || !(plan.strips instanceof Array) || plan.strips.length < 1 || plan.strips.length > 8) throw new Error("NeoPixel: Ungültiger Inbetriebnahme-Entwurf.");
    var bindings = [], total = 0;
    function add(fullName, name, value) {
        var parameter = device.getParameterByName(fullName);
        if (!parameter) throw new Error("NeoPixel: ETS-Parameter fehlt: " + fullName);
        bindings.push({parameter:parameter, name:name, value:value, commissioning:true});
    }
    for (var i = 0; i < plan.strips.length; i++) {
        var strip = plan.strips[i];
        if (!strip || strip.id !== i+1 || !integer(strip.length,0,16383) || !integer(strip.skip,0,(strip.length ? Math.min(255,strip.length-1) : 0))) throw new Error("NeoPixel: Ungültige LED-Anzahl oder übersprungene LEDs.");
        total += strip.length;
        add("NEOSTRIP_NEO"+strip.id+"Length", "Strip"+strip.id+"Length", strip.length);
        add("NEOSTRIP_NEO"+strip.id+"SkipFirstLEDs", "Strip"+strip.id+"SkipFirstLEDs", strip.skip);
    }
    if (total>65535 || !integer(plan.start,1,Math.min(total,16383)) || !integer(plan.end,plan.start,Math.min(total,16383))) throw new Error("NeoPixel: Segmentbereich liegt außerhalb der Streifen.");
    var prefix="NEO_NEO"+toInt(context.segment,0);
    add(prefix+"SegmentStart","SegmentStart",plan.start); add(prefix+"SegmentEnd","SegmentEnd",plan.end);
    return bindings;
}

function NEO_studioIsColor(name) {
    return name === "SegmentStartupColor" || name === "SegmentStartupSecondaryColor" ||
        /^Scene[A-J](Primary|Secondary)Color$/.test(name);
}

// ETS TypeColor may be a signed ARGB integer (red = -65536), not a #RRGGBB string.
function NEO_studioRgb(value) {
    var text = String(value);
    if (/^#[0-9a-fA-F]{6}$/.test(text)) return text.toUpperCase();
    if (/^#[0-9a-fA-F]{8}$/.test(text)) return '#' + text.slice(-6).toUpperCase();
    if ((typeof value === "number" || /^-?[0-9]+$/.test(text)) && isFinite(Number(value)))
        return '#' + ('000000' + (Number(value) & 0xFFFFFF).toString(16)).slice(-6).toUpperCase();
    return text;
}

function NEO_studioEtsValue(binding) {
    if (!NEO_studioIsColor(binding.name)) return binding.value;
    var current = binding.parameter.value;
    if (typeof current === "number") return parseInt(binding.value.slice(1), 16) | 0xFF000000;
    if (/^-?[0-9]+$/.test(String(current))) return String(parseInt(binding.value.slice(1), 16) | 0xFF000000);
    return binding.value;
}

function NEO_studioDifferences(device, context, settings, bindings, verifyImport) {
    var different = [], prefix = "NEO_NEO" + toInt(context.segment, 0);
    for (var i = 0; i < bindings.length; i++) {
        var binding = bindings[i], actual = binding.parameter.value, expected = binding.value;
        if (verifyImport ? binding.verify === false : binding.compare === false) continue;
        if (binding.name === "SegmentStartupSeparateWhite" && !verifyImport) continue;
        if (!verifyImport && settings.channels === 5 && !Number(device.getParameterByName(prefix + "SegmentStartupSeparateWhite").value)) {
            var legacyWhite = Number(device.getParameterByName(prefix + "SegmentStartupW").value);
            if (binding.name === "SegmentStartupW") actual = Math.floor(legacyWhite / 2);
            if (binding.name === "SegmentStartupCW") actual = legacyWhite - Math.floor(legacyWhite / 2);
        }
        if (NEO_studioIsColor(binding.name)) {
            actual = NEO_studioRgb(actual); expected = NEO_studioRgb(expected);
        }
        var equal = typeof expected === "number" ? Number(actual) === expected : String(actual) === String(expected);
        if (!equal) different.push("- " + binding.name + ": ETS = " + NEO_studioValueText(actual) + (verifyImport ? "; Studio-Speicherstand = " : "; Gerät = ") + NEO_studioValueText(expected));
    }
    return different;
}

function NEO_studioReport(progress, report) {
    progress.setProgress(100);
    progress.setText(report);
    // ETS task summaries can truncate multiline messages. Keep every difference
    // as its own log record, with an index so missing lines are immediately visible.
    if (typeof Log !== "undefined" && NEO_hasHostMethod(Log, "info")) {
        var lines = report.split("\n");
        for (var i = 0; i < lines.length; i++) {
            try { Log.info("[" + (i+1) + "/" + lines.length + "] " + lines[i]); } catch (logError) { }
        }
    }
}

function NEO_studioCheckedCount(bindings) {
    var count = 0;
    for (var i = 0; i < bindings.length; i++) {
        if (bindings[i].compare !== false && bindings[i].name !== "SegmentStartupSeparateWhite") count++;
    }
    return count;
}

function NEO_studioScope(context, bindings, settings) {
    var fields = "Farben, Helligkeit, Effekt und alle aktuellen Effektparameter";
    if (settings.scenes !== undefined) fields += ", Szenen";
    if (settings.commissioning !== undefined) fields += ", LED-Konfiguration einschließlich gemeinsamer Streifendaten";
    var report = "Nur Segment " + toInt(context.segment, 0) + ": " + NEO_studioCheckedCount(bindings) +
        " Werte geprüft (" + fields + "). Live-Ein/Aus und LED-Tests ändern das ETS-Startverhalten nicht. Vollständige Liste: ETS-Protokoll.";
    if (settings.scenes === undefined || settings.commissioning === undefined)
        report += " Fehlende Daten im älteren Speicherstand/Geräteformat: " +
            (settings.scenes === undefined ? "Szenen " : "") + (settings.commissioning === undefined ? "LED-Konfiguration " : "") +
            "wurden nicht geprüft. Firmware aktualisieren und gegebenenfalls erneut speichern.";
    return report;
}

function NEO_importStudioSettings(device, online, progress, context) {
    progress.setText("NeoPixel: Lese gespeicherte Studio-Einstellungen...");
    var settings = NEO_readStudioSettings(device, online, progress, context, 1, 0, 40);
    progress.setText("NeoPixel: Übernehme und prüfe alle Studio-Werte in ETS...");
    progress.setProgress(45);
    var bindings = NEO_studioBindings(device, context, settings), previous = [];
    // Resolve every target before changing anything. Verify what ETS actually accepted,
    // including numeric TypeColor conversion, instead of reporting an unchecked success.
    for (var i = 0; i < bindings.length; i++) previous.push(bindings[i].parameter.value);
    try {
        for (var j = 0; j < bindings.length; j++) bindings[j].parameter.value = NEO_studioEtsValue(bindings[j]);
        var rejected = NEO_studioDifferences(device, context, settings, bindings, true);
        if (rejected.length) throw new Error("NeoPixel: ETS hat diese Studio-Werte nicht übernommen:\n" + rejected.join("\n"));
    } catch (error) {
        for (var undo = bindings.length - 1; undo >= 0; undo--) {
            try { bindings[undo].parameter.value = previous[undo]; } catch (ignored) { }
        }
        throw error;
    }
    var report = "Gespeicherte Studio-Werte in ETS übernommen und geprüft. Jetzt die ETS-Applikation in das Gerät laden.";
    // A saved snapshot and current runtime are different sources. Report changes since
    // Save explicitly, without silently substituting live values for the requested save.
    try {
        progress.setText("Studio-Import geprüft. Vergleiche mit aktuellen Gerätewerten...");
        var live = NEO_readStudioSettings(device, online, progress, context, 0, 50, 95);
        var changed = NEO_studioDifferences(device, context, live, NEO_studioBindings(device, context, live), false);
        if (changed.length) report = "Gespeicherter Studio-Stand vollständig in ETS übernommen und geprüft. Der aktuelle Geräte-Stand weicht davon ab (" + changed.length + " Wert(e)):\n" + changed.join("\n") +
            "\nFür aktuelle Änderungen erneut in Studio speichern und anschließend importieren. Der ETS-Download legt den dauerhaften Startzustand fest.";
        else report = "Studio-Import geprüft: ETS, gespeicherter Studio-Stand und aktuelle Gerätewerte stimmen überein. Jetzt die ETS-Applikation in das Gerät laden.";
    } catch (liveError) {
        report += "\nAktueller Geräte-Stand konnte nicht geprüft werden: " + liveError.message;
    }
    NEO_studioReport(progress, report + "\n" + NEO_studioScope(context, bindings, settings));
}

function NEO_compareStudioSettings(device, online, progress, context) {
    progress.setText("NeoPixel: Vergleiche ETS mit aktuellen Gerätewerten...");
    var settings = NEO_readStudioSettings(device, online, progress, context, 0);
    var bindings = NEO_studioBindings(device, context, settings);
    var different = NEO_studioDifferences(device, context, settings, bindings, false);
    var report = different.length ? "NeoPixel Segment " + toInt(context.segment, 0) + ": " + different.length + " abweichende Studio-Wert(e).\n" + different.join("\n")
        : "Identisch: Alle geprüften Werte stimmen mit ETS überein.";
    // Differences are a successful comparison result, not a parameter script failure.
    NEO_studioReport(progress, report + "\n" + NEO_studioScope(context, bindings, settings));
}

// BEGIN AUTO-GENERATED: Studio ETS parameter names
var NEO_StudioEffectParameters = {};
// END AUTO-GENERATED: Studio ETS parameter names

function NEO_studioSceneBindings(device, context, settings) {
    if (settings.scenes === undefined) return [];
    var data = settings.scenes;
    if (!(data instanceof Array) || data.length < 1 || data[0] > 10 || data.length !== 1 + data[0] * 36) throw new Error("NeoPixel: Ungültige Szenendaten.");
    for (var b = 0; b < data.length; b++) if (typeof data[b] !== 'number' || data[b] < 0 || data[b] > 255 || Math.floor(data[b]) !== data[b]) throw new Error("NeoPixel: Ungültiges Szenenbyte.");
    var prefix = 'NEO_NEO' + toInt(context.segment, 0), bindings = [];
    function add(name, value, optional) {
        var parameter = device.getParameterByName(prefix + name);
        if (!parameter) { if (optional) return null; throw new Error('NeoPixel: ETS-Szenenparameter fehlt: ' + prefix + name); }
        bindings.push({parameter:parameter, name:name, value:value, compare:!optional});
        return parameter;
    }
    function rgb(at) {
        var result = '#';
        for (var i = 0; i < 3; i++) result += ('0' + data[at+i].toString(16)).slice(-2);
        return result.toUpperCase();
    }
    function latin(at) {
        var special={164:8364,166:352,168:353,180:381,184:382,188:338,189:339,190:376}, result='';
        for(var i=0;i<14 && data[at+i];i++) result+=String.fromCharCode(special[data[at+i]] || data[at+i]);
        return result;
    }
    // Generic scene slots and their visible effect-specific ParameterRefs can
    // contain different project values. Compare the visible alias; verify every
    // value written during import, including its generic backing reference.
    function activeAlias(parameter, value, effect) {
        if (!parameter.parameterRefId || !NEO_hasHostMethod(device, 'getParameterById') || effect >= 200) return;
        var suffix=String(effect+2); if(suffix.length<2)suffix='0'+suffix;
        var alias=null;
        try { alias=device.getParameterById(parameter.parameterRefId.slice(0,-2)+suffix); } catch(e) { }
        if(alias) {
            var original=bindings[bindings.length-1];
            original.compare=false;
            bindings.push({parameter:alias,name:original.name,value:value});
        }
    }
    add('SceneCount', data[0]);
    for (var scene=0;scene<data[0];scene++) {
        var at=1+scene*36, name='Scene'+String.fromCharCode(65+scene), effect=data[at];
        if (!NEO_StudioEffectParameters[String(effect)] && !(effect>=200 && effect<=215)) throw new Error('NeoPixel: Unbekannter Szeneneffekt.');
        add(name+'EffectType',effect); add(name+'EffectTypeNo2D',effect,true);
        add(name+'PrimaryColor',rgb(at+1)); add(name+'PrimaryWW',data[at+4]); add(name+'PrimaryCW',data[at+5]);
        add(name+'SecondaryColor',rgb(at+6)); add(name+'SecondaryWW',data[at+9]); add(name+'SecondaryCW',data[at+10]);
        add(name+'Brightness',data[at+11]);
        var textParameter=add(name+'EffectText',latin(at+22));
        activeAlias(textParameter,latin(at+22),effect);
        for (var slot=0;slot<10;slot++) {
            var parameter=add(name+'Slot'+slot,data[at+12+slot]);
            activeAlias(parameter,data[at+12+slot],effect);
        }
    }
    return bindings;
}
