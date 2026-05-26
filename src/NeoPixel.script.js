// Colour-order enum values (match NEOColourOrder in NeoPixel.share.xml)
// 3-channel RGB
var CO_GRB = 0, CO_RGB = 1, CO_BRG = 2, CO_RBG = 3, CO_BGR = 4, CO_GBR = 5;
// 4-channel RGBW
var CO_RGBW = 6, CO_GRBW = 7;
// 5-channel RGBCCT
var CO_RGBCCT = 8, CO_GRBCCT = 9, CO_RGBCTW = 10, CO_GRBCTW = 11;

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
  map[9]  = CO_GRBW;   // TM1814 - GRBW
  map[14] = CO_GRBW;   // TM1914 - GRBW
  map[15] = CO_GRBW;   // FW1906 - GRBW
  map[16] = CO_RGBW;   // UCS8904 - RGBW

  // ---- 5-Channel RGBCCT Protocols ----
  map[1]  = CO_GRBCCT; // WS2805 RGBCCT - GRBCCT standard
  map[17] = CO_GRBCCT; // WS2805_RGBCW - GRBCCT
  map[18] = CO_GRBCCT; // SM16825 - GRBCCT
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
  // Clamp to valid 0..11 just in case
  if (ord < 0 || ord > 11 || isNaN(ord)) ord = defaultOrder;

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
    
    // The current ETS parameter model stores the generated hardware index.
    try {
        // Method 1: ETS standard way using getParameterByName
        var param = device.getParameterByName('NEO_NeoPixelHardwareSelect');
        if (param) {
            param.value = hwIndex;
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

  var NEO_sceneLetters = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J"];

  function NEO_hexByte(value) {
    var normalized = Math.max(0, Math.min(255, toInt(value, 0)));
    var text = normalized.toString(16).toUpperCase();
    return text.length < 2 ? "0" + text : text;
  }

  function NEO_segmentSyncErrorMessage(code, segmentId) {
    if (code === 1) {
      return "NeoPixel: Ungültige Segmentanforderung für Segment " + segmentId + ".";
    }
    if (code === 2) {
      return "NeoPixel: Segment " + segmentId + " ist im Gerät nicht vorhanden.";
    }
    if (code === 3) {
      return "NeoPixel: Segmentdaten sind im Gerät derzeit nicht verfügbar.";
    }
    if (code === 4) {
      return "NeoPixel: Segmentdaten konnten vom Gerät nicht exportiert werden.";
    }
    return "NeoPixel: Unbekannter Segmentfehler (Code " + code + ").";
  }

  function NEO_sceneSyncErrorMessage(code, segmentId) {
    return NEO_segmentSyncErrorMessage(code, segmentId);
  }

  function NEO_getSceneSegmentPrefix(context) {
    if (context && context.segmentPrefix) {
      var prefix = String(context.segmentPrefix);
      if (/^NEO_NEO\d+$/.test(prefix)) {
        return prefix;
      }
      if (/^NEO\d+$/.test(prefix)) {
        return "NEO_" + prefix;
      }
      if (/^\d+$/.test(prefix)) {
        return "NEO_NEO" + prefix;
      }
      return prefix;
    }

    if (context && context.segment) {
      return "NEO_NEO" + toInt(context.segment, 0);
    }

    return "";
  }

  function NEO_getSceneSegmentId(context) {
    if (context && context.segment) {
      return toInt(context.segment, 0);
    }

    var prefix = NEO_getSceneSegmentPrefix(context);
    var match = prefix.match(/(\d+)$/);
    return match ? toInt(match[1], 0) : 0;
  }

  function NEO_requireParameter(device, parameterName) {
    var parameter = device.getParameterByName(parameterName);
    if (!parameter) {
      throw new Error("Parameter '" + parameterName + "' nicht gefunden");
    }
    return parameter;
  }

  function NEO_tryParameterRef(device, parameterName, refSuffix) {
    var parameter = device.getParameterByName(parameterName);
    if (!parameter || !parameter.parameterRefId || refSuffix === undefined || refSuffix === null) {
      return null;
    }

    var suffixText = String(toInt(refSuffix, 0));
    if (suffixText.length < 2) {
      suffixText = "0" + suffixText;
    }

    try {
      return device.getParameterById(parameter.parameterRefId.slice(0, -2) + suffixText);
    } catch (error) {
      return null;
    }
  }

  function NEO_sceneEffectRefSuffix(effectType) {
    var normalizedEffectType = toInt(effectType, 0);
    return normalizedEffectType > 0 ? normalizedEffectType + 2 : 1;
  }

  function NEO_updateSceneSlotAliases(device, slotParameterName, slotValue) {
    // ETS stores a separate cached ParameterRef value for each effect-specific
    // scene-slot alias. Refresh all aliases from the imported slot byte so the
    // currently visible effect parameters do not fall back to their XML defaults.
    var maxEffectType = 0;
    for (var effectTypeKey in NEO_SceneEffectDefaults) {
      var numericEffectType = toInt(effectTypeKey, 0);
      if (numericEffectType > maxEffectType) {
        maxEffectType = numericEffectType;
      }
    }

    for (var refSuffix = 3; refSuffix <= NEO_sceneEffectRefSuffix(maxEffectType); refSuffix++) {
      var aliasedSlotParameter = NEO_tryParameterRef(device, slotParameterName, refSuffix);
      if (aliasedSlotParameter) {
        aliasedSlotParameter.value = slotValue;
      }
    }
  }

  function NEO_writeSceneSlotToEts(device, segmentPrefix, sceneIndex, sceneBytes) {
    var sceneLetter = NEO_sceneLetters[sceneIndex];
    if (!sceneLetter) {
      throw new Error("Szenenindex " + (sceneIndex + 1) + " wird nicht unterstützt");
    }

    var scenePrefix = segmentPrefix + "Scene" + sceneLetter;
    var effectType = toInt(sceneBytes[0], 0);
    NEO_requireParameter(device, scenePrefix + "EffectType").value = effectType;
    NEO_requireParameter(device, scenePrefix + "PrimaryColor").value = "#" + NEO_hexByte(sceneBytes[1]) + NEO_hexByte(sceneBytes[2]) + NEO_hexByte(sceneBytes[3]);
    NEO_requireParameter(device, scenePrefix + "PrimaryWW").value = toInt(sceneBytes[4], 0);
    NEO_requireParameter(device, scenePrefix + "PrimaryCW").value = toInt(sceneBytes[5], 0);
    NEO_requireParameter(device, scenePrefix + "SecondaryColor").value = "#" + NEO_hexByte(sceneBytes[6]) + NEO_hexByte(sceneBytes[7]) + NEO_hexByte(sceneBytes[8]);
    NEO_requireParameter(device, scenePrefix + "SecondaryWW").value = toInt(sceneBytes[9], 0);
    NEO_requireParameter(device, scenePrefix + "SecondaryCW").value = toInt(sceneBytes[10], 0);
    NEO_requireParameter(device, scenePrefix + "Brightness").value = toInt(sceneBytes[11], 0);

    var activeSlotRefSuffix = NEO_sceneEffectRefSuffix(effectType);

    for (var slotIndex = 0; slotIndex < 10; slotIndex++) {
      var slotValue = toInt(sceneBytes[12 + slotIndex], 0);
      var slotParameterName = scenePrefix + "Slot" + slotIndex;
      NEO_requireParameter(device, slotParameterName).value = slotValue;

      NEO_updateSceneSlotAliases(device, slotParameterName, slotValue);

      // The scene UI shows aliased ParameterRefs per effect. Update the active
      // alias explicitly so ETS refreshes the currently visible controls.
      var aliasedSlotParameter = NEO_tryParameterRef(device, slotParameterName, activeSlotRefSuffix);
      if (aliasedSlotParameter) {
        aliasedSlotParameter.value = slotValue;
      }
    }
  }

  function NEO_writeSegmentRuntimeToEts(device, segmentPrefix, effectType, brightness, segmentBytes, parameterCount, parameterBytes) {
    NEO_requireParameter(device, segmentPrefix + "SegmentStartupColor").value =
      "#" + NEO_hexByte(segmentBytes[0]) + NEO_hexByte(segmentBytes[1]) + NEO_hexByte(segmentBytes[2]);
    NEO_requireParameter(device, segmentPrefix + "SegmentStartupW").value = toInt(segmentBytes[3], 0);
    NEO_requireParameter(device, segmentPrefix + "SegmentStartupBrightness").value = toInt(brightness, 0);
    NEO_requireParameter(device, segmentPrefix + "NEOEffectType").value = effectType;

    var startupEffectParameter = device.getParameterByName(segmentPrefix + "SegmentStartupEffect");
    if (startupEffectParameter) {
      startupEffectParameter.value = effectType;
    }

    var parameterNames = NEO_SegmentEffectParameterNames[String(effectType)] || [];
    var parameterLimit = Math.min(Math.min(parameterNames.length, toInt(parameterCount, 0)), parameterBytes.length);

    for (var parameterIndex = 0; parameterIndex < parameterLimit; parameterIndex++) {
      NEO_requireParameter(device, segmentPrefix + parameterNames[parameterIndex]).value = toInt(parameterBytes[parameterIndex], 0);
    }
  }

  function NEO_syncSegmentFromDevice(device, online, progress, context) {
    var segmentPrefix = NEO_getSceneSegmentPrefix(context);
    var segmentId = NEO_getSceneSegmentId(context);

    if (!segmentPrefix || segmentId < 1) {
      throw new Error("NeoPixel: Segmentkontext für Segment-Synchronisation fehlt.");
    }

    progress.setText("NeoPixel: Lese Segmentdaten von Segment " + segmentId + "...");
    progress.setProgress(10);
    online.connect();

    var response;
    try {
      response = BASE_invokeFunctionPropertyWrapper(158, 12, [segmentId], device, online, progress, 20, 76);
    } finally {
      online.disconnect();
    }

    if (!response || response.length < 20) {
      throw new Error("NeoPixel: Ungültige Segmentantwort vom Gerät.");
    }

    if (response[0] !== 0) {
      throw new Error(NEO_segmentSyncErrorMessage(response[0], segmentId));
    }

    var payloadVersion = toInt(response[1], 0);
    if (payloadVersion !== 1) {
      throw new Error("NeoPixel: Nicht unterstütztes Segment-Sync-Format (" + payloadVersion + ").");
    }

    var effectType = toInt(response[2], 0);
    var brightness = toInt(response[3], 0);
    var currentSegmentBytes = response.slice(4, 8);
    var parameterCount = Math.min(10, toInt(response[8], 0));
    var currentParameterBytes = response.slice(9, 19);
    var sceneCount = toInt(response[19], 0);
    var expectedLength = 20 + sceneCount * 22;
    if (response.length < expectedLength) {
      throw new Error("NeoPixel: Unvollständige Segmentdaten vom Gerät empfangen.");
    }

    NEO_writeSegmentRuntimeToEts(device, segmentPrefix, effectType, brightness, currentSegmentBytes, parameterCount, currentParameterBytes);
    progress.setText("NeoPixel: Übernehme aktuelle Segmentparameter...");
    progress.setProgress(84);

    NEO_requireParameter(device, segmentPrefix + "SceneCount").value = sceneCount;
    progress.setProgress(88);

    for (var sceneIndex = 0; sceneIndex < sceneCount; sceneIndex++) {
      var offset = 20 + sceneIndex * 22;
      NEO_writeSceneSlotToEts(device, segmentPrefix, sceneIndex, response.slice(offset, offset + 22));
      progress.setText("NeoPixel: Übernehme Szene " + (sceneIndex + 1) + " von " + sceneCount + "...");
      progress.setProgress(88 + Math.floor(((sceneIndex + 1) * 12) / Math.max(sceneCount, 1)));
    }

    progress.setText("NeoPixel: Segment " + segmentId + " wurde in ETS übernommen.");
    progress.setProgress(100);
  }

  function NEO_syncScenesFromDevice(device, online, progress, context) {
    NEO_syncSegmentFromDevice(device, online, progress, context);
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

var NEO_SegmentEffectParameterNames = {
  "1": ["WipeSpeed", "WipeDirection"],
  "2": ["RainbowSpeed", "RainbowDelta"],
  "3": ["RainbowCycleSpeed", "RainbowCycleSaturation", "RainbowCycleDensity", "RainbowCycleReverse"],
  "4": ["Pride2015Speed"],
  "5": ["ConfettiFadeSpeed", "ConfettiSaturation"],
  "6": ["JuggleSpeed", "JuggleNumDots", "JuggleFadeSpeed", "JuggleHueOffset"],
  "7": ["BPMBPM", "BPMHue"],
  "8": ["CylonSpeed", "CylonHue", "CylonEyeSize", "CylonFadeAmount"],
  "9": ["RGBWTestPhaseDuration"],
  "10": ["GarageDoorPhase", "GarageDoorArrowSize", "GarageDoorArrowSpeed", "GarageDoorRunwayGroupSize", "GarageDoorRunwaySpeed", "GarageDoorBreathingSpeed", "GarageDoorOpeningDuration", "GarageDoorRunwayDuration"],
  "11": ["FireSpeed", "FireCooling", "FireSparking", "FireReverseDirection", "FireBlueFireMode"],
  "12": ["TheaterChaseSpeed", "TheaterChaseSpacing", "TheaterChaseDotSize", "TheaterChaseTrailMode"],
  "13": ["TheaterChaseRainbowSpeed", "TheaterChaseRainbowSpacing", "TheaterChaseRainbowDotSize", "TheaterChaseRainbowColorSpeed", "TheaterChaseRainbowTrailMode"],
  "14": ["SinelonSpeed", "SinelonFadeRate", "SinelonDotSize", "SinelonRainbowMode", "SinelonBounceMode"],
  "15": ["TwinkleSpeed", "TwinkleFadeRate", "TwinkleDensity", "TwinkleRainbowMode", "TwinkleVariableBrightness"],
  "16": ["SparkleSpeed", "SparkleFadeRate", "SparkleSparkleCount", "SparkleProbability", "SparkleWhiteOnly", "SparkleBurstMode"],
  "17": ["BreathingSpeed", "BreathingMinBrightness", "BreathingCurve", "BreathingHoldAtPeak", "BreathingRainbowBreathing"],
  "18": ["StrobeSpeed", "StrobeOnRatio", "StrobeMinBrightness", "StrobeRandomTiming", "StrobeRainbowStrobe"],
  "19": ["PulseSpeed", "PulsePulseWidth", "PulseGamma", "PulseSharpPulse", "PulseRainbowPulse"],
  "20": ["CometSpeed", "CometFadeRate", "CometTailLength", "CometBounceMode", "CometRainbowMode"],
  "21": ["MeteorSpeed", "MeteorMeteorSize", "MeteorFrequency", "MeteorRandomColors", "MeteorMultiMeteor"],
  "22": ["NoiseSpeed", "NoiseScale", "NoiseSaturation", "NoiseHueOffset"],
  "23": ["PaletteSpeed", "PalettePalette", "PaletteBlend", "PaletteSpacing"],
  "24": ["BlitzSpeed", "BlitzWidth", "BlitzDecay", "BlitzHue", "BlitzIntensity"],
  "25": ["GradientSpeed", "GradientStartHue", "GradientEndHue", "GradientSaturation"],
  "26": ["RGBCCTTestPhaseDuration"],
  "27": ["Kerzespeed", "Kerzeintensity"],
  "28": ["KerzenMultiSpeed", "KerzenMultiIntensity"]
};

// BEGIN AUTO-GENERATED: Scene Effect Defaults

var NEO_SceneEffectDefaults = {};
// END AUTO-GENERATED: Scene Effect Defaults

// BEGIN AUTO-GENERATED: Scene Effect Defaults Function
function NEO_SetSceneEffectDefaults(input, output, context) { }
// END AUTO-GENERATED: Scene Effect Defaults Function
