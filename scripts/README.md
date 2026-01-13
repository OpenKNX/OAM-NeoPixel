# Dynamic Multi-Hardware GPIO Template Generator

## Overview

A sophisticated build-time code generation system that creates ETS XML templates with **hardware-specific GPIO configurations** for **multiple target boards simultaneously**. This system generates:

- **Hardware-specific ParameterTypes** (8 variants with different GPIO enumerations)
- **96 flat ParameterCalculations** for GPIO port mapping (8 hardware × 6 strips × 2 GPIO types)
- **JavaScript-based conflict detection** (4-way: Data-Data, Clock-Clock, Data-Clock, Clock-Data)
- **Automatic hardware-change reset** to prevent invalid GPIO selections

## Device Hardware ID Structure

Each hardware variant is identified by a unique `DEVICE_HW_ID` that encodes the device family, board type, and variant in a hierarchical 16-bit structure:

```
Bits 15-12: Main Group (4 bits = 16 main groups)
Bits 11-8:  Subgroup   (4 bits = 16 subgroups)
Bits 7-0:   Variant    (8 bits = 256 variants)

Format: 0xHUVV
        H = Main Group
        U = Subgroup
        VV = Variant
Example: 0x1201 = Main Group 1, Subgroup 2, Variant 1
```

### Device ID Hierarchy

```
Main Group 0x1 = OpenKNX (0x1000-0x1FFF)
├─ 0x10 = REG1  (0x1000-0x10FF) → 256 variants
├─ 0x11 = REG2  (0x1100-0x11FF) → 256 variants
├─ 0x12 = UP1   (0x1200-0x12FF) → 256 variants
├─ 0x13 = XIAO  (0x1300-0x13FF) → 256 variants
└─ ... to 0x1F (16 subgroups)

Main Group 0x2 = OpenKNX-Ready (0x2000-0x2FFF)
├─ 0x20 = Partner A
├─ 0x21 = Partner B
└─ ...

Main Group 0x3 = Community Devices (0x3000-0x3FFF)
├─ 0x30 = Device A
├─ 0x31 = Device B
└─ ...
```

### XIAO Platform Examples (0x13XX)

| Device HW ID | Decimal | Device Name                    | Platform  |
|--------------|---------|--------------------------------|-----------|
| `0x1300`     | 4864    | OpenKNXiao KNeoPix RP2040 V1   | RP2040    |
| `0x1301`     | 4865    | OpenKNXiao Mini RP2040 V1      | RP2040    |
| `0x1310`     | 4880    | OpenKNXiao KNeoPix RP2350 V1.4 | RP2350    |
| `0x1320`     | 4896    | OpenKNXiao KNeoPix ESP32S3 V1  | ESP32-S3  |
| `0x1321`     | 4897    | OpenKNXiao Mini ESP32S3 V1     | ESP32-S3  |
| `0x1330`     | 4912    | OpenKNXiao KNeoPix ESP32C6 V1  | ESP32-C6  |
| `0x1350`     | 4944    | OpenKNXiao KNeoPix ESP32C3 V1  | ESP32-C3  |
| `0x1351`     | 4945    | OpenKNXiao Mini ESP32C3 V1     | ESP32-C3  |

**Usage in Code**:
```cpp
// Defined in lib/OGM-HardwareConfig/include/HardwareConfig/OpenKNX/Xiao.h
#ifdef OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
    #define DEVICE_HW_ID 0x1300
    #define DEVICE_ID PREFIX_ID "V10-KNeoPix-2040"
    #define DEVICE_NAME PREFIX_NAME "V1.0 KNeoPix RP2040"
#endif
```

This ID is extracted during build-time via C++ preprocessor and used to:
- **Map ETS parameters** to correct hardware-specific GPIO enumerations
- **Enable hardware-specific features** in generated XML templates
- **Detect hardware changes** and reset invalid GPIO configurations

## Problem Statement

Multi-hardware embedded systems with static XML parameter definitions face critical challenges:

1. **Hardware Diversity**: Board A has 7 GPIO ports, Board B has 9, Board C has 5 → Static ETS XML can't adapt
2. **GPIO Conflicts**: Strip 1 uses GPIO 6, Strip 2 also selects GPIO 6 → Hardware collision
3. **Hardware Switch Issues**: User switches from RP2040 (has D3) to ESP32S3 (no D3) → Invalid configuration persists
4. **SPI vs Serial**: APA102 needs 2 GPIOs (Clock + Data), WS2812B needs 1 → Different port occupation rules

## Solution Architecture

This system solves these problems through **multi-phase code generation**:

### Phase 1: Hardware Configuration Parsing
- Reads `platformio.hardware.ini` sections (e.g., `[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1]`)
- Extracts `-DNEOPIXEL_HW_PORT_N_GPIO` and `-DNEOPIXEL_HW_PORT_N_LABEL` build flags
- Supports 1-9 GPIO ports per hardware variant

### Phase 2: C++ Preprocessor Extraction
- Uses actual compiler with hardware-specific `-D` flags
- Runs preprocessor-only compilation (`-E -dM`)
- Extracts resolved GPIO numbers (e.g., `OKNXHW_OPENKNXIAO_D3` → `5`)

### Phase 3: ETS XML Generation
- **8 ParameterTypes** with hardware-specific GPIO enumerations (PT-NeoPixelGPIOPortHW0..HW7)
- **96 ParameterCalculations** (no Choose-blocks - OpenKNXproducer limitation workaround)
- **Conflict Detection** ParameterCalculation with 4-way collision checking
- **Hardware Change Reset** ParameterCalculation (resets 12 ports to dummy value 15)

## Architecture

### High-Level Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                    platformio.hardware.ini                          │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │ [neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1]                │  │
│  │ build_flags = -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3 │  │
│  │               -DNEOPIXEL_HW_PORT_1_LABEL="D3"                 │  │
│  └───────────────────────────────────────────────────────────────┘  │
└──────────────────────────┬──────────────────────────────────────────┘
                           │
                           ▼
         ┌─────────────────────────────────────┐
         │   PowerShell INI Parser (Regex)     │
         │ • Pattern: \[neopixel_oknxhw_.*\]   │
         │ • Extract: Section → Content        │
         │ • Parse: build_flags → -D array     │
         └──────────┬──────────────────────────┘
                    │
                    ▼
    ┌───────────────────────────────────────────────┐
    │  Build Flags Extraction (per Hardware)        │
    │  ["-DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1",    │
    │   "-DNEOPIXEL_HW_PORT_1_GPIO=...",            │
    │   "-DNEOPIXEL_HW_PORT_1_LABEL=\"D3\""]        │
    └───────────────┬───────────────────────────────┘
                    │
                    ▼
         ┌──────────────────────────────────────┐
         │  C++ Preprocessor Invocation (g++)   │
         │  g++ -E -P -dM \                      │
         │    -Ilib/OGM-HardwareConfig/include \ │
         │    -DOKNXHW_OPENKNXIAO_... \          │
         │    -DNEOPIXEL_HW_PORT_1_GPIO=... \    │
         │    < extract_template.cpp             │
         └──────────┬───────────────────────────┘
                    │
                    ▼
    ┌────────────────────────────────────────────────┐
    │  Preprocessor Output (Macro Resolution)        │
    │  #define OKNXHW_OPENKNXIAO_D3 5                │
    │  #define NEOPIXEL_HW_PORT_1_GPIO 5             │
    │  #define NEOPIXEL_HW_PORT_1_LABEL "D3"         │
    │  #define HW_DEVICE_ID_BIT 0x1001               │
    └──────────┬─────────────────────────────────────┘
               │
               ▼
    ┌──────────────────────────────────────────┐
    │  PowerShell Parser (Key-Value Pairs)     │
    │  Extract: #define X Y → [X] = Y          │
    │  Filter: NEOPIXEL_HW_*, HW_*             │
    └──────────┬───────────────────────────────┘
               │
               ▼
    ┌─────────────────────────────────────────────────┐
    │  Hardware Config Object (per Hardware)          │
    │  @{                                             │
    │    Name = "neopixel_oknxhw_..._RP2040_V1"      │
    │    DeviceIdBit = 4097 (0x1001)                 │
    │    GPIOPorts = @(                              │
    │      @{ Index=1, Label="D3", GPIO=5 },        │
    │      @{ Index=2, Label="D4", GPIO=6 }         │
    │    )                                           │
    │  }                                             │
    └──────────┬──────────────────────────────────────┘
               │ (× 8 Hardware Variants)
               ▼
    ┌──────────────────────────────────────────────┐
    │  ETS XML Generation Engine                   │
    │  • 8× ParameterTypes (GPIO Enums)            │
    │  • 96× ParameterCalculations (Copy Logic)    │
    │  • Conflict Detection JavaScript             │
    │  • Hardware Change Reset Logic               │
    └──────────┬───────────────────────────────────┘
               │
               ▼
    ┌────────────────────────────────────────────────┐
    │  Generated Files                               │
    │  • src/NeoPixel.Strip.templ.xml (Template)     │
    │  • src/NeoPixel.share.xml (Shared Params)      │
    │  • src/NeoPixel.script.js (JavaScript Logic)   │
    └────────────────────────────────────────────────┘
```

### Component Architecture

#### 1. INI Parser (platformio.hardware.ini → Build Flags)

**Technology**: PowerShell Regex Pattern Matching

**Input**: Multi-section INI file with PlatformIO-style syntax

**Process**:
```powershell
# Step 1: Find all sections matching feature pattern
$sectionPattern = "\[${featureName}_oknxhw_([^\]]+)\]"
$matches = [regex]::Matches($iniContent, $sectionPattern)

# Step 2: For each section, extract build_flags block
$sectionContent = Extract-Section-Between-Brackets
$buildFlagsText = $sectionContent -match "build_flags\s*=(.*?)(?=\n\S|\z)"

# Step 3: Parse -D flags using regex
$defineMatches = [regex]::Matches($buildFlagsText, "-D([^\s]+)")
# Result: ["-DOKNXHW_BOARD", "-DNEOPIXEL_HW_PORT_1_GPIO=..."]
```

**Output**: Array of hardware configs with extracted `-D` flags

**Key Features**:
- **Case-insensitive** section matching (`neopixel` matches `NeoPixel`)
- **Preserves original case** in section names
- **Escapes quotes** for shell compatibility (`"D3"` → `\"D3\"`)
- **Multi-line support** for `build_flags = \n   -D... \n   -D...`

#### 2. C++ Preprocessor Engine (Build Flags → Resolved Macros)

**Technology**: Native C++ Compiler Preprocessor (g++, clang, arm-none-eabi-gcc)

**Why C++ Preprocessor?**
- Hardware defines use **nested macros**: `NEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3`
- `OKNXHW_OPENKNXIAO_D3` is defined in `lib/OGM-HardwareConfig/src/hardware/Hardware_*.h`
- Only the **actual compiler** can resolve these dependencies correctly
- No need to reimplement complex C preprocessor logic in PowerShell

**Process**:
```powershell
# Step 1: Build compiler command
$compilerArgs = @(
  "-E"   # Preprocessor only (no compilation)
  "-P"   # Remove line markers (#line directives)
  "-dM"  # Output #define macros only
  "-Ilib/OGM-HardwareConfig/include"  # Include hardware headers
  "-DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1"  # Board identifier
  "-DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3"  # From INI
)

# Step 2: Feed temporary C++ file to preprocessor
$tempCppContent = @"
// Include hardware config headers
#include "hardware.h"

// Preprocessor will resolve all macros
"@

# Step 3: Execute preprocessor
$preprocessorOutput = & $compiler $compilerArgs < $tempCppFile
```

**Input Example**:
```ini
# platformio.hardware.ini
[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1]
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3
    -DNEOPIXEL_HW_PORT_1_LABEL=\"D3\"
```

**Preprocessing Chain**:
```cpp
// 1. INI defines (via -D flags)
NEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3

// 2. Hardware header (lib/OGM-HardwareConfig/.../Hardware_OpenKNXiao_KNeoPix_RP2040_V1.h)
#ifdef OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
  #define OKNXHW_OPENKNXIAO_D3 5
#endif

// 3. Preprocessor resolves recursively
NEOPIXEL_HW_PORT_1_GPIO 
  → OKNXHW_OPENKNXIAO_D3 
  → 5
```

**Output** (`g++ -E -dM`):
```cpp
#define OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 1
#define OKNXHW_OPENKNXIAO_D3 5
#define NEOPIXEL_HW_PORT_1_GPIO 5
#define NEOPIXEL_HW_PORT_1_LABEL "D3"
#define HW_DEVICE_ID_BIT 0x1001
#define HW_DEVICE_NAME "OpenKNXiao KNeoPix RP2040"
```

**Compiler Detection Logic**:
```powershell
# Priority 1: PlatformIO toolchain (finds exact compiler for target)
$pioCompiler = Find-PlatformIOCompiler

# Priority 2: System compilers
$systemCompilers = @("g++", "clang++", "cpp")

# Result: Correct compiler with matching hardware defines
```

---

## Deep Dive: INI Parser Implementation

### Section Detection Algorithm

**Challenge**: Find all hardware sections for a specific feature across entire INI file

**Solution**: Regex pattern matching with case-insensitive search

```powershell
# Input: platformio.hardware.ini (raw text)
$iniContent = Get-Content "platformio.hardware.ini" -Raw

# Pattern: [featurename_oknxhw_ANYTHING]
$featureNameLower = "neopixel"  # User input, normalized
$sectionPattern = "\[${featureNameLower}_oknxhw_([^\]]+)\]"

# Find ALL matches (not just first)
$matches = [regex]::Matches(
    $iniContent, 
    $sectionPattern,
    [System.Text.RegularExpressions.RegexOptions]::IgnoreCase
)

# Example matches:
# [0] → "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1"
# [1] → "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2350_V1"
# [2] → "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32S3_V1"
# ... up to [7]
```

### Section Content Extraction

**Challenge**: Extract content between section header and next section (or EOF)

**Solution**: Substring manipulation with position tracking

```powershell
foreach ($match in $matches) {
  # Find section start position
  $sectionStartPos = $match.Index
  $sectionBracketEnd = $iniContent.IndexOf("]", $sectionStartPos)
  
  # Extract section name (preserve original case!)
  $sectionLineStart = $iniContent.LastIndexOf("`n", $sectionStartPos) + 1
  $sectionName = $iniContent.Substring(
      $sectionLineStart + 1,  # Skip "["
      $sectionBracketEnd - $sectionLineStart - 1
  )
  # Result: "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1"
  
  # Find next section (or end of file)
  $nextSectionPos = $iniContent.IndexOf("`n[", $sectionBracketEnd)
  if ($nextSectionPos -eq -1) { $nextSectionPos = $iniContent.Length }
  
  # Extract content between sections
  $sectionContent = $iniContent.Substring(
      $sectionBracketEnd + 1,
      $nextSectionPos - $sectionBracketEnd - 1
  )
}
```

**Example Section Content**:
```ini
board = rpipico
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
    -DNEOPIXEL_HW_GPIO_PORTS
    -DNEOPIXEL_HW_PORT_COUNT=7
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL
    -DNEOPIXEL_HW_PORT_1_LABEL=\"Onboard\"
```

### Build Flags Parsing

**Challenge**: Extract multi-line `build_flags = ...` block with line continuations

**Solution**: Regex with non-greedy matching until next section key

```powershell
# Match: "build_flags = ANYTHING until next section key OR end"
$buildFlagsText = ""
if ($sectionContent -match "(?s)build_flags\s*=(.*?)(?=\n\S|\z)") {
    $buildFlagsText = $matches[1]
}

# Regex breakdown:
# (?s)           → Dotall mode (. matches newlines)
# build_flags    → Literal text
# \s*            → Optional whitespace
# =              → Equals sign
# (.*?)          → Capture ANYTHING (non-greedy)
# (?=\n\S|\z)    → Stop at: newline + non-whitespace OR end-of-string

# Example captured text:
#     -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
#     -DNEOPIXEL_HW_GPIO_PORTS
#     -DNEOPIXEL_HW_PORT_COUNT=7
#     ...
```

### Define Flag Extraction

**Challenge**: Parse individual `-D` flags from multi-line text, handle quotes

**Solution**: Regex match + quote escaping

```powershell
$buildFlags = @()

# Find all -DNAME or -DNAME=VALUE patterns
$defineMatches = [regex]::Matches($buildFlagsText, "-D([^\s]+)")

foreach ($defineMatch in $defineMatches) {
    $defineValue = $defineMatch.Groups[1].Value
    # Example: NEOPIXEL_HW_PORT_1_LABEL=\"Onboard\"
    
    # Escape quotes for shell compatibility
    # Input:  KEY="VALUE"
    # Output: KEY=\"VALUE\"
    $defineValue = $defineValue -replace '"', '\"'
    
    $buildFlags += "-D$defineValue"
}

# Result array:
# ["-DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1",
#  "-DNEOPIXEL_HW_GPIO_PORTS",
#  "-DNEOPIXEL_HW_PORT_COUNT=7",
#  "-DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL",
#  "-DNEOPIXEL_HW_PORT_1_LABEL=\"Onboard\""]
```

### Hardware Config Object Assembly

```powershell
$hardwareConfigs += @{
    Name        = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1"
    SectionName = "neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1"
    BuildFlags  = $buildFlags  # Array of -D flags
    GPIOPorts   = @()          # Populated later by C++ preprocessor
    DeviceIdBit = 0            # Populated later (e.g., 4097)
    DeviceId    = ""           # Populated later
    DeviceName  = ""           # Populated later
}
```

**Output**: Array of 8 hardware config objects ready for preprocessor phase

---

## Deep Dive: C++ Preprocessor Engine

### Why Use Real Compiler?

**Problem**: Hardware macros are **deeply nested**:

```cpp
// platformio.hardware.ini defines:
-DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3

// But OKNXHW_OPENKNXIAO_D3 is defined in:
// lib/OGM-HardwareConfig/src/hardware/Hardware_OpenKNXiao_KNeoPix_RP2040_V1.h
#ifdef OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
  #define OKNXHW_OPENKNXIAO_NEOPIXEL 12
  #define OKNXHW_OPENKNXIAO_D3 5
  #define OKNXHW_OPENKNXIAO_D4 6
#endif

// And that file includes:
#include "Hardware_OpenKNXiao_Pinout.h"
```

**Naive Approach (doesn't work)**:
- PowerShell regex to parse C++ defines? → Fails on `#ifdef`, `#include`, complex macros
- Manual define database? → Duplicates hardware config, gets out of sync

**Correct Approach**: Use the **actual C++ compiler** that knows how to:
- Process `#include` directives
- Evaluate `#ifdef` conditions
- Resolve nested macro expansions
- Handle conditional compilation

### Preprocessor Invocation

```powershell
# Compiler command construction
$compilerArgs = @(
    "-E"    # Run preprocessor only (no compilation)
    "-P"    # Suppress line markers (#line directives)
    "-dM"   # Dump macro definitions
    "-Ilib/OGM-HardwareConfig/include"  # Hardware headers
    "-DFEATURE_PREFIX=NEOPIXEL_HW"      # Feature namespace
)

# Add hardware-specific build flags from INI
$compilerArgs += $hwConfig.BuildFlags
# Result: [..., "-DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1", 
#          "-DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3", ...]

# Create temporary C++ source file
$tempCppContent = @"
// This file is fed to preprocessor
#include "hardware.h"
"@

# Execute preprocessor
$preprocessorOutput = & $compiler $compilerArgs -x c++ - <<< $tempCppContent
```

### Preprocessor Output Parsing

**Raw Output** (`g++ -E -dM`):
```cpp
#define __STDC__ 1
#define __cplusplus 201703L
#define OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 1
#define OKNXHW_OPENKNXIAO_NEOPIXEL 12
#define OKNXHW_OPENKNXIAO_D3 5
#define OKNXHW_OPENKNXIAO_D4 6
#define NEOPIXEL_HW_GPIO_PORTS 1
#define NEOPIXEL_HW_PORT_COUNT 7
#define NEOPIXEL_HW_PORT_1_GPIO 5           ← Resolved!
#define NEOPIXEL_HW_PORT_1_LABEL "Onboard"
#define HW_DEVICE_ID_BIT 0x1001
#define HW_DEVICE_NAME "OpenKNXiao KNeoPix RP2040"
```

**Parsing Logic**:
```powershell
$hwData = @{}

foreach ($line in $preprocessorOutput -split "`n") {
    # Match: #define KEY VALUE
    if ($line -match "^#define\s+(\S+)\s+(.*)$") {
        $key = $matches[1]
        $value = $matches[2].Trim()
        
        # Filter: Only keep relevant defines
        if ($key -match "^(NEOPIXEL_HW|HW_DEVICE|FEATURE_PREFIX)") {
            $hwData[$key] = $value
        }
    }
}

# Result hashtable:
# @{
#   "NEOPIXEL_HW_GPIO_PORTS" = "1"
#   "NEOPIXEL_HW_PORT_COUNT" = "7"
#   "NEOPIXEL_HW_PORT_1_GPIO" = "5"           ← Number!
#   "NEOPIXEL_HW_PORT_1_LABEL" = "\"Onboard\""
#   "HW_DEVICE_ID_BIT" = "0x1001"
#   "HW_DEVICE_NAME" = "\"OpenKNXiao KNeoPix RP2040\""
# }
```

### Device Information Extraction

```powershell
# Extract device metadata
$hwConfig.DeviceId = if ($hwData.ContainsKey("HW_DEVICE_ID")) {
    # Handle concatenated string literals: "A" "B" → "AB"
    $deviceId = $hwData["HW_DEVICE_ID"] -replace '"\s*"', ''
    $deviceId -replace '"', ''  # Remove all quotes
} else {
    $hwName  # Fallback to section name
}

$hwConfig.DeviceIdBit = if ($hwData.ContainsKey("HW_DEVICE_ID_BIT")) {
    # Parse hex: 0x1001 → 4097 (decimal)
    [int]($hwData["HW_DEVICE_ID_BIT"])
} else {
    0  # Invalid → Will be filtered out later
}

# Example result:
# DeviceId = "OpenKNXiao_KNeoPix_RP2040_V1"
# DeviceName = "OpenKNXiao KNeoPix RP2040"
# DeviceIdBit = 4097 (0x1001)
```

### GPIO Port Extraction Loop

```powershell
# Check if hardware defines GPIO ports
$hwPortsAvailable = $hwData["HW_GPIO_PORTS_DEFINED"] -eq "1"
$portCount = [int]$hwData["HW_GPIO_PORT_COUNT"]

if ($hwPortsAvailable -and $portCount -gt 0) {
    for ($i = 1; $i -le $portCount; $i++) {
        # Build key names for this port
        $gpioKey = "HW_GPIO_PORT_${i}_GPIO_DATA"
        $labelKey = "HW_GPIO_PORT_${i}_LABEL"
        
        if ($hwData.ContainsKey($gpioKey)) {
            # Extract label (remove quotes and backslashes)
            $label = $hwData[$labelKey] -replace '"', '' -replace '\\+', ''
            # Result: "D3" → D3
            
            # Extract GPIO number (for JavaScript conflict detection)
            $gpioNum = $hwData["NEOPIXEL_HW_PORT_${i}_GPIO"]
            # Result: "5" → 5
            
            # Store in hardware config
            $hwConfig.GPIOPorts += @{
                Index = $i       # Port index (1-based)
                Label = $label   # User-friendly name ("D3")
                GPIO  = $gpioNum # Actual GPIO number (5)
            }
        }
    }
}

# Final GPIOPorts array for RP2040:
# @(
#   @{ Index=1, Label="Onboard", GPIO=12 },
#   @{ Index=2, Label="D4",      GPIO=6  },
#   @{ Index=3, Label="D5",      GPIO=7  },
#   @{ Index=4, Label="D8",      GPIO=28 },
#   @{ Index=5, Label="D9",      GPIO=29 },
#   @{ Index=6, Label="D3",      GPIO=5  },
#   @{ Index=7, Label="SPI-CLK", GPIO=10 }
# )
```

### Error Handling & Validation

```powershell
# Filter out hardware without valid DEVICE_ID_BIT
$hardwareConfigsFiltered = @()

foreach ($hwConfig in $hardwareConfigs) {
    if ($hwConfig.DeviceIdBit -eq 0) {
        Write-Warning "Hardware '$($hwConfig.Name)' has no DEVICE_HW_ID - skipping"
        # This hardware won't appear in ETS dropdown
    } else {
        $hardwareConfigsFiltered += $hwConfig
    }
}

# Validation: Must have at least 1 hardware with valid ID
if ($hardwareConfigsFiltered.Count -eq 0) {
    Write-Error "No hardware configurations with DEVICE_HW_ID found"
    exit 1
}
```

---

## Preprocessor Output Examples

### Example 1: RP2040 (7 Ports)

**Input** (`platformio.hardware.ini`):
```ini
[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1]
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL
    -DNEOPIXEL_HW_PORT_2_GPIO=OKNXHW_OPENKNXIAO_D4
```

**C++ Header** (`Hardware_OpenKNXiao_KNeoPix_RP2040_V1.h`):
```cpp
#ifdef OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
  #define HW_DEVICE_ID_BIT 0x1001
  #define HW_DEVICE_NAME "OpenKNXiao KNeoPix RP2040"
  #define OKNXHW_OPENKNXIAO_NEOPIXEL 12
  #define OKNXHW_OPENKNXIAO_D4 6
#endif
```

**Preprocessor Output**:
```cpp
#define OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 1
#define HW_DEVICE_ID_BIT 0x1001
#define HW_DEVICE_NAME "OpenKNXiao KNeoPix RP2040"
#define NEOPIXEL_HW_PORT_1_GPIO 12         ← Resolved from OKNXHW_OPENKNXIAO_NEOPIXEL
#define NEOPIXEL_HW_PORT_2_GPIO 6          ← Resolved from OKNXHW_OPENKNXIAO_D4
```

**Extracted Data**:
```powershell
@{
    DeviceIdBit = 4097
    DeviceName = "OpenKNXiao KNeoPix RP2040"
    GPIOPorts = @(
        @{ Index=1, Label="Onboard", GPIO=12 },
        @{ Index=2, Label="D4",      GPIO=6  }
    )
}
```

### Example 2: ESP32S3 (Different Pins)

**Input**:
```ini
[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_ESP32S3_V1]
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL
```

**C++ Header**:
```cpp
#ifdef OKNXHW_OPENKNXIAO_KNEOPIX_ESP32S3_V1
  #define HW_DEVICE_ID_BIT 0x1003
  #define OKNXHW_OPENKNXIAO_NEOPIXEL 48  ← Different GPIO!
#endif
```

**Preprocessor Output**:
```cpp
#define NEOPIXEL_HW_PORT_1_GPIO 48  ← Same macro, different value!
```

**Key Insight**: Same macro name (`NEOPIXEL_HW_PORT_1_GPIO`) resolves to different GPIO numbers depending on hardware variant. This is why we need per-hardware ParameterTypes in ETS!

---

## From Preprocessor Output to ETS XML

### Data Transformation Pipeline

```powershell
# Input: 8 hardware configs with resolved GPIO data
foreach ($hwConfig in $hardwareConfigs) {
    # Generate ParameterType for this hardware
    $parameterTypeXml += @"
<ParameterType Id="%AID%_PT-NeoPixelGPIOPortHW${hwIdx}" Type="Enum">
  <Item Text="---" Value="15" DisplayOrder="0"/>
"@
    
    # Add items for each GPIO port
    foreach ($port in $hwConfig.GPIOPorts) {
        $parameterTypeXml += @"
  <Item Text="$($port.Label) (GPIO $($port.GPIO))" 
        Value="$($port.Index - 1)" 
        DisplayOrder="$($port.Index)"/>
"@
    }
    
    # Add manual mode option
    $parameterTypeXml += @"
  <Item Text="Manuell" Value="10" DisplayOrder="99"/>
</ParameterType>
"@
}
```

**Result**: 8 ParameterTypes, each tailored to its hardware's available GPIOs

### Hardware Selection Dropdown Generation

```powershell
# Generate choose-when structure for hardware selection
$hardwareSelectionXml = "<choose>`n"

foreach ($hwConfig in $hardwareConfigs) {
    $hwId = $hwConfig.DeviceIdBit  # e.g., 4097
    $hwIdx = Get-HardwareIndex     # 0-7
    
    $hardwareSelectionXml += @"
  <when test="4000018 == $hwId">  <!-- $($hwConfig.DeviceName) -->
    <display ParameterRefId="%AID%_UP-%TT%9%C%073_R-%TT%9%C%07301"/>
  </when>
"@
}

$hardwareSelectionXml += "</choose>`n"
```

**How it works in ETS**:
1. User selects hardware from dropdown (Parameter 4000018)
2. ETS evaluates `<when test="4000018 == 4097">` for RP2040
3. Displays corresponding GPIO port parameter (073 = HW0 for RP2040)
4. Each hardware shows different ParameterType with correct GPIO options

### Critical Design Decisions

#### 1. Flat ParameterCalculations (No Choose-Blocks)

**Problem**: OpenKNXproducer does NOT process `<choose>` blocks in `Static/ParameterCalculations`

**Solution**: Generate 96 separate ParameterCalculations without wrappers:
```xml
<!-- Hardware 0: RP2040 -->
<ParameterCalculation Id="%AID%_PC-%TT%9%C%0800" Name="Strip%C%_CopyGPIOPort_HW0" .../>
<!-- Hardware 1: RP2350 -->
<ParameterCalculation Id="%AID%_PC-%TT%9%C%0801" Name="Strip%C%_CopyGPIOPort_HW1" .../>
<!-- ... 94 more ... -->
```

#### 2. Port Value Semantics

- **0-N**: Real hardware ports (0 is VALID since v1.0.5)
- **10**: Manual mode (user enters raw GPIO number)
- **15**: Dummy/unassigned (prevents auto-selection, used as default)

**Critical**: Conflict detection skips ONLY 10 and 15 (NOT 0!)

#### 3. ParameterRefRef Pattern for Conditional UI

**Problem**: ParameterSeparator visibility requires hidden ParameterRefRef

**Solution**:
```xml
<!-- Hidden trigger -->
<ParameterRefRef RefId="..." InternalDescription="HIDDEN"/>
<!-- Conditional separator -->
<choose>
  <when test="...">
    <ParameterSeparator Id="PS-unique%C%" Text="⚠ Warning!" IconId="..."/>
  </when>
</choose>
```

#### 4. ParameterSeparator ID Rules

**ETS Constraint**: ID after "PS-" must be **numeric** OR **text+%C%**

✅ Valid: `PS-123`, `PS-gpioconflict%C%`, `PS-manualdata%C%`  
❌ Invalid: `PS-manual-warning%C%` (generates `PS-manual-warning1` → ETS error: "'M' is not a legal digit")

### Hardware Configuration Format

## Usage

### For NeoPixel LED Strips (Example Implementation)

```powershell
./Build-NeoPixelStrips.ps1
```

This wrapper internally calls:
```powershell
./Build-DynamicGPIOTemplate.ps1 `
    -FeatureName "NeoPixel" `
    -DefinesPrefix "NEOPIXEL_HW" `
    -TemplateFile "src/NeoPixel.Strip.templ.xml.in" `
    -OutputFile "src/NeoPixel.Strip.templ.xml"
```

### For Custom Features

Create a wrapper script or call directly:

```powershell
./Build-DynamicGPIOTemplate.ps1 `
    -FeatureName "I2C" `
    -DefinesPrefix "I2C_HW" `
    -TemplateFile "src/I2C.Config.templ.xml.in" `
    -OutputFile "src/I2C.Config.templ.xml" `
    -MaxPorts 4
```

### Hardware Configuration Format

Hardware GPIO mappings are defined in **`platformio.hardware.ini`** using INI section syntax:

```ini
; ============================================================================
; Hardware GPIO Configuration for NeoPixel Module
; ============================================================================

; ----------------------------------------------------------------------------
; OpenKNXiao KNeoPix RP2040 V1 (7 GPIO Ports)
; ----------------------------------------------------------------------------
[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2040_V1]
board = rpipico
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1
    -DNEOPIXEL_HW_GPIO_PORTS
    -DNEOPIXEL_HW_PORT_COUNT=7
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL
    -DNEOPIXEL_HW_PORT_1_LABEL=\"Onboard\"
    -DNEOPIXEL_HW_PORT_2_GPIO=OKNXHW_OPENKNXIAO_D4
    -DNEOPIXEL_HW_PORT_2_LABEL=\"D4\"
    -DNEOPIXEL_HW_PORT_3_GPIO=OKNXHW_OPENKNXIAO_D5
    -DNEOPIXEL_HW_PORT_3_LABEL=\"D5\"
    ; ... more ports ...

; ----------------------------------------------------------------------------
; OpenKNXiao KNeoPix RP2350 V1 (9 GPIO Ports)
; ----------------------------------------------------------------------------
[neopixel_oknxhw_OPENKNXIAO_KNEOPIX_RP2350_V1]
board = rpipico2
build_flags =
    -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1
    -DNEOPIXEL_HW_GPIO_PORTS
    -DNEOPIXEL_HW_PORT_COUNT=9
    -DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_NEOPIXEL
    -DNEOPIXEL_HW_PORT_1_LABEL=\"Onboard\"
    ; ... more ports ...
```

### Naming Conventions

#### Section Name Pattern
```
[{feature}_{platform}_{BOARD_DEFINE}]
```
- **feature**: Lowercase feature name (e.g., `neopixel`, `i2c`, `uart`)
- **platform**: Always `oknxhw` (OpenKNX Hardware)
- **BOARD_DEFINE**: Exact match of board's `OKNXHW_*` define (e.g., `OPENKNXIAO_KNEOPIX_RP2040_V1`)

#### Build Flags Pattern
```cpp
-D{PREFIX}_GPIO_PORTS              // Feature flag (enables hardware mode)
-D{PREFIX}_PORT_COUNT=N            // Number of ports (1-9)
-D{PREFIX}_PORT_N_GPIO={MACRO}     // GPIO number (can be macro like OKNXHW_OPENKNXIAO_D3)
-D{PREFIX}_PORT_N_LABEL="Label"    // User-friendly port name
```

**Important**: Use `\"` for escaped quotes in labels!

### C++ Preprocessor Magic

The script uses the **actual C++ compiler** to resolve hardware-specific macros:

```cpp
// In lib/OGM-HardwareConfig/src/hardware/Hardware_OpenKNXiao_KNeoPix_RP2040_V1.h
#define OKNXHW_OPENKNXIAO_D3 5
#define OKNXHW_OPENKNXIAO_D4 6

// In platformio.hardware.ini
-DNEOPIXEL_HW_PORT_1_GPIO=OKNXHW_OPENKNXIAO_D3

// Preprocessor resolves to:
NEOPIXEL_HW_PORT_1_GPIO = 5
```

**How it works**:
1. Script extracts `-D` flags from INI section
2. Creates temporary C++ file: `#include "hardware.h"`
3. Runs compiler preprocessor: `g++ -E -dM -DOKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1 ...`
4. Parses `#define NEOPIXEL_HW_PORT_1_GPIO 5` from output
5. Generates ETS XML with resolved GPIO numbers

## Generated ETS XML Structure

### Multi-Hardware ParameterTypes

For **each hardware variant**, the script generates a unique ParameterType:

```xml
<!-- share.xml: Hardware 0 (RP2040 with 7 ports) -->
<ParameterType Id="%AID%_PT-NeoPixelGPIOPortHW0" Type="Enum">
  <Item Text="---" Value="15" DisplayOrder="0"/>           <!-- Dummy (default) -->
  <Item Text="Onboard (GPIO 12)" Value="0" DisplayOrder="1"/>
  <Item Text="D4 (GPIO 6)" Value="1" DisplayOrder="2"/>
  <Item Text="D5 (GPIO 7)" Value="2" DisplayOrder="3"/>
  <!-- ... 4 more ports ... -->
  <Item Text="Manuell" Value="10" DisplayOrder="99"/>      <!-- Manual mode -->
</ParameterType>

<!-- share.xml: Hardware 1 (RP2350 with 9 ports) -->
<ParameterType Id="%AID%_PT-NeoPixelGPIOPortHW1" Type="Enum">
  <Item Text="---" Value="15" DisplayOrder="0"/>
  <Item Text="Onboard (GPIO 12)" Value="0" DisplayOrder="1"/>
  <!-- ... 9 ports total ... -->
  <Item Text="Manuell" Value="10" DisplayOrder="99"/>
</ParameterType>
```

### Template Parameters (8 per Strip)

Each LED strip has **8 hardware-specific parameters** (073-080 for Data, 081-088 for Clock):

```xml
<!-- Strip.templ.xml: Strip %C% Parameters -->
<Parameter Id="%AID%_UP-%TT%9%C%073" Name="NEO%C%GPIODataPortHW0" 
           ParameterType="%AID%_PT-NeoPixelGPIOPortHW0" Value="15"/>
<Parameter Id="%AID%_UP-%TT%9%C%074" Name="NEO%C%GPIODataPortHW1" 
           ParameterType="%AID%_PT-NeoPixelGPIOPortHW1" Value="15"/>
<!-- ... HW2-HW7 ... -->
```

### Share Parameters (1 per Strip)

Shared parameters receive the **actual port value** via ParameterCalculation:

```xml
<!-- share.xml: Shared Data/Clock Port Storage -->
<Parameter Id="%AID%_UP-%TT%0010%C%" Name="NEOGPIODataPort%C%" 
           ParameterType="%AID%_PT-NeoPixelGPIOPortHW0" Value="15"/>
<Parameter Id="%AID%_UP-%TT%0011%C%" Name="NEOGPIOClockPort%C%" 
           ParameterType="%AID%_PT-NeoPixelGPIOPortHW0" Value="15"/>
```

### Flat ParameterCalculations (96 Total)

**OpenKNXproducer Limitation**: Does NOT process `<choose>` blocks in Static/ParameterCalculations section.

**Workaround**: Generate 96 separate calculations (8 hardware × 6 strips × 2 GPIO types):

```xml
<!-- Strip.templ.xml: Copy Template → Share (6 strips × 8 hardware × 2 GPIOs = 96) -->

<!-- Strip 1, Data GPIO, Hardware 0 -->
<ParameterCalculation Id="%AID%_PC-%TT%9%C%0800" Name="Strip%C%_CopyGPIOPort_HW0"
                      LRTransformationFunc="NEO_CopyValue" RLTransformationFunc="NEO_Empty">
  <LParameters>
    <ParameterRefRef RefId="%AID%_UP-%TT%9%C%073_R-%TT%9%C%07301" AliasName="TemplateGPIOPort"/>
  </LParameters>
  <RParameters>
    <ParameterRefRef RefId="%AID%_UP-%TT%00101_R-%TT%0010101" AliasName="ShareGPIOPort"/>
  </RParameters>
</ParameterCalculation>

<!-- Strip 1, Data GPIO, Hardware 1 -->
<ParameterCalculation Id="%AID%_PC-%TT%9%C%0801" Name="Strip%C%_CopyGPIOPort_HW1" ... />

<!-- ... 94 more calculations ... -->
```

**Why 96?**: Each strip needs separate calculation for each hardware (no dynamic Choose)

## GPIO Conflict Detection

### The Challenge

With 6 LED strips and multiple hardware variants, **GPIO conflicts** are inevitable:

```
Strip 1: WS2812B on D4 (GPIO 6)
Strip 2: WS2812B on D4 (GPIO 6)  ❌ CONFLICT!

Strip 1: APA102 on D4 (GPIO 6+7 for SPI)
Strip 2: WS2812B on D5 (GPIO 7)  ❌ CONFLICT! (SPI uses 2 GPIOs)
```

### 4-Way Conflict Detection

The system checks **all combinations**:

1. **Data-Data**: Strip 1 Data vs Strip 2 Data
2. **Clock-Clock**: Strip 1 Clock vs Strip 2 Clock
3. **Data-Clock**: Strip 1 Data vs Strip 2 Clock (SPI collision)
4. **Clock-Data**: Strip 1 Clock vs Strip 2 Data (SPI collision)

**Skip Logic**: Only ignore ports with Value=**10** (Manual) or **15** (Dummy). Port **0 is VALID**!

### JavaScript Implementation

```javascript
// NeoPixel.script.js (auto-generated)
function NEO_DetectGPIOConflicts() {
  var conflicts = {
    dataConflicts: [],
    clockConflicts: [],
    dataClockConflicts: [],
    clockDataConflicts: []
  };
  
  // Read 12 ports (6 strips × 2 GPIOs)
  var dataPorts = [
    getLParameter("ShareDataPort1"),
    getLParameter("ShareDataPort2"),
    // ... up to 6 ...
  ];
  
  var clockPorts = [ /* same for clock */ ];
  
  // 4-way comparison (skip Manual=10, Dummy=15)
  for (var i = 0; i < 6; i++) {
    for (var j = i + 1; j < 6; j++) {
      var portA_data = dataPorts[i];
      var portB_data = dataPorts[j];
      
      if (portA_data != 10 && portA_data != 15 &&
          portB_data != 10 && portB_data != 15 &&
          portA_data == portB_data) {
        conflicts.dataConflicts.push([i+1, j+1]);
      }
      
      // ... same for Clock-Clock, Data-Clock, Clock-Data ...
    }
  }
  
  // Write conflict flags (00091-00096 Data, 00117-00122 Clock)
  setRParameter("DataConflict1", conflicts.dataConflicts.includes(1) ? 1 : 0);
  // ... up to 6 strips ...
}
```

### Conflict Status Display

```xml
<!-- Strip.templ.xml: Visual conflict warning -->
<ParameterSeparator Id="PS-gpioconflict%C%" 
                    Text="⚠ GPIO-Konflikt erkannt! Strip %C% belegt bereits verwendeten Port." 
                    IconId="icon_alert">
  <choose>
    <when test="%AID%_UP-%TT%00091 == 1">      <!-- Data conflict flag -->
      <display/>
    </when>
    <when test="%AID%_UP-%TT%00117 == 1">      <!-- Clock conflict flag -->
      <display/>
    </when>
  </choose>
</ParameterSeparator>
```

## Hardware Change Reset

### The Problem

User workflow:
1. Selects RP2040 hardware → Configures Strip 1 on "D3" (Port Value=5)
2. Switches to ESP32S3 (doesn't have "D3")
3. **Port Value=5 persists** → Points to wrong GPIO or doesn't exist!

### The Solution

Automatic reset of **all 12 ports** (6 Data + 6 Clock) to **Dummy (15)** when hardware changes:

```xml
<!-- share.xml: Hardware Change Trigger -->
<ParameterCalculation Id="%AID%_UP-00061" Name="ResetAllGPIOsOnHardwareChange"
                      LRTransformationFunc="NEO_ResetAllPortsOnHardwareChange"
                      RLTransformationFunc="NEO_Empty">
  <LParameters>
    <ParameterRefRef RefId="%AID%_UP-4000018_R-400001801" AliasName="HardwareSelection"/>
  </LParameters>
  <RParameters>
    <!-- All 12 share ports -->
    <ParameterRefRef RefId="%AID%_UP-%TT%00101_R-%TT%0010101" AliasName="DataPort1"/>
    <ParameterRefRef RefId="%AID%_UP-%TT%00102_R-%TT%0010201" AliasName="DataPort2"/>
    <!-- ... DataPort3-6, ClockPort1-6 ... -->
  </RParameters>
</ParameterCalculation>
```

```javascript
// NeoPixel.script.js
function NEO_ResetAllPortsOnHardwareChange() {
  // Reset all 12 ports to Dummy (15)
  setRParameter("DataPort1", 15);
  setRParameter("DataPort2", 15);
  // ... up to DataPort6 ...
  setRParameter("ClockPort1", 15);
  // ... up to ClockPort6 ...
}
```

**Trigger**: Runs automatically when user changes `4000018` (Hardware Selection dropdown)

## Usage

### For NeoPixel LED Strips

Direct invocation:

```powershell
pwsh scripts/Build-DynamicGPIOTemplate.ps1 -FeatureName NeoPixel -DefinesPrefix NEO
```

The script automatically:
1. Scans `platformio.hardware.ini` for `[neopixel_oknxhw_*]` sections
2. Extracts GPIO configurations via C++ preprocessor
3. Generates 8 ParameterTypes (HW0-HW7)
4. Generates 96 ParameterCalculations (flat, no Choose)
5. Updates `src/NeoPixel.Strip.templ.xml`, `src/NeoPixel.share.xml`, `src/NeoPixel.script.js`

### For Custom Features (I2C, UART, etc.)

1. **Create hardware sections** in `platformio.hardware.ini`:

```ini
[i2c_oknxhw_MYBOARD_V1]
build_flags =
    -DI2C_HW_GPIO_PORTS
    -DI2C_HW_PORT_COUNT=2
    -DI2C_HW_PORT_1_GPIO=4
    -DI2C_HW_PORT_1_LABEL=\"I2C0\"
    -DI2C_HW_PORT_2_GPIO=6
    -DI2C_HW_PORT_2_LABEL=\"I2C1\"
```

2. **Run generator**:

```powershell
pwsh scripts/Build-DynamicGPIOTemplate.ps1 `
    -FeatureName "I2C" `
    -DefinesPrefix "I2C_HW" `
    -TemplateFile "src/I2C.Config.templ.xml" `
    -ShareXml "src/I2C.share.xml" `
    -JavaScriptFile "src/I2C.script.js" `
    -MaxPorts 4
```

### Integration into Build Process

Add to `scripts/Build-Release.ps1`:

```powershell
# Generate dynamic GPIO templates
Write-Host "Generating multi-hardware GPIO templates..." -ForegroundColor Cyan
& "$PSScriptRoot/Build-DynamicGPIOTemplate.ps1" -FeatureName NeoPixel -DefinesPrefix NEO
if ($LASTEXITCODE -ne 0) {
    Write-Error "GPIO template generation failed"
    exit 1
}
```

## Script Parameters

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| FeatureName | No | "NeoPixel" | Display name for console output |
| DefinesPrefix | No | "NEO" | Prefix for hardware defines (e.g., NEO_HW_PORT_1_GPIO) |
| TemplateFile | No | "src/NeoPixel.Strip.templ.xml" | Template file to update |
| ShareXml | No | "src/NeoPixel.share.xml" | Share XML file to update |
| JavaScriptFile | No | "src/NeoPixel.script.js" | JavaScript file to update |
| MaxPorts | No | 7 | Maximum GPIO ports per hardware |
| CleanOnly | No | false | Only clean markers, don't generate |
| DryRun | No | false | Show what would be generated without writing |
| VerboseMode | No | false | Extra debug output |

## Troubleshooting

### No Hardware Configurations Found

**Symptom**: `No hardware configurations found matching pattern: NEO_*`

**Cause**: Missing or misnamed sections in `platformio.hardware.ini`

**Solution**: 
```ini
; ❌ Wrong: Missing prefix
[oknxhw_MYBOARD_V1]

; ✅ Correct: Must start with feature name
[neopixel_oknxhw_MYBOARD_V1]
```

### Preprocessor Extraction Failed

**Symptom**: `Failed to extract GPIO configuration for hardware X`

**Cause**: C++ compiler not found or hardware defines missing

**Solution**:
```bash
# Check compiler
which g++
which arm-none-eabi-g++

# Test manually
g++ -E -dM -DOKNXHW_MYBOARD_V1 -DNEOPIXEL_HW_PORT_1_GPIO=5 \
    -I. -Iinclude -Ilib/OGM-HardwareConfig/src \
    -x c++ - < /dev/null | grep NEOPIXEL_HW
```

### ETS Import Error: "'M' is not a legal digit"

**Symptom**: ETS fails to import knxprod with digit parsing error

**Cause**: ParameterSeparator ID contains illegal characters

**Solution**:
```xml
<!-- ❌ Wrong: Text without %C% generates PS-manual-warning1 -->
<ParameterSeparator Id="PS-manual-warning%C%" .../>

<!-- ✅ Correct: Unique text + %C% pattern -->
<ParameterSeparator Id="PS-manualdata%C%" .../>
<ParameterSeparator Id="PS-manualclock%C%" .../>
```

### GPIO Conflict Not Detected

**Symptom**: Two strips use same GPIO, no warning shown

**Cause**: Port value is 10 (Manual) or 15 (Dummy) - conflict detection skips these

**Solution**: Check share parameter values (00101-00106, 00111-00116). Only **real ports (0-9)** trigger conflicts.

### Hardware Switch Doesn't Reset Ports

**Symptom**: Old port values persist after changing hardware

**Cause**: Hardware Change Reset ParameterCalculation not triggered

**Solution**: Verify `share.xml` contains:
```xml
<ParameterCalculation Id="%AID%_UP-00061" Name="ResetAllGPIOsOnHardwareChange" ...>
  <LParameters>
    <ParameterRefRef RefId="%AID%_UP-4000018_R-400001801" AliasName="HardwareSelection"/>
  </LParameters>
  <!-- ... 12 RParameters ... -->
</ParameterCalculation>
```

### Wrong GPIO Count in ETS

**Symptom**: Dropdown shows 5 ports, but hardware has 7

**Cause**: Cached ParameterType or build flags incorrect

**Solution**:
```bash
# Regenerate with verbose mode
pwsh scripts/Build-DynamicGPIOTemplate.ps1 -VerboseMode

# Check platformio.hardware.ini
grep -A 20 "\[neopixel_oknxhw_MYBOARD" platformio.hardware.ini
```

## Advanced Features

### Multi-Hardware Support (8 Variants)

The system generates ETS XML that supports **8 different hardware boards simultaneously**:

```xml
<!-- ETS Application Package with 8 Hardware Options -->
<choose>
  <when test="4000018 == 4097">  <!-- OpenKNXiao RP2040 -->
    <display ParameterRefId="%AID%_UP-%TT%9%C%073_R-%TT%9%C%07301"/>  <!-- HW0 -->
  </when>
  <when test="4000018 == 4098">  <!-- OpenKNXiao RP2350 -->
    <display ParameterRefId="%AID%_UP-%TT%9%C%074_R-%TT%9%C%07401"/>  <!-- HW1 -->
  </when>
  <!-- ... HW2-HW7 ... -->
</choose>
```

**Hardware ID Mapping**:
- 4097: OpenKNXiao RP2040 (HW0)
- 4098: OpenKNXiao RP2350 (HW1)
- 4099: OpenKNXiao ESP32S3 (HW2)
- 4104: OpenKNXiao ESP32C3 (HW3)
- 4105: OpenKNXiao ESP32C6 (HW4)
- 4100: OpenKNXiao RP2040 Mini (HW5)
- 4101: OpenKNXiao ESP32S3 Mini (HW6)
- 4112: OpenKNXiao ESP32C3 Mini (HW7)

### Conditional Port Definitions

Use preprocessor conditions in hardware defines:

```ini
[neopixel_oknxhw_ADVANCED_BOARD_V1]
build_flags =
    -DBOARD_HAS_EXTERNAL_GPIO_EXPANDER
    -DNEOPIXEL_HW_PORT_COUNT=16  ; Extended ports via expander
```

In C++ code:
```cpp
#ifdef BOARD_HAS_EXTERNAL_GPIO_EXPANDER
  #define MAX_STRIPS 12
#else
  #define MAX_STRIPS 6
#endif
```

### CleanOnly Mode

Useful for debugging or resetting to empty state:

```powershell
pwsh scripts/Build-DynamicGPIOTemplate.ps1 -CleanOnly
```

Removes all auto-generated content between markers:
- `<!-- BEGIN AUTO-GENERATED: ... -->` ... `<!-- END AUTO-GENERATED: ... -->`

### DryRun Mode

Preview what would be generated without modifying files:

```powershell
pwsh scripts/Build-DynamicGPIOTemplate.ps1 -DryRun -VerboseMode
```

Output shows generated XML and affected files without writing changes.

## Performance & Impact

- **Build Time**: +2-5 seconds (C++ preprocessor × 8 hardware variants + XML generation)
- **Runtime Impact**: Zero (all processing is build-time only)
- **ETS Import Time**: No measurable difference
- **File Size**: 
  - `share.xml`: +15 KB (8 ParameterTypes + conflict parameters)
  - `Strip.templ.xml`: +25 KB (96 ParameterCalculations + conflict UI)
  - `script.js`: +8 KB (conflict detection + hardware reset functions)

## Implementation Guidelines for Other OAM Modules

Want to add multi-hardware GPIO support to your OpenKNX module? Follow these steps:

### Step 1: Create platformio.hardware.ini

```ini
; Replace "myfeature" with your feature name (lowercase)
; Replace "MYFEATURE_HW" with your defines prefix (uppercase)

[myfeature_oknxhw_BOARD1_V1]
build_flags =
    -DMYFEATURE_HW_GPIO_PORTS
    -DMYFEATURE_HW_PORT_COUNT=5
    -DMYFEATURE_HW_PORT_1_GPIO=4
    -DMYFEATURE_HW_PORT_1_LABEL=\"SDA\"
    ; ... more ports ...

[myfeature_oknxhw_BOARD2_V1]
build_flags =
    -DMYFEATURE_HW_GPIO_PORTS
    -DMYFEATURE_HW_PORT_COUNT=3
    ; ... ports ...
```

### Step 2: Copy Build Script

```bash
cp scripts/Build-DynamicGPIOTemplate.ps1 scripts/Build-MyFeatureGPIO.ps1
```

Or call directly with your parameters in `scripts/Build-Release.ps1`.

### Step 3: Create XML Template Markers

In your `src/MyFeature.Config.templ.xml`:

```xml
<Parameters>
  <!-- BEGIN AUTO-GENERATED: GPIO Port Selection Parameter -->
  <!-- Cleaned - Ready for regeneration -->
  <!-- END AUTO-GENERATED: GPIO Port Selection Parameter -->
</Parameters>

<ParameterRefs>
  <!-- BEGIN AUTO-GENERATED: GPIO Port Selection ParameterRef -->
  <!-- Cleaned - Ready for regeneration -->
  <!-- END AUTO-GENERATED: GPIO Port Selection ParameterRef -->
</ParameterRefs>

<ParameterCalculations>
  <!-- BEGIN AUTO-GENERATED: GPIO Copy ParameterCalculation -->
  <!-- Cleaned - Ready for regeneration -->
  <!-- END AUTO-GENERATED: GPIO Copy ParameterCalculation -->
</ParameterCalculations>
```

In your `src/MyFeature.share.xml`:

```xml
<ParameterTypes>
  <!-- BEGIN AUTO-GENERATED: Hardware-Specific GPIO ParameterTypes -->
  <!-- Cleaned - Ready for regeneration -->
  <!-- END AUTO-GENERATED: Hardware-Specific GPIO ParameterTypes -->
</ParameterTypes>

<Parameters>
  <!-- BEGIN AUTO-GENERATED: Shared GPIO Port Parameters -->
  <!-- Cleaned - Ready for regeneration -->
  <!-- END AUTO-GENERATED: Shared GPIO Port Parameters -->
</Parameters>
```

### Step 4: Run Generator

```powershell
pwsh scripts/Build-DynamicGPIOTemplate.ps1 `
    -FeatureName "MyFeature" `
    -DefinesPrefix "MYFEATURE_HW" `
    -TemplateFile "src/MyFeature.Config.templ.xml" `
    -ShareXml "src/MyFeature.share.xml" `
    -JavaScriptFile "src/MyFeature.script.js" `
    -MaxPorts 8
```

### Step 5: Implement JavaScript Functions

The generator creates function stubs. Implement:

```javascript
// Auto-generated by Build-DynamicGPIOTemplate.ps1
function MYFEATURE_CopyValue() {
  var templateValue = getLParameter("TemplateGPIOPort");
  setRParameter("ShareGPIOPort", templateValue);
}

function MYFEATURE_Empty() {
  // Intentionally empty (RLTransformationFunc required but unused)
}
```

### Step 6: Add Conflict Detection (Optional)

If your feature uses multiple instances (like 6 LED strips), add conflict detection:

1. Add conflict result parameters to share.xml
2. Generate NEO_DetectGPIOConflicts-style JavaScript function
3. Add ParameterSeparator warnings in template.xml

See NeoPixel implementation as reference.

## Known Limitations & Workarounds

### 1. OpenKNXproducer Choose-Block Issue

**Limitation**: OpenKNXproducer does NOT process `<choose>` blocks in `Static/ParameterCalculations`

**Impact**: Cannot use hardware-conditional ParameterCalculations

**Workaround**: Generate 96 flat ParameterCalculations (8 hardware × 6 strips × 2 GPIOs)

### 2. ETS JavaScript Capabilities

**Limitation**: ETS uses old ECMA3 JavaScript (no `const`, `let`, `=>`, modern APIs)

**Impact**: 
- No `console.log()` for debugging
- No array methods like `.find()`, `.filter()`
- Limited string manipulation

**Workaround**: Use ES5-compatible code only, debug via parameter writes

### 3. ParameterSeparator ID Format

**Limitation**: ETS parser requires specific ID format after `PS-`

**Impact**: `PS-manual-warning%C%` generates `PS-manual-warning1` → Parse error

**Workaround**: Use `PS-manualdata%C%` (unique text + %C% pattern) or `PS-001` (numeric)

### 4. Port 0 Semantic Change

**Historical Issue**: Port 0 was treated as "unused" in early versions

**Fix**: Since v1.0.5, **Port 0 is VALID** (e.g., Onboard NeoPixel on RP2040)

**Critical**: Conflict detection must NOT skip Port 0, only skip 10 (Manual) and 15 (Dummy)

### 5. Hardware Change Persistence

**Problem**: ETS preserves parameter values when changing hardware selection

**Impact**: Invalid GPIO ports persist after hardware switch

**Solution**: Implemented Hardware Change Reset ParameterCalculation (resets all 12 ports to Dummy=15)

## Dependencies

- **PowerShell Core** 7.0+ (cross-platform: Windows, macOS, Linux)
- **C++ Compiler** (g++, clang, arm-none-eabi-gcc, xtensa-esp32-*)
- **PlatformIO** (optional, for automatic compiler detection)
- **OpenKNXproducer** (for ETS knxprod generation)

## Version History

- **v0.1** (2024): Initial multi-hardware GPIO template generator
- **v0.2** (2024): Added conflict detection (4-way GPIO collision checking)
- **v0.3** (2024): Implemented hardware change reset functionality
- **v0.4** (2024): Fixed Port 0 semantic (0 now valid, not "unused")
- **v0.5** (2025): Script cleanup, removed TEST MODE sections, optimized for production

## License

Part of the OpenKNX project. See project root for license information.

## Contributing

When adding features or fixing bugs:

1. Test with `-DryRun -VerboseMode` first
2. Verify ETS import after generation (`pwsh scripts/Build-Release.ps1`)
3. Test all 8 hardware variants
4. Check conflict detection works (try duplicate GPIO selections)
5. Verify hardware switch resets ports correctly
6. Update this README with architectural changes

## Support & Resources

- **GitHub Issues**: [OpenKNX/OAM-NeoPixel](https://github.com/OpenKNX/OAM-NeoPixel/issues)
- **Forum**: https://forum.openknx.de
- **Wiki**: https://wiki.openknx.de
- **Discord**: OpenKNX Community Server

## Developer Notes

### Marker Block Pattern

All auto-generated sections use this pattern:

```xml
<!-- BEGIN AUTO-GENERATED: Section Name -->
<!-- Cleaned - Ready for regeneration -->
<!-- END AUTO-GENERATED: Section Name -->
```

**Important**: 
- Marker comments must be **exact match** (case-sensitive)
- "Cleaned" line indicates section is empty and ready
- Script REPLACES everything between BEGIN/END markers
- Never manually edit between markers (changes will be lost!)

### Parameter ID Ranges

| Range | Usage | Location |
|-------|-------|----------|
| 073-080 | Template Data Port Selection (HW0-HW7) | Strip.templ.xml |
| 081-088 | Template Clock Port Selection (HW0-HW7) | Strip.templ.xml |
| 00101-00106 | Shared Data Port Storage (Strip 1-6) | share.xml |
| 00111-00116 | Shared Clock Port Storage (Strip 1-6) | share.xml |
| 00091-00096 | Data Conflict Flags (Strip 1-6) | share.xml |
| 00117-00122 | Clock Conflict Flags (Strip 1-6) | share.xml |
| 0800-0807 | Data Port Copy Calculations (HW0-HW7) | Strip.templ.xml × 6 strips |
| 0890-0897 | Clock Port Copy Calculations (HW0-HW7) | Strip.templ.xml × 6 strips |
| 00060 | Conflict Detection ParameterCalculation | share.xml |
| 00061 | Hardware Change Reset ParameterCalculation | share.xml |
| 4000018 | Hardware Selection Dropdown | share.xml |

### Debug Workflow

1. **Enable verbose mode**: `pwsh scripts/Build-DynamicGPIOTemplate.ps1 -VerboseMode`
2. **Check preprocessor output**: Look for `[DEBUG] Resolved GPIO:` lines
3. **Verify marker detection**: Check `Found marker: ...` messages
4. **Inspect generated XML**: Compare before/after with `git diff`
5. **Test ETS import**: Run full build, import knxprod, check for errors
6. **Validate conflicts**: Set duplicate GPIOs, verify warning appears

---

**Last Updated**: January 2026  
**Maintainer**: OpenKNX Project - Erkan Çolak
