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

      if (context && typeof context.addInfo === "function") {
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
      if (context && typeof context.addInfo === "function") {
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

// Studio online functions import only visual settings; strip/segment/power configuration stays in ETS.
function NEO_readStudioSettings(device, online, progress, context, source) {
    var segment = toInt(context && context.segment, 0);
    if (segment < 1 || segment > 16) throw new Error("NeoPixel: Ungültiger Segmentkontext.");
    var bytes = [], offset = 0, token = -1, total = 0;
    online.connect();
    try {
        do {
            var response = BASE_invokeFunctionPropertyWrapper(158, 10,
                [32, segment, source, (offset >> 8) & 255, offset & 255], device, online, progress, 10, 85);
            if (!response || response.length < 7 || response[0] !== 0 || response[1] !== 1)
                throw new Error(source === 1 ? "NeoPixel: Kein gültiger Studio-Speicherstand. In Studio erneut speichern; nach einem ETS-Download ist der alte Stand ungültig." : "NeoPixel: Gerät unterstützt den Studio-Vergleich nicht oder ist noch nicht bereit.");
            var nextToken = response[2] * 256 + response[3];
            var nextTotal = response[4] * 256 + response[5];
            if (offset === 0) { token = nextToken; total = nextTotal; }
            if (token !== nextToken || total !== nextTotal || total < 1 || total > 8192 || offset + response.length - 6 > total)
                throw new Error("NeoPixel: Segmentdaten haben sich während der Übertragung geändert. Erneut versuchen.");
            bytes = bytes.concat(response.slice(6));
            offset = bytes.length;
        } while (offset < total);
    } finally { online.disconnect(); }
    var encoded = "";
    for (var i = 0; i < bytes.length; i++) encoded += "%" + ("0" + bytes[i].toString(16)).slice(-2);
    var settings = JSON.parse(decodeURIComponent(encoded));
    if (settings.version !== 1 || settings.segment !== segment || !settings.parameters || !(settings.parameters instanceof Array))
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
        if (!entry || allowed.indexOf(entry.name) < 0 || seen[entry.name]) throw new Error("NeoPixel: Ungültige Studio-Parameterliste.");
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
    return bindings.concat(NEO_studioSceneBindings(device, context, settings));
}

function NEO_importStudioSettings(device, online, progress, context) {
    progress.setText("NeoPixel: Lese gespeicherte Studio-Einstellungen...");
    var settings = NEO_readStudioSettings(device, online, progress, context, 1);
    var bindings = NEO_studioBindings(device, context, settings);
    // Resolve every target before changing anything, so incompatible applications remain untouched.
    for (var i = 0; i < bindings.length; i++) bindings[i].parameter.value = bindings[i].value;
    var reducedEffect = device.getParameterByName("NEO_NEO" + toInt(context.segment, 0) + "NEOEffectTypeNo2D");
    if (reducedEffect) reducedEffect.value = settings.effect;
    progress.setProgress(100);
    progress.setText("Studio-Werte in ETS übernommen. Jetzt die ETS-Applikation in das Gerät laden.");
}

function NEO_compareStudioSettings(device, online, progress, context) {
    progress.setText("NeoPixel: Vergleiche ETS mit aktuellen Gerätewerten...");
    var settings = NEO_readStudioSettings(device, online, progress, context, 0);
    var bindings = NEO_studioBindings(device, context, settings), different = [];
    for (var i = 0; i < bindings.length; i++) {
        var binding = bindings[i], actual = binding.parameter.value, expected = binding.value;
        if (binding.compare === false) continue;
        if (binding.name === "SegmentStartupSeparateWhite") continue;
        var prefix = "NEO_NEO" + toInt(context.segment, 0);
        if (settings.channels === 5 && !Number(device.getParameterByName(prefix + "SegmentStartupSeparateWhite").value)) {
            var legacyWhite = Number(device.getParameterByName(prefix + "SegmentStartupW").value);
            if (binding.name === "SegmentStartupW") actual = Math.floor(legacyWhite / 2);
            if (binding.name === "SegmentStartupCW") actual = legacyWhite - Math.floor(legacyWhite / 2);
        }
        var equal = typeof expected === "number" ? Number(actual) === expected : String(actual).toUpperCase() === String(expected).toUpperCase();
        if (binding.name.indexOf("Color") < 0 && typeof expected === "string") equal = String(actual) === expected;
        if (!equal) different.push(binding.name);
    }
    progress.setProgress(100);
    if (different.length) throw new Error("NeoPixel: ETS und Gerät unterscheiden sich bei " + different.length + " Studio-Wert(en): " + different.join(", "));
    progress.setText("Identisch: Farben, Helligkeit, Effekt und Effektparameter stimmen mit ETS überein.");
}

// BEGIN AUTO-GENERATED: Studio ETS parameter names
var NEO_StudioEffectParameters = {"0":[],"1":["WipeSpeed","WipeDirection","WipeLoop"],"2":["RainbowSpeed","RainbowDelta","RainbowSaturation","RainbowDensity","RainbowMode"],"3":["Pride2015Speed"],"4":["JuggleSpeed","JuggleNumDots","JuggleFadeSpeed","JuggleHueOffset"],"5":["BPMBPM","BPMHue"],"6":["CylonSpeed","CylonHue","CylonEyeSize","CylonFadeAmount","CylonDirection","CylonMode"],"7":["TestPhaseDuration","TestMode"],"8":["FireSpeed","FireCooling","FireSparking","FireReverseDirection","FireBlueFireMode"],"9":["TheaterChaseSpeed","TheaterChaseSpacing","TheaterChaseDotSize","TheaterChaseColorMode","TheaterChaseColorSpeed","TheaterChaseBounce"],"10":["SparkleSpeed","SparkleFadeRate","SparkleSparkleCount","SparkleProbability","SparkleWhiteOnly","SparkleBurstMode","SparkleMode"],"11":["BreathingSpeed","BreathingMinBrightness","BreathingCurve","BreathingRainbowMode","BreathingWaveform"],"12":["StrobeSpeed","StrobeOnRatio","StrobeMinBrightness","StrobeRandomTiming","StrobeRainbowStrobe"],"13":["CometSpeed","CometFadeRate","CometTailLength","CometBounceMode","CometRainbowMode","CometMode"],"14":["NoiseSpeed","NoiseScale","NoiseSaturation","NoiseHueOffset","NoisePalette"],"15":["PaletteSpeed","PalettePalette","PaletteBlend","PaletteSpacing"],"16":["BlitzSpeed","BlitzWidth","BlitzDecay","BlitzHue","BlitzIntensity"],"17":["GradientSpeed","GradientStartHue","GradientEndHue","GradientSaturation"],"18":["KerzeSpeed","KerzeIntensity","KerzeZones"],"19":["LaufschriftSpeed","LaufschriftGap","LaufschriftLoop","LaufschriftText","LaufschriftFont"],"20":["Uhr2DViewMode","Uhr2DShowSeconds","Uhr2DBlinkColon","Uhr2DColourHue","Uhr2DDateMode","Uhr2DDateFormat","Uhr2DDateHue","Uhr2DSwitchSec","Uhr2DScrollSpeed","Uhr2DFont"],"21":["Snake2DSpeed","Snake2DHeadHue","Snake2DBodyMode","Snake2DBodyHue"],"22":["Matrix2DSpeed","Matrix2DDensity","Matrix2DPalette","Matrix2DGlitch"],"23":["Tetris2DSpeed","Tetris2DBgBrightness","Tetris2DGhostPiece","Tetris2DColorMode","Tetris2DFlashLines","Tetris2DAutoPlay"],"24":["TRONSpeed","TRONHue","TRONGridSpacing","TRONGlow"],"25":["StarfieldWarpSpeed","StarfieldWarpDensity","StarfieldWarpColorMode","StarfieldWarpWarpPulse"],"26":["PlasmaNebulaSpeed","PlasmaNebulaSaturation","PlasmaNebulaContrast","PlasmaNebulaPaletteShift"],"27":["UFOSwarmSpeed","UFOSwarmCount","UFOSwarmHue","UFOSwarmBeam"],"28":["GameofLife2DSpeed","GameofLife2DHue","GameofLife2DColorMode","GameofLife2DSpawnDensity"],"29":["DNA2DSpeed","DNA2DHue","DNA2DTwist","DNA2DRungSpacing"],"30":["Aurora2DSpeed","Aurora2DHue","Aurora2DScale","Aurora2DIntensity"],"31":["Lissajous2DSpeed","Lissajous2DHue","Lissajous2DFreqA","Lissajous2DFreqB"],"32":["Metaballs2DSpeed","Metaballs2DHue","Metaballs2DBlobCount","Metaballs2DContrast"]};
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
    function rgb(at) { return '#' + data.slice(at,at+3).map(function(v){return ('0'+v.toString(16)).slice(-2);}).join('').toUpperCase(); }
    function latin(at) {
        var special={164:8364,166:352,168:353,180:381,184:382,188:338,189:339,190:376}, result='';
        for(var i=0;i<14 && data[at+i];i++) result+=String.fromCharCode(special[data[at+i]] || data[at+i]);
        return result;
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
        if (textParameter.parameterRefId && typeof device.getParameterById==='function') {
            for (var textEffectId in NEO_StudioEffectParameters) {
                if (!Object.prototype.hasOwnProperty.call(NEO_StudioEffectParameters,textEffectId) || Number(textEffectId)===0) continue;
                var textSuffix=String(Number(textEffectId)+2); if(textSuffix.length<2)textSuffix='0'+textSuffix;
                var textAlias=null;
                try { textAlias=device.getParameterById(textParameter.parameterRefId.slice(0,-2)+textSuffix); } catch(e) {}
                if(textAlias)bindings.push({parameter:textAlias,name:name+'EffectText',value:latin(at+22),compare:false});
            }
        }
        for (var slot=0;slot<10;slot++) {
            var parameter=add(name+'Slot'+slot,data[at+12+slot]);
            if (parameter.parameterRefId && typeof device.getParameterById==='function') {
                for (var effectId in NEO_StudioEffectParameters) {
                    if (!Object.prototype.hasOwnProperty.call(NEO_StudioEffectParameters,effectId) || Number(effectId)===0) continue;
                    var suffix=String(Number(effectId)+2); if(suffix.length<2)suffix='0'+suffix;
                    var alias=null;
                    try { alias=device.getParameterById(parameter.parameterRefId.slice(0,-2)+suffix); } catch(e) {}
                    if(alias)bindings.push({parameter:alias,name:name+'Slot'+slot,value:data[at+12+slot],compare:false});
                }
            }
        }
    }
    return bindings;
}
