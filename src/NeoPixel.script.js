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
function NEO_LedTypeToRGB(input, output, context) {
  var ledType = toInt(input.LedType);

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
    info("NEO_LedTypeToRGB: CUSTOM type, leaving ColourOrder unchanged");
    return;
  }

  var ord = map[ledType];
  // Clamp to valid 0..11 just in case
  if (ord < 0 || ord > 11 || isNaN(ord)) ord = defaultOrder;

  output.RGBColourOrder = ord;
  info("NEO_LedTypeToRGB: LedType " + ledType + " -> ColourOrder " + ord);
}

// Reset Clock GPIO when switching between SPI and 1-Wire LED types
function NEO_ResetClockGPIOOnLedTypeChange(input, output, context) {
  var ledType = toInt(input.LedType);
  
  // Check if this is a SPI LED type
  var isSPI = (ledType === 5 || ledType === 6 || ledType === 21 || 
               ledType === 22 || ledType === 23 || ledType === 24 || ledType === 25);
  
  if (!isSPI) {
    // LED type changed to 1-Wire → Reset all Clock Ports to Dummy (15)
    output.Strip1ClockPort = 15;
    output.Strip2ClockPort = 15;
    output.Strip3ClockPort = 15;
    output.Strip4ClockPort = 15;
    output.Strip5ClockPort = 15;
    output.Strip6ClockPort = 15;
    
    info("NEO_ResetClockGPIOOnLedTypeChange: LED Type " + ledType + " (1-Wire) → Clock Ports reset to 15");
  } else {
    info("NEO_ResetClockGPIOOnLedTypeChange: LED Type " + ledType + " (SPI) → Clock Ports unchanged");
  }
}

// Calculate Start-LED indices for the virtual strip
function NEO_UpdateVirtualStripStartIndices(input, output, context) {
  info("NEO_UpdateVirtualStripStartIndices called");

  // Number of physical strips in use (1..6)
  var numStrips = toInt(input.NumberOfLEDStrips, 6);
  if (numStrips < 1) numStrips = 1;
  if (numStrips > 6) numStrips = 6;

  // ------------------------------------------------------------------
  // Build effective order array pos[] depending on NumberOfLEDStrips
  // pos[i] = physical strip index (1..6) at virtual position i+1
  // ------------------------------------------------------------------
  var pos = [0, 0, 0, 0, 0, 0];

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
    default:
      // Full range – UI uses canonical params directly
      pos[0] = toInt(input.Pos1);
      pos[1] = toInt(input.Pos2);
      pos[2] = toInt(input.Pos3);
      pos[3] = toInt(input.Pos4);
      pos[4] = toInt(input.Pos5);
      pos[5] = toInt(input.Pos6);
      break;
  }

  // Physical strip lengths (per physical index 1..6)
  // Always use canonical length params; they are independent of NumberOfLEDStrips.
  var len = [
    toInt(input.Len1),
    toInt(input.Len2),
    toInt(input.Len3),
    toInt(input.Len4),
    toInt(input.Len5),
    toInt(input.Len6)
  ];

  // ------------------------------------------------------------------
  // Duplicate check → HasDuplicate flag + optional info log
  // ------------------------------------------------------------------
  var seen = {};        // physStrip -> first position
  var hasDup = 0;

  for (var i = 0; i < 6; i++) {
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
  var start = [0, 0, 0, 0, 0, 0];
  var end = [0, 0, 0, 0, 0, 0];
  var current = 1; // 1-based LED index

  for (var i = 0; i < 6; i++) {
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

  output.End1 = end[0];
  output.End2 = end[1];
  output.End3 = end[2];
  output.End4 = end[3];
  output.End5 = end[4];
  output.End6 = end[5];

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
// Reads DEVICE_HW_ID from firmware and writes to Hardware Selection parameter
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
      // Unknown hardware - set to 0 (reserved: "no selection") and let user know
      progress.setText("NeoPixel: Unbekannte Hardware-ID: 0x" + hwId.toString(16).toUpperCase() + " - bitte manuell konfigurieren");
      hwId = 0;  // 0x0000 = reserved value for "no hardware selected"
    }
    
    // Write hardware HW_ID directly to parameter (NOT index!)
    // This makes the parameter stable across ETS versions when new hardware is added
    try {
        // Method 1: ETS standard way using getParameterByName
        var param = device.getParameterByName('NEO_NeoPixelHardwareSelect');
        if (param) {
            //param.value = hwId; // ToDo EC: Later: Write HW_ID directly when supported to prevent index shifts!!
            param.value = hwIndex;
            if (isKnownHardware && hwName) {
                progress.setText("NeoPixel: " + hwName + " (0x" + hwId.toString(16).toUpperCase() + ") ausgewählt");
            } else {
                progress.setText("NeoPixel: Hardware-Auswahl auf HW-ID 0x" + hwId.toString(16).toUpperCase() + " gesetzt");
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
