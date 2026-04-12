# OAM-NeoPixel LED Behaviour

This document describes the LED status codes specific to the OAM-NeoPixel module.
All NeoPixel diagnostics are shown exclusively on the **STATUS LED**.

## Design

| Condition | LED | Colour | Effect | Identifiable? |
|-----------|-----|--------|--------|---------------|
| **Error** | STATUS | Red | `errorCode(N)` — N× blink, pause, repeat | Yes — count the blinks |
| **Warning** | STATUS | Orange / Yellow | `pulsing()` — smooth sine-wave breathing | By colour only — check serial log for details |

Errors take visual priority over warnings. If both exist, only the error is shown
(the warning is still logged to serial).

## LED Priority System (OpenKNX Framework)

The STATUS LED follows the OpenKNX priority chain. Higher priority always wins:

| Priority | Mode | Used by |
|----------|------|---------|
| 1 | PowerSave (LED off) | Flash save operations |
| **2** | **errorCode(N)** | **NeoPixel errors (this module)** |
| 3 | Heartbeat | Debug heartbeat (if enabled) |
| 4 | ForceOn | KNX programming mode |
| **5** | **pulsing()** | **NeoPixel warnings (this module)** |

## Severity Logic

When multiple errors or warnings occur, only the **most severe** is displayed:

- Lower code number = higher severity (code 1 wins over code 3)
- An active error (Prio 2) suppresses all warnings (Prio 5) on the same LED
- Codes persist until device restart

---

## Error Codes (STATUS LED, Red, countable blinks)

These indicate critical configuration or hardware failures that prevent normal operation.

| Code | Blinks | Condition | Description | Resolution |
|------|--------|-----------|-------------|------------|
| `NEO_ERROR_HW_MISMATCH` | 1× | **Hardware Mismatch** | Firmware compiled for different hardware than ETS configuration. GPIO port mappings from ETS are ignored to prevent damage. | Flash correct firmware, or update hardware selection in ETS (use "Hardware auto-detect"). |
| `NEO_ERROR_NO_HW_CONFIGURED` | 2× | **No Hardware Configured** | ETS has dummy value (255) — no board selected yet. | Open ETS and select correct hardware or use "Hardware auto-detect". |
| `NEO_ERROR_GPIO_CONFLICT` | 3× | **GPIO Pin Conflict** | Multiple strips or relays share the same GPIO pin. Conflicting strips are skipped. | Check ETS strip and relay configuration for duplicate GPIO assignments. |
| `NEO_ERROR_STRIP_FAILED` | 4× | **Strip Creation Failed** | Physical LED strip could not be created (nullptr). Memory exhaustion or driver failure. | Reduce strip/LED count. Check SPI configuration. |
| `NEO_ERROR_VSTRIP_FAILED` | 5× | **Virtual Strip Failed** | Virtual strip could not be created — no LED output possible. | Ensure at least one physical strip was created. Check serial log. |

---

## Warning Codes (STATUS LED, pulsing / breathing)

These indicate non-critical issues. The device continues to operate with limitations.
Warnings use a smooth breathing effect — the colour indicates the type of warning.
For the exact warning, connect a serial console.

| Code | Colour | Condition | Description | Resolution |
|------|--------|-----------|-------------|------------|
| `NEO_WARN_ABL_ACTIVE` | Orange | **Auto Brightness Limiting** | Power consumption >= 95% of limit. Brightness is being reduced. | Increase power limit, reduce LEDs/brightness, or upgrade power supply. |
| `NEO_WARN_FLASH_DISCARDED` | Purple | **Flash Data Discarded** | Saved states don't match current config (e.g., segment count changed). Default startup behaviour used. | Expected after ETS config changes. Clears after next save cycle. |
| `NEO_WARN_UNKNOWN_EFFECT` | Yellow | **Unknown Effect Type** | Effect configured in ETS not recognized by firmware. | Update firmware or change effect type in ETS. |

---

## Quick Identification Guide

| What you see on STATUS LED | Meaning |
|----------------------------|---------|
| **Red, 1× blink** | Hardware mismatch (firmware vs ETS) |
| **Red, 2× blink** | No hardware selected in ETS |
| **Red, 3× blink** | GPIO pin conflict |
| **Red, 4× blink** | Strip creation failed |
| **Red, 5× blink** | Virtual strip creation failed |
| **Orange, breathing** | Power limit active |
| **Purple, breathing** | Flash data discarded (config changed) |
| **Yellow, breathing** | Unknown effect type |
| **No indication** | Everything OK |

---

## OpenKNX Framework Fatal Errors (PROG LED, for reference)

These are defined by OGM-Common, not by OAM-NeoPixel. They use the **PROG LED** (Red)
and **halt the device** completely.

| Code | Blinks | Condition |
|------|--------|-----------|
| `FATAL_FLASH_PARAMETERS` | 2× | Flash parameter error |
| `FATAL_I2C_BUSY` | 3× | I2C bus busy during startup |
| `FATAL_LOG_WRONG_CHANNEL_COUNT` | 4× | knxprod channel count mismatch |
| `FATAL_SENS_UNKNOWN` | 5× | Unknown or unsupported sensor |
| `FATAL_SCHEDULE_MAX_CALLBACKS` | 6× | Too many scheduler callbacks |
| `FATAL_NETWORK` | 7× | Network error |
| `FATAL_INIT_FILESYSTEM` | 10× | LittleFS initialization failed |
| `FATAL_SYSTEM` | 20× | System error (e.g., buffer overrun) |