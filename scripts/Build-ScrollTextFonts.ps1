#Requires -Version 5.1
<#
Open ■
┬────┴  Build-ScrollTextFonts
■ KNX   2024 OpenKNX - Erkan Çolak

FILEPATH: scripts/Build-ScrollTextFonts.ps1

.SYNOPSIS
    Font generator for ScrollTextEffect.

.DESCRIPTION
    Produces lib/OFM-NeoPixel/src/effects/ScrollTextFonts.h with monospace bitmap
    fonts (5x7, 4x6, 3x5), each covering:
      - all 95 printable ASCII chars (0x20..0x7E)
      - a shared block of extended Latin glyphs (German umlauts, C-cedilla, section sign, common
        accents and a few symbols) appended after the ASCII block, fixed order.

    Glyphs are authored as readable ASCII art ('#'=on, '.'=off) so the data is
    reviewable and hard to get wrong. Each glyph is packed COLUMN-MAJOR, one byte
    per column, bit r = row r from the TOP (bit 0 = top row) - matching the
    original kFont5x7 layout used by ScrollTextEffect.

    The packing/emit pipeline is size-generic: add a new size by adding an art
    table and an entry in $Fonts. Get-ScaledGlyph can also derive larger sizes
    from a base font by integer upscaling (e.g. 10x14 = 5x7 x2).

.PARAMETER Preview
    Print every glyph re-rendered from its packed bytes (visual self-check),
    do not write the header.

.EXAMPLE
    pwsh scripts/Build-ScrollTextFonts.ps1
    pwsh scripts/Build-ScrollTextFonts.ps1 -Preview
#>
[CmdletBinding()]
param([switch]$Preview)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# ---------------------------------------------------------------------------
# Extended glyph block - SHARED ORDER across all fonts.
# Index in this list -> glyph index 95+i.
# Keyed by an ASCII MNEMONIC (N) so this script stays pure ASCII and is safe to
# run under Windows PowerShell 5.1 regardless of file encoding. The displayed
# glyph char for the C comments is rebuilt from the codepoint (Cp) at runtime.
# ---------------------------------------------------------------------------
$Ext = @(
    @{ N = 'ae'; Cp = 0x00E4 }, @{ N = 'oe'; Cp = 0x00F6 }, @{ N = 'ue'; Cp = 0x00FC }
    @{ N = 'Ae'; Cp = 0x00C4 }, @{ N = 'Oe'; Cp = 0x00D6 }, @{ N = 'Ue'; Cp = 0x00DC }
    @{ N = 'ss'; Cp = 0x00DF }
    @{ N = 'deg'; Cp = 0x00B0 }, @{ N = 'euro'; Cp = 0x20AC }, @{ N = 'sect'; Cp = 0x00A7 }
    @{ N = 'micro'; Cp = 0x00B5 }, @{ N = 'pound'; Cp = 0x00A3 }
    @{ N = 'eacute'; Cp = 0x00E9 }, @{ N = 'egrave'; Cp = 0x00E8 }, @{ N = 'agrave'; Cp = 0x00E0 }
    @{ N = 'ccedil'; Cp = 0x00E7 }, @{ N = 'Ccedil'; Cp = 0x00C7 }, @{ N = 'ntilde'; Cp = 0x00F1 }
)

# ---------------------------------------------------------------------------
# 5x7 ASCII - reuse the ORIGINAL, already-verified byte table (column-major,
# bit0=top). Source: original kFont5x7 in ScrollTextEffect.h.
# 95 entries, in ASCII order 0x20..0x7E.
# ---------------------------------------------------------------------------
$F5x7AsciiBytes = @(
    @(0x00,0x00,0x00,0x00,0x00), @(0x00,0x00,0x5F,0x00,0x00), @(0x00,0x07,0x00,0x07,0x00)
    @(0x14,0x7F,0x14,0x7F,0x14), @(0x24,0x2A,0x7F,0x2A,0x12), @(0x23,0x13,0x08,0x64,0x62)
    @(0x36,0x49,0x55,0x22,0x50), @(0x00,0x05,0x03,0x00,0x00), @(0x00,0x1C,0x22,0x41,0x00)
    @(0x00,0x41,0x22,0x1C,0x00), @(0x14,0x08,0x3E,0x08,0x14), @(0x08,0x08,0x3E,0x08,0x08)
    @(0x00,0x50,0x30,0x00,0x00), @(0x08,0x08,0x08,0x08,0x08), @(0x00,0x60,0x60,0x00,0x00)
    @(0x20,0x10,0x08,0x04,0x02), @(0x3E,0x51,0x49,0x45,0x3E), @(0x00,0x42,0x7F,0x40,0x00)
    @(0x42,0x61,0x51,0x49,0x46), @(0x21,0x41,0x45,0x4B,0x31), @(0x18,0x14,0x12,0x7F,0x10)
    @(0x27,0x45,0x45,0x45,0x39), @(0x3C,0x4A,0x49,0x49,0x30), @(0x01,0x71,0x09,0x05,0x03)
    @(0x36,0x49,0x49,0x49,0x36), @(0x06,0x49,0x49,0x29,0x1E), @(0x00,0x36,0x36,0x00,0x00)
    @(0x00,0x56,0x36,0x00,0x00), @(0x08,0x14,0x22,0x41,0x00), @(0x14,0x14,0x14,0x14,0x14)
    @(0x00,0x41,0x22,0x14,0x08), @(0x02,0x01,0x51,0x09,0x06), @(0x32,0x49,0x79,0x41,0x3E)
    @(0x7E,0x11,0x11,0x11,0x7E), @(0x7F,0x49,0x49,0x49,0x36), @(0x3E,0x41,0x41,0x41,0x22)
    @(0x7F,0x41,0x41,0x22,0x1C), @(0x7F,0x49,0x49,0x49,0x41), @(0x7F,0x09,0x09,0x09,0x01)
    @(0x3E,0x41,0x49,0x49,0x7A), @(0x7F,0x08,0x08,0x08,0x7F), @(0x00,0x41,0x7F,0x41,0x00)
    @(0x20,0x40,0x41,0x3F,0x01), @(0x7F,0x08,0x14,0x22,0x41), @(0x7F,0x40,0x40,0x40,0x40)
    @(0x7F,0x02,0x0C,0x02,0x7F), @(0x7F,0x04,0x08,0x10,0x7F), @(0x3E,0x41,0x41,0x41,0x3E)
    @(0x7F,0x09,0x09,0x09,0x06), @(0x3E,0x41,0x51,0x21,0x5E), @(0x7F,0x09,0x19,0x29,0x46)
    @(0x46,0x49,0x49,0x49,0x31), @(0x01,0x01,0x7F,0x01,0x01), @(0x3F,0x40,0x40,0x40,0x3F)
    @(0x1F,0x20,0x40,0x20,0x1F), @(0x3F,0x40,0x38,0x40,0x3F), @(0x63,0x14,0x08,0x14,0x63)
    @(0x07,0x08,0x70,0x08,0x07), @(0x61,0x51,0x49,0x45,0x43), @(0x00,0x7F,0x41,0x41,0x00)
    @(0x02,0x04,0x08,0x10,0x20), @(0x00,0x41,0x41,0x7F,0x00), @(0x04,0x02,0x01,0x02,0x04)
    @(0x40,0x40,0x40,0x40,0x40), @(0x00,0x01,0x02,0x04,0x00), @(0x20,0x54,0x54,0x54,0x78)
    @(0x7F,0x48,0x44,0x44,0x38), @(0x38,0x44,0x44,0x44,0x20), @(0x38,0x44,0x44,0x48,0x7F)
    @(0x38,0x54,0x54,0x54,0x18), @(0x08,0x7E,0x09,0x01,0x02), @(0x0C,0x52,0x52,0x52,0x3E)
    @(0x7F,0x08,0x04,0x04,0x78), @(0x00,0x44,0x7D,0x40,0x00), @(0x20,0x40,0x44,0x3D,0x00)
    @(0x7F,0x10,0x28,0x44,0x00), @(0x00,0x41,0x7F,0x40,0x00), @(0x7C,0x04,0x18,0x04,0x78)
    @(0x7C,0x08,0x04,0x04,0x78), @(0x38,0x44,0x44,0x44,0x38), @(0x7C,0x14,0x14,0x14,0x08)
    @(0x08,0x14,0x14,0x18,0x7C), @(0x7C,0x08,0x04,0x04,0x08), @(0x48,0x54,0x54,0x54,0x20)
    @(0x04,0x3F,0x44,0x40,0x20), @(0x3C,0x40,0x40,0x40,0x3C), @(0x1C,0x20,0x40,0x20,0x1C)
    @(0x3C,0x40,0x30,0x40,0x3C), @(0x44,0x28,0x10,0x28,0x44), @(0x0C,0x50,0x50,0x50,0x3C)
    @(0x44,0x64,0x54,0x4C,0x44), @(0x00,0x08,0x36,0x41,0x00), @(0x00,0x00,0x7F,0x00,0x00)
    @(0x00,0x41,0x36,0x08,0x00), @(0x0C,0x02,0x0C,0x00,0x00)
)

# ---------------------------------------------------------------------------
# Helper to build a case-sensitive art table (PowerShell hashtables are
# case-insensitive by default - that would collide 'A' with 'a').
# ---------------------------------------------------------------------------
function New-ArtTable { [System.Collections.Hashtable]::new([System.StringComparer]::Ordinal) }

# 5x7 extended glyphs (width 5, height 7) - umlaut dots on top row.
$F5x7Ext = New-ArtTable
$F5x7Ext['ae']     = @('.#.#.','.....','.###.','...#.','.####','.#..#','.###.')
$F5x7Ext['oe']     = @('.#.#.','.....','.###.','.#..#','.#..#','.#..#','.###.')
$F5x7Ext['ue']     = @('.#.#.','.....','.#..#','.#..#','.#..#','.#..#','.###.')
$F5x7Ext['Ae']     = @('#...#','..#..','.###.','#...#','#####','#...#','#...#')
$F5x7Ext['Oe']     = @('#.#.#','..#..','.###.','#...#','#...#','#...#','.###.')
$F5x7Ext['Ue']     = @('#.#.#','.....','#...#','#...#','#...#','#...#','.###.')
$F5x7Ext['ss']     = @('.###.','#...#','#...#','#.##.','#...#','#...#','#.##.')
$F5x7Ext['deg']    = @('.###.','.#.#.','.###.','.....','.....','.....','.....')
$F5x7Ext['euro']   = @('.####','#....','####.','#....','####.','#....','.####')
$F5x7Ext['sect']   = @('.###.','#...#','.##..','#.#.#','..##.','#...#','.###.')
$F5x7Ext['micro']  = @('.....','.....','#...#','#...#','#...#','#..##','#.#.#')
$F5x7Ext['pound']  = @('..##.','.#..#','.#...','####.','.#...','.#...','#####')
$F5x7Ext['eacute'] = @('..#..','.#...','.###.','#...#','#####','#....','.###.')
$F5x7Ext['egrave'] = @('..#..','...#.','.###.','#...#','#####','#....','.###.')
$F5x7Ext['agrave'] = @('..#..','...#.','.###.','...#.','.####','#...#','.###.')
$F5x7Ext['ccedil'] = @('.....','.....','.###.','#....','#....','.###.','..#..')
$F5x7Ext['Ccedil'] = @('.###.','#...#','#....','#....','#...#','.###.','..##.')
$F5x7Ext['ntilde'] = @('.###.','.....','#.##.','##..#','#...#','#...#','#...#')

# ===========================================================================
# 4x6 font (width 4, height 6). 3-wide glyphs, baseline at row4, descenders row5.
# ===========================================================================
$F4x6 = New-ArtTable
$F4x6[' ']  = @('....','....','....','....','....','....')
$F4x6['!']  = @('.#..','.#..','.#..','.#..','....','.#..')
$F4x6['"']  = @('#.#.','#.#.','....','....','....','....')
$F4x6['#']  = @('#.#.','###.','#.#.','###.','#.#.','....')
$F4x6['$']  = @('.#..','###.','##..','..#.','###.','.#..')
$F4x6['%']  = @('#.#.','..#.','.#..','#...','#.#.','....')
$F4x6['&']  = @('.#..','#.#.','.#..','#.#.','.##.','....')
$F4x6["'"]  = @('.#..','.#..','....','....','....','....')
$F4x6['(']  = @('..#.','.#..','.#..','.#..','..#.','....')
$F4x6[')']  = @('#...','.#..','.#..','.#..','#...','....')
$F4x6['*']  = @('....','#.#.','.#..','#.#.','....','....')
$F4x6['+']  = @('....','.#..','###.','.#..','....','....')
$F4x6[',']  = @('....','....','....','....','.#..','#...')
$F4x6['-']  = @('....','....','###.','....','....','....')
$F4x6['.']  = @('....','....','....','....','.#..','....')
$F4x6['/']  = @('..#.','..#.','.#..','#...','#...','....')
$F4x6['0']  = @('.#..','#.#.','#.#.','#.#.','.#..','....')
$F4x6['1']  = @('.#..','##..','.#..','.#..','###.','....')
$F4x6['2']  = @('##..','..#.','.#..','#...','###.','....')
$F4x6['3']  = @('##..','..#.','.#..','..#.','##..','....')
$F4x6['4']  = @('#.#.','#.#.','###.','..#.','..#.','....')
$F4x6['5']  = @('###.','#...','##..','..#.','##..','....')
$F4x6['6']  = @('.##.','#...','##..','#.#.','.#..','....')
$F4x6['7']  = @('###.','..#.','.#..','.#..','.#..','....')
$F4x6['8']  = @('.#..','#.#.','.#..','#.#.','.#..','....')
$F4x6['9']  = @('.#..','#.#.','.##.','..#.','##..','....')
$F4x6[':']  = @('....','.#..','....','.#..','....','....')
$F4x6[';']  = @('....','.#..','....','.#..','#...','....')
$F4x6['<']  = @('..#.','.#..','#...','.#..','..#.','....')
$F4x6['=']  = @('....','###.','....','###.','....','....')
$F4x6['>']  = @('#...','.#..','..#.','.#..','#...','....')
$F4x6['?']  = @('##..','..#.','.#..','....','.#..','....')
$F4x6['@']  = @('.#..','#.#.','###.','#...','.##.','....')
$F4x6['A']  = @('.#..','#.#.','###.','#.#.','#.#.','....')
$F4x6['B']  = @('##..','#.#.','##..','#.#.','##..','....')
$F4x6['C']  = @('.##.','#...','#...','#...','.##.','....')
$F4x6['D']  = @('##..','#.#.','#.#.','#.#.','##..','....')
$F4x6['E']  = @('###.','#...','##..','#...','###.','....')
$F4x6['F']  = @('###.','#...','##..','#...','#...','....')
$F4x6['G']  = @('.##.','#...','#.#.','#.#.','.##.','....')
$F4x6['H']  = @('#.#.','#.#.','###.','#.#.','#.#.','....')
$F4x6['I']  = @('###.','.#..','.#..','.#..','###.','....')
$F4x6['J']  = @('..#.','..#.','..#.','#.#.','.#..','....')
$F4x6['K']  = @('#.#.','#.#.','##..','#.#.','#.#.','....')
$F4x6['L']  = @('#...','#...','#...','#...','###.','....')
$F4x6['M']  = @('#.#.','###.','###.','#.#.','#.#.','....')
$F4x6['N']  = @('#.#.','##.#','#.#.','#.#.','#.#.','....')
$F4x6['O']  = @('.#..','#.#.','#.#.','#.#.','.#..','....')
$F4x6['P']  = @('##..','#.#.','##..','#...','#...','....')
$F4x6['Q']  = @('.#..','#.#.','#.#.','#.#.','.##.','...#')
$F4x6['R']  = @('##..','#.#.','##..','#.#.','#.#.','....')
$F4x6['S']  = @('.##.','#...','.#..','..#.','##..','....')
$F4x6['T']  = @('###.','.#..','.#..','.#..','.#..','....')
$F4x6['U']  = @('#.#.','#.#.','#.#.','#.#.','.#..','....')
$F4x6['V']  = @('#.#.','#.#.','#.#.','.#..','.#..','....')
$F4x6['W']  = @('#.#.','#.#.','###.','###.','#.#.','....')
$F4x6['X']  = @('#.#.','#.#.','.#..','#.#.','#.#.','....')
$F4x6['Y']  = @('#.#.','#.#.','.#..','.#..','.#..','....')
$F4x6['Z']  = @('###.','..#.','.#..','#...','###.','....')
$F4x6['[']  = @('##..','#...','#...','#...','##..','....')
$F4x6['\']  = @('#...','#...','.#..','..#.','..#.','....')
$F4x6[']']  = @('##..','..#.','..#.','..#.','##..','....')
$F4x6['^']  = @('.#..','#.#.','....','....','....','....')
$F4x6['_']  = @('....','....','....','....','....','###.')
$F4x6['`']  = @('#...','.#..','....','....','....','....')
$F4x6['a']  = @('....','.##.','#.#.','#.#.','.##.','....')
$F4x6['b']  = @('#...','#...','##..','#.#.','##..','....')
$F4x6['c']  = @('....','.##.','#...','#...','.##.','....')
$F4x6['d']  = @('..#.','..#.','.##.','#.#.','.##.','....')
$F4x6['e']  = @('....','.#..','#.#.','##..','.##.','....')
$F4x6['f']  = @('..#.','.#..','###.','.#..','.#..','....')
$F4x6['g']  = @('....','.##.','#.#.','.##.','..#.','##..')
$F4x6['h']  = @('#...','#...','##..','#.#.','#.#.','....')
$F4x6['i']  = @('.#..','....','.#..','.#..','.#..','....')
$F4x6['j']  = @('..#.','....','..#.','..#.','#.#.','.#..')
$F4x6['k']  = @('#...','#.#.','##..','#.#.','#.#.','....')
$F4x6['l']  = @('##..','.#..','.#..','.#..','###.','....')
$F4x6['m']  = @('....','##..','###.','###.','#.#.','....')
$F4x6['n']  = @('....','##..','#.#.','#.#.','#.#.','....')
$F4x6['o']  = @('....','.#..','#.#.','#.#.','.#..','....')
$F4x6['p']  = @('....','##..','#.#.','##..','#...','#...')
$F4x6['q']  = @('....','.##.','#.#.','.##.','..#.','..#.')
$F4x6['r']  = @('....','#.#.','##..','#...','#...','....')
$F4x6['s']  = @('....','.##.','##..','..#.','##..','....')
$F4x6['t']  = @('.#..','###.','.#..','.#..','..#.','....')
$F4x6['u']  = @('....','#.#.','#.#.','#.#.','.##.','....')
$F4x6['v']  = @('....','#.#.','#.#.','.#..','.#..','....')
$F4x6['w']  = @('....','#.#.','###.','###.','#.#.','....')
$F4x6['x']  = @('....','#.#.','.#..','.#..','#.#.','....')
$F4x6['y']  = @('....','#.#.','#.#.','.##.','..#.','##..')
$F4x6['z']  = @('....','###.','.#..','#...','###.','....')
$F4x6['{']  = @('..#.','.#..','##..','.#..','..#.','....')
$F4x6['|']  = @('.#..','.#..','.#..','.#..','.#..','....')
$F4x6['}']  = @('#...','.#..','.##.','.#..','#...','....')
$F4x6['~']  = @('....','.#.#','#.#.','....','....','....')

$F4x6Ext = New-ArtTable
$F4x6Ext['ae']     = @('#.#.','....','##..','#.#.','.##.','....')
$F4x6Ext['oe']     = @('#.#.','....','.#..','#.#.','.#..','....')
$F4x6Ext['ue']     = @('#.#.','....','#.#.','#.#.','.##.','....')
$F4x6Ext['Ae']     = @('#.#.','.#..','#.#.','###.','#.#.','....')
$F4x6Ext['Oe']     = @('#.#.','.#..','#.#.','#.#.','.#..','....')
$F4x6Ext['Ue']     = @('#.#.','....','#.#.','#.#.','.#..','....')
$F4x6Ext['ss']     = @('.#..','#.#.','##..','#.#.','##..','#...')
$F4x6Ext['deg']    = @('.#..','#.#.','.#..','....','....','....')
$F4x6Ext['euro']   = @('.##.','#...','###.','#...','.##.','....')
$F4x6Ext['sect']   = @('.##.','##..','#.#.','.##.','##..','....')
$F4x6Ext['micro']  = @('....','#.#.','#.#.','###.','#...','#...')
$F4x6Ext['pound']  = @('.##.','.#..','###.','.#..','###.','....')
$F4x6Ext['eacute'] = @('..#.','.#..','#.#.','##..','.##.','....')
$F4x6Ext['egrave'] = @('.#..','..#.','#.#.','##..','.##.','....')
$F4x6Ext['agrave'] = @('.#..','..#.','.##.','#.#.','.##.','....')
$F4x6Ext['ccedil'] = @('....','.##.','#...','.##.','..#.','.#..')
$F4x6Ext['Ccedil'] = @('.##.','#...','#...','#...','.##.','.#..')
$F4x6Ext['ntilde'] = @('###.','....','##..','#.#.','#.#.','....')

# ===========================================================================
# 3x5 font (width 3, height 5). Smallest full-ASCII set; M/N/W are compromised.
# ===========================================================================
$F3x5 = New-ArtTable
$F3x5[' ']  = @('...','...','...','...','...')
$F3x5['!']  = @('.#.','.#.','.#.','...','.#.')
$F3x5['"']  = @('#.#','#.#','...','...','...')
$F3x5['#']  = @('#.#','###','#.#','###','#.#')
$F3x5['$']  = @('.#.','###','##.','###','.#.')
$F3x5['%']  = @('#.#','..#','.#.','#..','#.#')
$F3x5['&']  = @('.#.','#.#','.#.','#.#','.##')
$F3x5["'"]  = @('.#.','.#.','...','...','...')
$F3x5['(']  = @('.#.','#..','#..','#..','.#.')
$F3x5[')']  = @('.#.','..#','..#','..#','.#.')
$F3x5['*']  = @('#.#','.#.','###','.#.','#.#')
$F3x5['+']  = @('...','.#.','###','.#.','...')
$F3x5[',']  = @('...','...','...','.#.','#..')
$F3x5['-']  = @('...','...','###','...','...')
$F3x5['.']  = @('...','...','...','...','.#.')
$F3x5['/']  = @('..#','..#','.#.','#..','#..')
$F3x5['0']  = @('###','#.#','#.#','#.#','###')
$F3x5['1']  = @('.#.','##.','.#.','.#.','###')
$F3x5['2']  = @('##.','..#','.#.','#..','###')
$F3x5['3']  = @('##.','..#','.#.','..#','##.')
$F3x5['4']  = @('#.#','#.#','###','..#','..#')
$F3x5['5']  = @('###','#..','##.','..#','##.')
$F3x5['6']  = @('.##','#..','###','#.#','###')
$F3x5['7']  = @('###','..#','.#.','.#.','.#.')
$F3x5['8']  = @('###','#.#','###','#.#','###')
$F3x5['9']  = @('###','#.#','###','..#','##.')
$F3x5[':']  = @('...','.#.','...','.#.','...')
$F3x5[';']  = @('...','.#.','...','.#.','#..')
$F3x5['<']  = @('..#','.#.','#..','.#.','..#')
$F3x5['=']  = @('...','###','...','###','...')
$F3x5['>']  = @('#..','.#.','..#','.#.','#..')
$F3x5['?']  = @('##.','..#','.#.','...','.#.')
$F3x5['@']  = @('.#.','#.#','###','#..','.##')
$F3x5['A']  = @('.#.','#.#','###','#.#','#.#')
$F3x5['B']  = @('##.','#.#','##.','#.#','##.')
$F3x5['C']  = @('.##','#..','#..','#..','.##')
$F3x5['D']  = @('##.','#.#','#.#','#.#','##.')
$F3x5['E']  = @('###','#..','##.','#..','###')
$F3x5['F']  = @('###','#..','##.','#..','#..')
$F3x5['G']  = @('.##','#..','#.#','#.#','.##')
$F3x5['H']  = @('#.#','#.#','###','#.#','#.#')
$F3x5['I']  = @('###','.#.','.#.','.#.','###')
$F3x5['J']  = @('..#','..#','..#','#.#','.#.')
$F3x5['K']  = @('#.#','#.#','##.','#.#','#.#')
$F3x5['L']  = @('#..','#..','#..','#..','###')
$F3x5['M']  = @('#.#','###','###','#.#','#.#')
$F3x5['N']  = @('#.#','##.','#.#','#.#','#.#')
$F3x5['O']  = @('.#.','#.#','#.#','#.#','.#.')
$F3x5['P']  = @('##.','#.#','##.','#..','#..')
$F3x5['Q']  = @('.#.','#.#','#.#','.#.','..#')
$F3x5['R']  = @('##.','#.#','##.','#.#','#.#')
$F3x5['S']  = @('.##','#..','.#.','..#','##.')
$F3x5['T']  = @('###','.#.','.#.','.#.','.#.')
$F3x5['U']  = @('#.#','#.#','#.#','#.#','.#.')
$F3x5['V']  = @('#.#','#.#','#.#','.#.','.#.')
$F3x5['W']  = @('#.#','#.#','###','###','#.#')
$F3x5['X']  = @('#.#','#.#','.#.','#.#','#.#')
$F3x5['Y']  = @('#.#','#.#','.#.','.#.','.#.')
$F3x5['Z']  = @('###','..#','.#.','#..','###')
$F3x5['[']  = @('##.','#..','#..','#..','##.')
$F3x5['\']  = @('#..','#..','.#.','..#','..#')
$F3x5[']']  = @('.##','..#','..#','..#','.##')
$F3x5['^']  = @('.#.','#.#','...','...','...')
$F3x5['_']  = @('...','...','...','...','###')
$F3x5['`']  = @('#..','.#.','...','...','...')
$F3x5['a']  = @('...','.##','#.#','#.#','.##')
$F3x5['b']  = @('#..','#..','##.','#.#','##.')
$F3x5['c']  = @('...','.##','#..','#..','.##')
$F3x5['d']  = @('..#','..#','.##','#.#','.##')
$F3x5['e']  = @('...','.#.','#.#','##.','.##')
$F3x5['f']  = @('..#','.#.','###','.#.','.#.')
$F3x5['g']  = @('...','.##','#.#','.##','##.')
$F3x5['h']  = @('#..','#..','##.','#.#','#.#')
$F3x5['i']  = @('.#.','...','.#.','.#.','.#.')
$F3x5['j']  = @('..#','...','..#','#.#','.#.')
$F3x5['k']  = @('#..','#.#','##.','#.#','#.#')
$F3x5['l']  = @('##.','.#.','.#.','.#.','.##')
$F3x5['m']  = @('...','#.#','###','###','#.#')
$F3x5['n']  = @('...','##.','#.#','#.#','#.#')
$F3x5['o']  = @('...','.#.','#.#','#.#','.#.')
$F3x5['p']  = @('...','##.','#.#','##.','#..')
$F3x5['q']  = @('...','.##','#.#','.##','..#')
$F3x5['r']  = @('...','#.#','##.','#..','#..')
$F3x5['s']  = @('...','.##','##.','..#','##.')
$F3x5['t']  = @('.#.','###','.#.','.#.','..#')
$F3x5['u']  = @('...','#.#','#.#','#.#','.##')
$F3x5['v']  = @('...','#.#','#.#','.#.','.#.')
$F3x5['w']  = @('...','#.#','###','###','#.#')
$F3x5['x']  = @('...','#.#','.#.','.#.','#.#')
$F3x5['y']  = @('...','#.#','#.#','.##','##.')
$F3x5['z']  = @('...','###','.#.','#..','###')
$F3x5['{']  = @('..#','.#.','##.','.#.','..#')
$F3x5['|']  = @('.#.','.#.','.#.','.#.','.#.')
$F3x5['}']  = @('#..','.#.','.##','.#.','#..')
$F3x5['~']  = @('...','.##','#.#','...','...')

$F3x5Ext = New-ArtTable
$F3x5Ext['ae']     = @('#.#','...','###','#.#','.##')
$F3x5Ext['oe']     = @('#.#','...','.#.','#.#','.#.')
$F3x5Ext['ue']     = @('#.#','...','#.#','#.#','.##')
$F3x5Ext['Ae']     = @('#.#','.#.','#.#','###','#.#')
$F3x5Ext['Oe']     = @('#.#','.#.','#.#','#.#','.#.')
$F3x5Ext['Ue']     = @('#.#','...','#.#','#.#','.#.')
$F3x5Ext['ss']     = @('.#.','#.#','##.','#.#','##.')
$F3x5Ext['deg']    = @('.#.','#.#','.#.','...','...')
$F3x5Ext['euro']   = @('.##','#..','###','#..','.##')
$F3x5Ext['sect']   = @('.##','##.','#.#','.##','##.')
$F3x5Ext['micro']  = @('...','#.#','#.#','###','#..')
$F3x5Ext['pound']  = @('.##','.#.','###','.#.','###')
$F3x5Ext['eacute'] = @('..#','.#.','###','##.','.##')
$F3x5Ext['egrave'] = @('.#.','..#','###','##.','.##')
$F3x5Ext['agrave'] = @('.#.','..#','.##','#.#','.##')
$F3x5Ext['ccedil'] = @('...','.##','#..','.##','.#.')
$F3x5Ext['Ccedil'] = @('.##','#..','#..','.##','.#.')
$F3x5Ext['ntilde'] = @('###','...','##.','#.#','#.#')

# ---------------------------------------------------------------------------
# Core packing helpers - fully size-generic.
# ---------------------------------------------------------------------------
function ConvertTo-GlyphBytes {
    param([string[]]$Rows, [int]$Width, [int]$Height)
    $cols = [int[]]::new($Width)
    for ($c = 0; $c -lt $Width; $c++) {
        $b = 0
        for ($r = 0; $r -lt $Height; $r++) {
            $line = if ($r -lt $Rows.Count) { $Rows[$r] } else { '' }
            $line = $line -replace ' ', '.'
            if ($c -lt $line.Length -and $line[$c] -eq '#') { $b = $b -bor (1 -shl $r) }
        }
        $cols[$c] = $b
    }
    , $cols
}

function ConvertTo-GlyphArt {
    param([int[]]$Cols, [int]$Width, [int]$Height)
    for ($r = 0; $r -lt $Height; $r++) {
        $sb = ''
        for ($c = 0; $c -lt $Width; $c++) {
            $sb += if (($Cols[$c] -shr $r) -band 1) { '#' } else { '.' }
        }
        $sb
    }
}

# Integer upscale: derive a larger glyph (Width*Sx, Height*Sy) from a base glyph.
function Get-ScaledGlyph {
    param([int[]]$Cols, [int]$Width, [int]$Height, [int]$Sx, [int]$Sy)
    $newW = $Width * $Sx
    $out = [int[]]::new($newW)
    for ($c = 0; $c -lt $Width; $c++) {
        $col = 0
        for ($r = 0; $r -lt $Height; $r++) {
            if (($Cols[$c] -shr $r) -band 1) {
                for ($k = 0; $k -lt $Sy; $k++) { $col = $col -bor (1 -shl ($r * $Sy + $k)) }
            }
        }
        for ($j = 0; $j -lt $Sx; $j++) { $out[$c * $Sx + $j] = $col }
    }
    , $out
}

# Build a full glyph list (95 ASCII + extended) for one font size.
function Build-Font {
    param($AsciiArt, $AsciiBytes, $ExtArt, [int]$Width, [int]$Height)
    $glyphs = [System.Collections.Generic.List[object]]::new()
    for ($cp = 0x20; $cp -le 0x7E; $cp++) {
        $ch = [string][char]$cp
        if ($null -ne $AsciiBytes) {
            $glyphs.Add([int[]]$AsciiBytes[$cp - 0x20])
        }
        else {
            if (-not $AsciiArt.ContainsKey($ch)) { throw "Missing ASCII glyph U+$('{0:X4}' -f $cp) '$ch' for ${Width}x${Height}" }
            $glyphs.Add((ConvertTo-GlyphBytes -Rows $AsciiArt[$ch] -Width $Width -Height $Height))
        }
    }
    foreach ($e in $Ext) {
        $ch = $e.N
        if (-not $ExtArt.ContainsKey($ch)) { throw "Missing EXT glyph '$ch' for ${Width}x${Height}" }
        $glyphs.Add((ConvertTo-GlyphBytes -Rows $ExtArt[$ch] -Width $Width -Height $Height))
    }
    , $glyphs
}

# ---------------------------------------------------------------------------
# Font registry - add a row here to add a size.
# ---------------------------------------------------------------------------
$Fonts = @(
    @{ Name = 'kScrollFont5x7'; Width = 5; Height = 7; AsciiArt = $null;  AsciiBytes = $F5x7AsciiBytes; ExtArt = $F5x7Ext; Comment = '0: 5x7' }
    @{ Name = 'kScrollFont4x6'; Width = 4; Height = 6; AsciiArt = $F4x6;  AsciiBytes = $null;           ExtArt = $F4x6Ext; Comment = '1: 4x6' }
    @{ Name = 'kScrollFont3x5'; Width = 3; Height = 5; AsciiArt = $F3x5;  AsciiBytes = $null;           ExtArt = $F3x5Ext; Comment = '2: 3x5' }
)

$labels = @(); for ($cp = 0x20; $cp -le 0x7E; $cp++) { $labels += [string][char]$cp }
# Rebuild the real glyph char from the codepoint (keeps source pure ASCII).
foreach ($e in $Ext) { $labels += [System.Char]::ConvertFromUtf32($e.Cp) }

$built = foreach ($f in $Fonts) {
    @{ Meta = $f; Glyphs = (Build-Font -AsciiArt $f.AsciiArt -AsciiBytes $f.AsciiBytes -ExtArt $f.ExtArt -Width $f.Width -Height $f.Height) }
}

$glyphCount = $built[0].Glyphs.Count
foreach ($b in $built) {
    if ($b.Glyphs.Count -ne $glyphCount) { throw "Glyph count mismatch in $($b.Meta.Name)" }
}

if ($Preview) {
    foreach ($b in $built) {
        Write-Host "`n===== $($b.Meta.Name) ($($b.Meta.Width)x$($b.Meta.Height)) ====="
        for ($i = 0; $i -lt $b.Glyphs.Count; $i++) {
            Write-Host "[$($labels[$i])]"
            foreach ($ln in (ConvertTo-GlyphArt -Cols $b.Glyphs[$i] -Width $b.Meta.Width -Height $b.Meta.Height)) {
                Write-Host "  $ln"
            }
        }
    }
    return
}

# ---------------------------------------------------------------------------
# Emit the C++ header.
# ---------------------------------------------------------------------------
function Format-Array {
    param($Name, $Glyphs, $Width)
    $sb = [System.Text.StringBuilder]::new()
    [void]$sb.AppendLine("static const uint8_t $Name[] PROGMEM = {")
    for ($i = 0; $i -lt $Glyphs.Count; $i++) {
        $vals = ($Glyphs[$i] | ForEach-Object { '0x{0:X2}' -f $_ }) -join ','
        $lbl = $labels[$i]
        if ($lbl -eq '\') { $lbl = 'backslash' }
        [void]$sb.AppendLine(("    {0}, // {1,3} '{2}'" -f $vals, $i, $lbl))
    }
    [void]$sb.AppendLine('};')
    $sb.ToString()
}

$extCps = ($Ext | ForEach-Object { '0x{0:X4}' -f $_.Cp }) -join ','

$out = [System.Text.StringBuilder]::new()
[void]$out.AppendLine('/**')
[void]$out.AppendLine(' * @file ScrollTextFonts.h')
[void]$out.AppendLine(' * @brief Bitmap fonts for ScrollTextEffect (5x7, 4x6, 3x5).')
[void]$out.AppendLine(' *')
[void]$out.AppendLine(' * AUTO-GENERATED by scripts/Build-ScrollTextFonts.ps1 - do not edit by hand.')
[void]$out.AppendLine(' * Each glyph is column-major, one byte per column, bit 0 = top row.')
[void]$out.AppendLine(" * Glyph order: ASCII 0x20..0x7E (indices 0..94), then the extended")
[void]$out.AppendLine(" * Latin block below (indices 95..$($glyphCount-1)).")
[void]$out.AppendLine(' *')
[void]$out.AppendLine(" * @copyright Copyright (c) 2025 Erkan $([System.Char]::ConvertFromUtf32(0x00C7))olak - OpenKNX (GNU GPL v3.0)")
[void]$out.AppendLine(' */')
[void]$out.AppendLine('#pragma once')
[void]$out.AppendLine('#include <stdint.h>')
[void]$out.AppendLine('')
[void]$out.AppendLine('#if defined(ARDUINO) || defined(ESP_PLATFORM)')
[void]$out.AppendLine('#include <Arduino.h>')
[void]$out.AppendLine('#else')
[void]$out.AppendLine('#ifndef PROGMEM')
[void]$out.AppendLine('#define PROGMEM')
[void]$out.AppendLine('#endif')
[void]$out.AppendLine('#ifndef pgm_read_byte')
[void]$out.AppendLine('#define pgm_read_byte(addr) (*(const uint8_t*)(addr))')
[void]$out.AppendLine('#endif')
[void]$out.AppendLine('#endif')
[void]$out.AppendLine('')
[void]$out.AppendLine("static const uint8_t kScrollFontGlyphCount = $glyphCount;")
[void]$out.AppendLine('static const uint8_t kScrollFontFirstAscii = 0x20; // first ASCII glyph')
[void]$out.AppendLine('static const uint8_t kScrollFontLastAscii  = 0x7E; // last ASCII glyph')
[void]$out.AppendLine("static const uint8_t kScrollFontExtCount    = $($Ext.Count);")
[void]$out.AppendLine('')
[void]$out.AppendLine('// Unicode codepoints for the extended glyph block (index i -> glyph 95+i)')
[void]$out.AppendLine("static const uint16_t kScrollFontExtCodepoints[$($Ext.Count)] PROGMEM = { $extCps };")
[void]$out.AppendLine('')
foreach ($b in $built) {
    [void]$out.AppendLine((Format-Array -Name $b.Meta.Name -Glyphs $b.Glyphs -Width $b.Meta.Width))
}
[void]$out.AppendLine('// Font descriptor: pick by index via ScrollTextEffect ''Font'' parameter')
[void]$out.AppendLine('struct ScrollFontDesc { const uint8_t* data; uint8_t width; uint8_t height; };')
[void]$out.AppendLine("static const ScrollFontDesc kScrollFonts[$($Fonts.Count)] = {")
foreach ($f in $Fonts) {
    [void]$out.AppendLine("    { $($f.Name), $($f.Width), $($f.Height) }, // $($f.Comment)")
}
[void]$out.AppendLine('};')
[void]$out.AppendLine("static const uint8_t kScrollFontCount = $($Fonts.Count);")

# NOTE: Join-Path with multiple child segments needs PS6+; Path.Combine works on 5.1.
$outPath = [System.IO.Path]::Combine($PSScriptRoot, '..', 'lib', 'OFM-NeoPixel', 'src', 'effects', 'ScrollTextFonts.h')
$outPath = [System.IO.Path]::GetFullPath($outPath)
[System.IO.File]::WriteAllText($outPath, $out.ToString(), [System.Text.UTF8Encoding]::new($false))
Write-Host "wrote $outPath ($glyphCount glyphs per font, $($Fonts.Count) sizes)"
