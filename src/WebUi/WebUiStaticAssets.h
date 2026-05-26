#pragma once

static const char kNeoPixelWebUiIndexHtml[] = R"NPXHTML(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="theme-color" content="#181a24">
    <title>OAM NeoPixel Studio</title>
    <link rel="stylesheet" href="/neopixel/app.css">
</head>
<body>
    <div class="app-shell">
        <header class="hero panel reveal">
            <div class="hero-meta">
                <div class="status-pill online" id="connectionState">Connected</div>
                <div class="status-pill" id="persistenceState">Runtime focus</div>
            </div>

            <div class="hero-utility">
                <div class="hero-nav-column">
                    <div class="section-tabs" aria-label="Workspace tabs" role="tablist">
                        <button type="button" class="is-active" id="tab-color" role="tab" aria-selected="true" aria-controls="section-color" data-panel="section-color">Colour</button>
                        <button type="button" id="tab-effects" role="tab" aria-selected="false" aria-controls="section-effects" data-panel="section-effects">Effects &amp; Scenes</button>
                    </div>

                    <div class="segment-strip-head">
                        <div>
                            <p class="panel-kicker">Segments</p>
                            <strong>Choose active segment</strong>
                        </div>
                        <span class="segment-selection-badge" id="segmentSelectionBadge">Active: Segment 1</span>
                    </div>

                    <div class="segment-strip" id="segmentStrip" aria-label="Segments"></div>
                </div>

                <div class="hero-sync-dock" aria-label="Sync Dock">
                    <div class="hero-sync-grid">
                        <div class="sync-action-panel">
                            <div class="action-grid">
                                <button type="button" class="primary" id="applyRuntimeButton">Apply live</button>
                                <button type="button" class="secondary" id="savePersistedButton">Save to device</button>
                                <button type="button" class="ghost" id="refreshStateButton">Pull state</button>
                                <button type="button" class="ghost" id="resetDraftButton">Reset draft</button>
                            </div>

                            <div class="state-card" id="draftStateCard">
                                <span class="label">Draft status</span>
                                <strong id="draftStatus">Clean</strong>
                                <p id="draftSummary">No pending changes.</p>
                            </div>
                        </div>

                        <div class="payload-panel">
                            <div class="payload-head">
                                <span class="label">Patch preview</span>
                                <button type="button" class="chip-button" id="payloadModeButton">Runtime patch</button>
                            </div>
                            <pre id="payloadPreview"></pre>
                        </div>
                    </div>
                </div>
            </div>
        </header>

        <div class="workspace">
            <section class="main-column">
                <article class="panel reveal tab-panel is-active" id="section-color" role="tabpanel" aria-labelledby="tab-color">
                    <div class="panel-head">
                        <div>
                            <p class="panel-kicker">Live Color Control</p>
                        </div>
                    </div>

                    <div class="color-layout">
                        <div class="picker-stack">
                            <div class="picker-stage">
                                <div class="picker-frame">
                                    <canvas id="svCanvas" width="320" height="320" aria-label="Saturation and value picker"></canvas>
                                    <div class="picker-crosshair" id="pickerCrosshair"></div>
                                </div>

                                <div class="color-target-stack" role="group" aria-label="Primary and secondary colour pickers">
                                    <label class="native-color-field is-active" id="primaryColorField">
                                        <input id="rgbColorInput" type="color" value="#ffffff" aria-label="Primary colour picker">
                                        <span>Primary</span>
                                    </label>

                                    <label class="native-color-field" id="secondaryColorField">
                                        <input id="secondaryColorInput" type="color" value="#000000" aria-label="Secondary colour picker">
                                        <span>Secondary</span>
                                    </label>
                                </div>
                            </div>

                            <label class="control-block accent-slider">
                                <span>Hue</span>
                                <input id="hueSlider" type="range" min="0" max="360" value="0">
                            </label>

                            <div class="swatch-toolbar">
                                <p class="panel-kicker">Quick Colours</p>
                                <button type="button" class="chip-button swatch-mode-button" id="saveSwatchButton" aria-pressed="false">Save current</button>
                            </div>

                            <div class="swatch-grid" id="swatchGrid" aria-label="Suggested colors"></div>
                        </div>

                        <div class="preview-stack">
                            <div class="preview-card">
                                <div class="preview-header">
                                    <div>
                                        <p class="panel-kicker">Selected Segment</p>
                                        <h3 id="segmentName">Segment 1</h3>
                                    </div>
                                    <div class="preview-dot" id="colorPreview"></div>
                                </div>

                                <div class="preview-readout">
                                    <div>
                                        <span class="label">Effect</span>
                                        <strong id="activeEffectName">Solid</strong>
                                    </div>
                                    <div>
                                        <span class="label" id="activeColorLabel">Primary colour</span>
                                        <strong id="activeColorHex">#ffffff</strong>
                                    </div>
                                    <div>
                                        <span class="label">Brightness</span>
                                        <strong id="activeBrightness">128</strong>
                                    </div>
                                </div>

                                <div class="mini-preview" id="miniPreviewBar"></div>
                            </div>

                            <div class="color-entry-card">
                                <div class="color-entry-head">
                                    <div>
                                        <p class="panel-kicker">Direct Input</p>
                                        <h3>RGB Picker</h3>
                                    </div>
                                </div>

                                <label class="text-control">
                                    <span>HEX</span>
                                    <input id="hexInput" type="text" inputmode="text" spellcheck="false" maxlength="7" placeholder="#FFFFFF">
                                </label>

                                <div class="rgb-field-grid">
                                    <label class="text-control rgb-channel">
                                        <span>R</span>
                                        <input id="redInput" type="number" min="0" max="255" step="1" inputmode="numeric">
                                    </label>

                                    <label class="text-control rgb-channel">
                                        <span>G</span>
                                        <input id="greenInput" type="number" min="0" max="255" step="1" inputmode="numeric">
                                    </label>

                                    <label class="text-control rgb-channel">
                                        <span>B</span>
                                        <input id="blueInput" type="number" min="0" max="255" step="1" inputmode="numeric">
                                    </label>
                                </div>
                            </div>

                            <div class="control-grid">
                                <label class="control-block">
                                    <span>Brightness</span>
                                    <input id="brightnessSlider" type="range" min="1" max="255" value="160">
                                    <output id="brightnessValue">160</output>
                                </label>

                                <label class="control-block">
                                    <span>White mix</span>
                                    <input id="whiteSlider" type="range" min="0" max="255" value="0">
                                    <output id="whiteValue">0</output>
                                </label>

                                <label class="control-block">
                                    <span>Temperature</span>
                                    <input id="temperatureSlider" type="range" min="1900" max="9000" value="4200">
                                    <output id="temperatureValue">4200 K</output>
                                </label>

                                <label class="toggle-block">
                                    <span>Segment power</span>
                                    <button type="button" class="toggle-button" id="powerToggle" aria-pressed="true">On</button>
                                </label>
                            </div>
                        </div>
                    </div>
                </article>

                <article class="panel reveal tab-panel" id="section-effects" role="tabpanel" aria-labelledby="tab-effects" hidden>
                    <div class="panel-head">
                        <div>
                            <h2>Effects &amp; Scenes</h2>
                        </div>
                    </div>

                    <div class="effect-layout">
                        <div class="effect-browser">
                            <div class="parameter-card effect-selector-card">
                                <div>
                                    <strong>Choose effect</strong>
                                </div>

                                <label class="text-control">
                                    <span>Effect</span>
                                    <select id="effectSelect" aria-label="Select effect"></select>
                                </label>
                            </div>

                            <div class="parameter-card effect-scene-card">
                                <div>
                                    <strong>Store current draft</strong>
                                </div>

                                <label class="text-control">
                                    <span>Scene slot</span>
                                    <select id="effectSceneSelect" aria-label="Select scene slot"></select>
                                </label>

                                <div class="scene-actions effect-scene-actions">
                                    <button type="button" class="secondary" id="effectSceneStoreButton">Assign to scene</button>
                                </div>
                            </div>
                        </div>

                        <div class="effect-detail">
                            <div class="parameter-grid" id="parameterGrid"></div>
                        </div>
                    </div>

                    <div class="effect-scenes-bar">
                        <div class="effect-scenes-summary">
                            <span class="label">Scenes</span>
                            <strong id="effectScenesSummary">No scenes configured</strong>
                        </div>
                        <button type="button" class="chip-button effect-scenes-toggle" id="sceneSectionToggleButton" aria-expanded="false" aria-controls="sceneGrid">Show scenes</button>
                    </div>

                    <div class="scene-grid effect-scene-grid" id="sceneGrid"></div>
                </article>
            </section>
        </div>
    </div>

    <script src="/neopixel/app.js"></script>
</body>
</html>
)NPXHTML";

static const char kNeoPixelWebUiAppCss[] = R"NPXCSS(
:root {
    --bg: #141722;
    --bg-soft: #1d2230;
    --panel: rgba(17, 21, 31, 0.84);
    --panel-strong: rgba(24, 28, 40, 0.96);
    --line: rgba(255, 255, 255, 0.08);
    --line-strong: rgba(255, 255, 255, 0.16);
    --text: #f4f1e8;
    --text-dim: #b8becc;
    --text-soft: #8a90a3;
    --accent: #f06b4f;
    --accent-2: #1ec7c1;
    --accent-3: #eac95b;
    --success: #5dd18f;
    --warning: #f29a49;
    --danger: #f46262;
    --shadow: 0 32px 80px rgba(0, 0, 0, 0.38);
    --radius-xl: 28px;
    --radius-lg: 22px;
    --radius-md: 16px;
    --radius-sm: 12px;
    --font-ui: "Avenir Next", "Segoe UI", "Trebuchet MS", sans-serif;
    --font-code: "Consolas", "SFMono-Regular", monospace;
}

* {
    box-sizing: border-box;
}

html {
    scroll-behavior: smooth;
}

body {
    margin: 0;
    min-height: 100vh;
    font-family: var(--font-ui);
    color: var(--text);
    background:
        radial-gradient(circle at top left, rgba(240, 107, 79, 0.2), transparent 28%),
        radial-gradient(circle at top right, rgba(30, 199, 193, 0.16), transparent 24%),
        linear-gradient(135deg, #0e1119 0%, #131826 48%, #1d2231 100%);
}

body::before {
    content: "";
    position: fixed;
    inset: 0;
    pointer-events: none;
    background-image:
        linear-gradient(rgba(255, 255, 255, 0.03) 1px, transparent 1px),
        linear-gradient(90deg, rgba(255, 255, 255, 0.03) 1px, transparent 1px);
    background-size: 40px 40px;
    mask-image: radial-gradient(circle at center, black, transparent 82%);
}

.app-shell {
    width: 100%;
    margin: 16px 0 28px;
}

.workspace {
    display: grid;
    grid-template-columns: minmax(0, 1fr);
    gap: 20px;
    align-items: start;
}

.main-column,
.side-column {
    display: grid;
    gap: 20px;
}

.panel {
    position: relative;
    overflow: hidden;
    border: 1px solid var(--line);
    border-radius: var(--radius-xl);
    background: linear-gradient(180deg, rgba(255, 255, 255, 0.02), transparent 24%), var(--panel);
    box-shadow: var(--shadow);
    backdrop-filter: blur(20px);
}

.panel::after {
    content: "";
    position: absolute;
    inset: 0;
    border-radius: inherit;
    pointer-events: none;
    background: linear-gradient(135deg, rgba(255, 255, 255, 0.08), transparent 32%);
}

.reveal {
    animation: rise-in 0.7s ease both;
}

.reveal:nth-child(2) {
    animation-delay: 0.08s;
}

.reveal:nth-child(3) {
    animation-delay: 0.16s;
}

@keyframes rise-in {
    from {
        opacity: 0;
        transform: translateY(16px);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.hero {
    display: grid;
    gap: 10px;
    padding: 14px 16px 14px;
    margin-bottom: 12px;
}

.hero-head {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    align-items: center;
}

.hero-copy {
    max-width: 640px;
}

.eyebrow,
.panel-kicker,
.label {
    margin: 0;
    text-transform: uppercase;
    letter-spacing: 0.18em;
    font-size: 0.73rem;
    color: var(--text-soft);
}

.hero h1,
.panel h2,
.panel h3 {
    margin: 0;
    font-weight: 700;
    letter-spacing: -0.03em;
}

.hero h1 {
    margin-top: 0;
    font-size: clamp(1.55rem, 3vw, 2.2rem);
    line-height: 1;
}

.lede,
.panel-copy,
.state-card p,
.effect-card p,
.scene-card p {
    color: var(--text-dim);
    line-height: 1.55;
}

.lede {
    margin: 8px 0 0;
    max-width: 50ch;
    font-size: 0.93rem;
}

.hero-meta {
    display: flex;
    flex-wrap: wrap;
    gap: 8px;
    margin: 0;
    justify-content: flex-end;
}

.hero-utility {
    display: grid;
    grid-template-columns: minmax(300px, 0.92fr) minmax(0, 1.08fr);
    gap: 8px 12px;
    align-items: start;
}

.hero-nav-column {
    display: grid;
    gap: 8px;
    min-width: 0;
    align-content: start;
}

.status-pill,
.segment-chip,
.chip-button,
.scene-tag,
.context-chip {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    min-height: 30px;
    padding: 0 10px;
    border-radius: 999px;
    border: 1px solid var(--line-strong);
    background: rgba(255, 255, 255, 0.05);
    color: var(--text);
    font-size: 0.84rem;
}

.status-pill.online::before,
.status-pill.accent::before {
    content: "";
    width: 8px;
    height: 8px;
    border-radius: 999px;
}

.status-pill.online::before {
    background: var(--success);
}

.status-pill.accent::before {
    background: var(--accent-2);
}

.section-tabs {
    display: flex;
    flex-wrap: wrap;
    gap: 8px;
}

.section-tabs button,
.chip-button,
.toggle-button,
.action-grid button,
.scene-actions button {
    cursor: pointer;
    border: 0;
    color: inherit;
    font: inherit;
}

.section-tabs button {
    min-height: 32px;
    padding: 0 12px;
    border-radius: 999px;
    border: 1px solid var(--line);
    background: rgba(255, 255, 255, 0.05);
    color: var(--text-dim);
    transition: background 0.2s ease, color 0.2s ease, transform 0.2s ease;
}

.section-tabs button.is-active {
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.22), rgba(30, 199, 193, 0.14));
    border-color: rgba(240, 107, 79, 0.35);
    color: var(--text);
}

.section-tabs button:hover,
.section-tabs button:focus-visible {
    background: rgba(240, 107, 79, 0.18);
    color: var(--text);
    transform: translateY(-1px);
}

.hero-sync-dock {
    min-width: 0;
}

.hero-sync-grid {
    display: grid;
    grid-template-columns: minmax(280px, 1fr) minmax(280px, 1.02fr);
    gap: 8px;
    align-items: start;
}

.hero-sync-grid > * {
    min-width: 0;
}

.segment-strip-head {
    display: flex;
    justify-content: space-between;
    gap: 10px;
    align-items: center;
}

.segment-strip-head > div {
    display: flex;
    align-items: center;
    gap: 8px;
    flex-wrap: wrap;
}

.segment-strip-head .panel-kicker {
    display: none;
}

.segment-strip-head strong {
    display: block;
    margin-top: 0;
    font-size: 0.88rem;
    font-weight: 600;
    letter-spacing: 0;
    color: var(--text-soft);
}

.segment-selection-badge {
    display: inline-flex;
    align-items: center;
    min-height: 28px;
    padding: 0 10px;
    border-radius: 999px;
    border: 1px solid var(--line-strong);
    background: rgba(255, 255, 255, 0.04);
    color: var(--text-dim);
    font-size: 0.82rem;
}

.hero-sync-grid .payload-panel,
.hero-sync-grid .sync-action-panel {
    height: 100%;
}

.hero-sync-grid .sync-action-panel {
    display: grid;
    gap: 8px;
    padding: 12px;
    border-radius: var(--radius-lg);
    border: 1px solid var(--line);
    background: rgba(255, 255, 255, 0.04);
    align-content: start;
}

.hero-sync-grid .payload-panel,
.hero-sync-grid .sync-action-panel {
    padding: 12px;
}

.hero-sync-grid .sync-action-panel .state-card {
    padding: 12px;
}

.hero-sync-grid .state-card p {
    margin: 6px 0 0;
}

.hero-sync-grid .payload-panel pre {
    margin-top: 8px;
    max-height: 132px;
    min-height: 96px;
    padding: 10px;
}

.segment-strip {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 8px;
    margin-top: 2px;
}

.segment-strip > :only-child {
    grid-column: 1 / -1;
}

.segment-chip {
    width: 100%;
    justify-content: space-between;
    min-height: 44px;
    padding: 8px 10px;
    text-align: left;
    border-radius: var(--radius-md);
    background: rgba(255, 255, 255, 0.04);
    transition: border-color 0.2s ease, transform 0.2s ease, background 0.2s ease;
}

.segment-chip strong {
    display: block;
    font-size: 0.88rem;
}

.segment-chip span {
    color: var(--text-soft);
    font-size: 0.76rem;
}

.segment-chip.active {
    border-color: rgba(240, 107, 79, 0.45);
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.18), rgba(30, 199, 193, 0.1));
}

.segment-chip:hover,
.segment-chip:focus-visible {
    transform: translateY(-2px);
}

.panel-head {
    display: flex;
    justify-content: space-between;
    gap: 16px;
    align-items: start;
    padding: 20px 20px 0;
}

.panel-head.compact {
    padding-bottom: 10px;
}

.panel-head h2 {
    margin-top: 5px;
    font-size: clamp(1.35rem, 2.7vw, 1.95rem);
}

.panel-copy {
    max-width: 36ch;
    margin: 0;
    font-size: 0.9rem;
}

.color-layout,
.effect-layout {
    display: grid;
    gap: 18px;
    padding: 18px 20px 20px;
}

.tab-panel[hidden] {
    display: none;
}

.color-layout {
    grid-template-columns: minmax(0, 1.08fr) minmax(320px, 0.92fr);
    align-items: start;
}

.picker-stack,
.preview-stack,
.dock-stack,
.effect-detail {
    display: grid;
    gap: 18px;
}

.picker-stage {
    display: grid;
    grid-template-columns: minmax(0, 360px) auto;
    gap: 16px;
    align-items: start;
    justify-content: center;
}

.color-target-stack {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(84px, 92px));
    gap: 14px;
    align-content: start;
    justify-content: start;
    justify-items: start;
    min-width: 0;
}

.dock-stack {
    padding: 0 20px 20px;
}

.picker-frame {
    position: relative;
    padding: 14px;
    width: 100%;
    max-width: 360px;
    margin: 0 auto;
    border-radius: calc(var(--radius-lg) + 2px);
    background: linear-gradient(180deg, rgba(255, 255, 255, 0.05), rgba(255, 255, 255, 0.02));
    border: 1px solid var(--line);
}

.picker-stage .picker-frame,
.picker-stage #svCanvas {
    margin: 0;
}

#svCanvas {
    width: 100%;
    max-width: 332px;
    margin: 0 auto;
    display: block;
    border-radius: var(--radius-lg);
    cursor: crosshair;
}

.picker-crosshair {
    position: absolute;
    width: 18px;
    height: 18px;
    border-radius: 999px;
    border: 2px solid white;
    box-shadow: 0 0 0 2px rgba(0, 0, 0, 0.25);
    pointer-events: none;
    transform: translate(-50%, -50%);
}

.control-grid,
.parameter-grid,
.system-grid,
.rgb-field-grid,
.scene-grid {
    display: grid;
    gap: 14px;
}

.parameter-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
    align-content: start;
}

.control-grid {
    grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
}

.rgb-field-grid {
    grid-template-columns: repeat(3, minmax(0, 1fr));
    gap: 10px;
}

.control-block,
.toggle-block,
.color-entry-card,
.parameter-card,
.system-card,
.scene-card,
.effect-hero,
.preview-card,
.payload-panel,
.state-card,
.effect-card {
    position: relative;
    padding: 16px;
    border-radius: var(--radius-lg);
    border: 1px solid var(--line);
    background: rgba(255, 255, 255, 0.04);
}

.control-block span,
.toggle-block span,
.text-control span {
    display: block;
    margin-bottom: 10px;
    font-size: 0.9rem;
    color: var(--text-dim);
}

.color-entry-card {
    display: grid;
    gap: 14px;
}

.state-card.is-pending {
    border-color: rgba(94, 208, 161, 0.42);
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.14), rgba(94, 208, 161, 0.08));
}

.state-card.is-success {
    border-color: rgba(104, 214, 161, 0.46);
    background: linear-gradient(135deg, rgba(104, 214, 161, 0.14), rgba(30, 199, 193, 0.08));
}

.state-card.is-error {
    border-color: rgba(240, 107, 79, 0.5);
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.16), rgba(176, 58, 76, 0.08));
}

.color-entry-head {
    display: grid;
    gap: 8px;
}

.swatch-toolbar {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    align-items: center;
}

.swatch-mode-button[aria-pressed="true"] {
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.9), rgba(30, 199, 193, 0.75));
    color: #10141d;
}

.color-entry-head h3 {
    margin-top: 4px;
    font-size: 1.08rem;
}

.native-color-field {
    --swatch-size: 54px;
    --swatch-radius: 18px;
    position: relative;
    display: grid;
    grid-template-columns: var(--swatch-size);
    grid-template-rows: var(--swatch-size) auto;
    gap: 6px;
    align-items: center;
    justify-content: center;
    justify-items: center;
    width: 92px;
    min-width: 84px;
    padding: 8px 10px;
    border: 1px solid var(--line);
    border-radius: 16px;
    background: rgba(255, 255, 255, 0.03);
    transition: border-color 0.2s ease, background 0.2s ease, transform 0.2s ease;
}

.native-color-field::before {
    content: "";
    grid-row: 1;
    grid-column: 1;
    width: var(--swatch-size);
    height: var(--swatch-size);
    border: 1px solid var(--line-strong);
    border-radius: var(--swatch-radius);
    background: var(--swatch-color, transparent);
    box-sizing: border-box;
    box-shadow: inset 0 0 18px rgba(255, 255, 255, 0.12);
}

.native-color-field.is-active {
    border-color: rgba(240, 107, 79, 0.42);
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.16), rgba(30, 199, 193, 0.1));
}

.native-color-field:hover {
    transform: translateY(-1px);
}

.native-color-field > span {
    grid-row: 2;
    grid-column: 1;
    margin: 0;
    color: var(--text-soft);
    font-size: 0.72rem;
    letter-spacing: 0.16em;
    text-transform: uppercase;
}

.native-color-field input[type="color"] {
    appearance: none;
    display: block;
    grid-row: 1;
    grid-column: 1;
    place-self: center;
    box-sizing: border-box;
    width: var(--swatch-size);
    height: var(--swatch-size);
    margin: 0;
    padding: 0;
    border: 0;
    border-radius: var(--swatch-radius);
    background: none;
    cursor: pointer;
    overflow: hidden;
    opacity: 0;
    z-index: 1;
}

.native-color-field input[type="color"]::-webkit-color-swatch-wrapper {
    padding: 0;
}

.native-color-field input[type="color"]::-webkit-color-swatch {
    border: 0;
    border-radius: 15px;
}

.native-color-field input[type="color"]::-moz-color-swatch {
    border: 0;
    border-radius: 15px;
}

.text-control {
    display: grid;
}

.text-control input {
    width: 100%;
    min-height: 46px;
    padding: 0 14px;
    border: 1px solid var(--line-strong);
    border-radius: 14px;
    background: rgba(10, 12, 20, 0.6);
    color: var(--text);
    font: inherit;
}

.rgb-channel input {
    text-align: center;
}

.control-block input[type="range"] {
    width: 100%;
}

.control-block output,
.parameter-card output {
    display: inline-block;
    margin-top: 8px;
    color: var(--text-soft);
}

.parameter-range-row {
    display: grid;
    grid-template-columns: minmax(0, 1fr) 72px;
    gap: 10px;
    align-items: center;
}

.parameter-value-input {
    width: 100%;
    min-height: 38px;
    padding: 0 10px;
    border: 1px solid var(--line-strong);
    border-radius: 10px;
    background: rgba(10, 12, 20, 0.6);
    color: var(--text);
    font: inherit;
    text-align: center;
}

.accent-slider {
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.16), rgba(240, 107, 79, 0.1));
}

input[type="range"] {
    appearance: none;
    height: 12px;
    border-radius: 999px;
    background: linear-gradient(90deg, rgba(255, 255, 255, 0.12), rgba(255, 255, 255, 0.05));
    outline: none;
}

input[type="range"]::-webkit-slider-thumb {
    appearance: none;
    width: 20px;
    height: 20px;
    border-radius: 999px;
    border: 2px solid white;
    background: var(--accent);
    box-shadow: 0 8px 18px rgba(0, 0, 0, 0.28);
}

.parameter-card input[type="range"] {
    height: 8px;
}

.parameter-card input[type="range"]::-webkit-slider-thumb {
    width: 18px;
    height: 18px;
}

.parameter-toggle-card {
    display: grid;
    grid-template-columns: minmax(0, 1fr) auto;
    gap: 12px;
    align-items: center;
}

.parameter-toggle-card > span {
    margin: 0;
}

.parameter-toggle-card .toggle-button {
    justify-self: end;
}

input[type="search"],
select {
    width: 100%;
    min-height: 46px;
    padding: 0 14px;
    border: 1px solid var(--line-strong);
    border-radius: 14px;
    background: rgba(10, 12, 20, 0.6);
    color: var(--text);
    font: inherit;
}

.toggle-block {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 12px;
}

.toggle-button {
    min-width: 88px;
    min-height: 42px;
    padding: 0 16px;
    border-radius: 999px;
    background: rgba(93, 209, 143, 0.2);
}

.toggle-button.is-off {
    background: rgba(244, 98, 98, 0.18);
}

.preview-card {
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.16), rgba(30, 199, 193, 0.14));
}

.preview-header,
.payload-head,
.effect-card-head,
.scene-head {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    align-items: start;
}

.preview-dot {
    width: 64px;
    height: 64px;
    border-radius: 18px;
    background: white;
    border: 1px solid rgba(255, 255, 255, 0.28);
    box-shadow: inset 0 0 18px rgba(255, 255, 255, 0.35);
}

.preview-readout {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 10px;
    margin-top: 18px;
}

.preview-readout strong,
.state-card strong,
.effect-card strong,
.scene-card strong,
.system-card strong {
    display: block;
    margin-top: 4px;
    font-size: 1.02rem;
}

.mini-preview {
    height: 72px;
    margin-top: 18px;
    border-radius: 18px;
    border: 1px solid rgba(255, 255, 255, 0.12);
    background: linear-gradient(90deg, #ffffff 0%, #dddddd 100%);
}

.swatch-grid {
    display: grid;
    grid-template-columns: repeat(5, minmax(0, 1fr));
    gap: 10px;
}

.swatch-grid button {
    position: relative;
    min-height: 48px;
    border-radius: 16px;
    border: 1px solid rgba(255, 255, 255, 0.14);
    background: white;
    cursor: pointer;
}

.swatch-grid button span {
    position: absolute;
    right: 8px;
    bottom: 8px;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 18px;
    min-height: 18px;
    padding: 0 5px;
    border-radius: 999px;
    background: rgba(8, 10, 16, 0.64);
    color: var(--text);
    font-size: 0.7rem;
    font-weight: 700;
}

.swatch-grid button.is-save-target {
    border-color: rgba(240, 107, 79, 0.7);
    box-shadow: 0 0 0 1px rgba(240, 107, 79, 0.28);
}

.effect-layout {
    grid-template-columns: minmax(260px, 0.85fr) minmax(0, 1.15fr);
    align-items: start;
}

.effect-browser {
    display: grid;
    gap: 14px;
    align-self: start;
    align-content: start;
    grid-auto-rows: max-content;
}

.effect-selector-card,
.effect-scene-card {
    display: grid;
    gap: 10px;
    align-content: start;
}

.effect-scene-actions {
    grid-template-columns: 1fr;
}

.effect-scenes-bar {
    display: flex;
    justify-content: space-between;
    gap: 12px;
    align-items: center;
    padding: 0 20px 14px;
}

.effect-scenes-summary {
    display: grid;
    gap: 4px;
}

.effect-scenes-summary strong {
    font-size: 1rem;
    color: var(--text);
}

.effect-scenes-toggle[disabled] {
    opacity: 0.55;
    cursor: not-allowed;
}

.effect-list {
    display: grid;
    gap: 10px;
    max-height: 620px;
    overflow: auto;
    padding-right: 4px;
}

.effect-card {
    text-align: left;
    background: rgba(8, 12, 20, 0.72);
    cursor: pointer;
    transition: transform 0.2s ease, border-color 0.2s ease, background 0.2s ease;
}

.effect-card strong {
    color: var(--text);
}

.effect-card-head p {
    color: #d9e4f2;
}

.effect-card > p {
    color: #eef3fb;
}

.effect-card.active {
    border-color: rgba(30, 199, 193, 0.45);
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.22), rgba(12, 18, 28, 0.84));
}

.effect-card:hover,
.effect-card:focus-visible {
    transform: translateY(-1px);
}

.scene-active-block {
    display: grid;
    gap: 14px;
    padding: 0 20px;
}

.scene-effect-hero {
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.12), rgba(240, 107, 79, 0.1));
}

.scene-parameter-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 12px;
}

.scene-parameter-card strong {
    display: block;
    margin-top: 4px;
    color: var(--text);
    font-size: 1rem;
}

.scene-grid {
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    padding: 18px 20px 20px;
}

.effect-scene-grid {
    padding-top: 0;
}

.scene-card {
    display: grid;
    gap: 14px;
}

.scene-toolbar-card {
    grid-column: 1 / -1;
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.12), rgba(240, 107, 79, 0.1));
}

.scene-preview {
    height: 86px;
    border-radius: 18px;
    border: 1px solid rgba(255, 255, 255, 0.1);
    background: linear-gradient(90deg, rgba(255, 255, 255, 0.08), rgba(255, 255, 255, 0.02));
}

.scene-actions,
.action-grid {
    display: grid;
    gap: 10px;
}

.scene-actions {
    grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
}

.action-grid {
    grid-template-columns: repeat(2, 1fr);
}

.action-grid button,
.scene-actions button,
.chip-button {
    min-height: 46px;
    padding: 0 14px;
    border-radius: 14px;
    border: 1px solid var(--line-strong);
    background: rgba(255, 255, 255, 0.06);
}

.action-grid .primary,
.scene-actions .primary {
    background: linear-gradient(135deg, rgba(240, 107, 79, 0.92), rgba(227, 150, 78, 0.92));
    color: #1a1210;
    font-weight: 700;
}

.action-grid .secondary,
.scene-actions .secondary {
    background: linear-gradient(135deg, rgba(30, 199, 193, 0.9), rgba(94, 208, 161, 0.9));
    color: #091415;
    font-weight: 700;
}

.chip-button,
.action-grid .ghost,
.scene-actions .ghost {
    color: var(--text-dim);
}

.action-grid button:disabled,
.scene-actions button:disabled,
.chip-button:disabled {
    opacity: 0.58;
    cursor: not-allowed;
    transform: none;
}

.sticky {
    position: sticky;
    top: 20px;
}

.payload-panel pre {
    margin: 12px 0 0;
    max-height: 340px;
    overflow: auto;
    padding: 14px;
    border-radius: 16px;
    background: rgba(8, 10, 16, 0.82);
    color: #d8fde3;
    font-family: var(--font-code);
    font-size: 0.8rem;
    line-height: 1.55;
}

.visually-hidden {
    position: absolute;
    width: 1px;
    height: 1px;
    padding: 0;
    margin: -1px;
    overflow: hidden;
    clip: rect(0, 0, 0, 0);
    white-space: nowrap;
    border: 0;
}

@media (min-width: 1101px) {
    body {
        background: linear-gradient(180deg, #10141d 0%, #151b27 100%);
    }

    body::before,
    .panel::after {
        content: none;
    }

    .reveal,
    .reveal:nth-child(2),
    .reveal:nth-child(3) {
        animation: none;
    }

    .app-shell {
        width: 100%;
        min-height: calc(100vh - 88px);
        margin: 8px 0;
        display: grid;
        grid-template-rows: auto minmax(0, 1fr);
        gap: 14px;
    }

    .workspace {
        height: 100%;
        min-height: 0;
        grid-template-columns: minmax(0, 1fr);
        gap: 16px;
    }

    .main-column,
    .side-column {
        min-height: 0;
        gap: 16px;
    }

    .tab-panel:not([hidden]) {
        display: grid;
        grid-template-rows: auto minmax(0, 1fr);
        min-height: 0;
    }

    .picker-stack,
    .preview-stack,
    .dock-stack,
    .effect-browser,
    .effect-detail {
        min-height: 0;
        gap: 10px;
    }

    .hero {
        gap: 8px;
        padding: 10px 12px 10px;
        margin-bottom: 0;
    }

    .hero-copy {
        max-width: 520px;
    }

    .hero h1 {
        font-size: clamp(1.25rem, 1.8vw, 1.6rem);
    }

    .hero-utility {
        grid-template-columns: minmax(320px, 0.88fr) minmax(0, 1.12fr);
        gap: 6px 10px;
    }

    .hero-nav-column {
        gap: 6px;
    }

    .hero-sync-grid {
        grid-template-columns: minmax(252px, 0.95fr) minmax(304px, 1.05fr);
        gap: 6px;
    }

    .status-pill,
    .segment-chip,
    .chip-button,
    .scene-tag,
    .context-chip {
        min-height: 26px;
        font-size: 0.76rem;
    }

    .hero-sync-grid .payload-panel,
    .hero-sync-grid .sync-action-panel {
        padding: 10px;
    }

    .hero-sync-grid .sync-action-panel .state-card {
        padding: 10px;
    }

    .segment-strip {
        grid-template-columns: repeat(2, minmax(0, 1fr));
        gap: 6px;
    }

    .segment-chip {
        min-height: 38px;
        padding: 6px 8px;
    }

    .segment-strip-head {
        gap: 8px;
    }

    .segment-strip-head > div {
        gap: 6px;
    }

    .segment-strip-head strong {
        font-size: 0.8rem;
    }

    .segment-selection-badge {
        min-height: 24px;
        padding: 0 8px;
        font-size: 0.72rem;
    }

    .panel {
        border-radius: 20px;
        background: #171c27;
        box-shadow: 0 14px 34px rgba(0, 0, 0, 0.24);
        backdrop-filter: none;
    }

    .panel-head {
        padding: 16px 16px 0;
    }

    .panel-head.compact {
        padding-bottom: 8px;
    }

    .panel-head h2 {
        margin-top: 4px;
        font-size: clamp(1.15rem, 2vw, 1.5rem);
    }

    .panel-copy {
        font-size: 0.84rem;
    }

    .color-layout,
    .effect-layout {
        gap: 12px;
        padding: 12px 14px 14px;
        min-height: 0;
        overflow: auto;
        align-content: start;
    }

    .color-layout {
        grid-template-columns: minmax(0, 0.98fr) minmax(260px, 0.88fr);
    }

    .picker-stage {
        gap: 14px;
    }

    .color-target-stack {
        gap: 14px;
    }

    .picker-frame {
        max-width: 310px;
        padding: 8px;
    }

    #svCanvas {
        max-width: 292px;
    }

    .control-grid,
    .parameter-grid,
    .system-grid,
    .rgb-field-grid,
    .scene-grid {
        gap: 8px;
    }

    .control-grid {
        grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    }

    .control-block,
    .toggle-block,
    .color-entry-card,
    .parameter-card,
    .system-card,
    .scene-card,
    .effect-hero,
    .preview-card,
    .payload-panel,
    .state-card,
    .effect-card {
        padding: 12px;
    }

    .control-block span,
    .toggle-block span,
    .text-control span {
        margin-bottom: 6px;
        font-size: 0.8rem;
    }

    .color-entry-card {
        gap: 10px;
    }

    .native-color-field {
        --swatch-size: 48px;
        --swatch-radius: 14px;
    }

    .text-control input,
    input[type="search"],
    select {
        min-height: 40px;
    }

    .toggle-button {
        min-width: 76px;
        min-height: 38px;
    }

    .preview-dot {
        width: 52px;
        height: 52px;
        border-radius: 14px;
    }

    .preview-readout {
        gap: 8px;
        margin-top: 10px;
    }

    .mini-preview {
        height: 44px;
        margin-top: 10px;
        border-radius: 14px;
    }

    .swatch-grid button {
        min-height: 40px;
        border-radius: 14px;
    }

    .effect-layout {
        grid-template-columns: minmax(220px, 0.8fr) minmax(0, 1.2fr);
    }

    .effect-browser,
    .effect-detail,
    .dock-stack {
        overflow: auto;
    }

    .dock-stack {
        padding: 0 14px 14px;
    }

    .effect-list {
        max-height: none;
        min-height: 0;
        gap: 8px;
    }

    .scene-active-block {
        gap: 12px;
        padding: 0 16px;
    }

    .effect-scenes-bar {
        padding: 0 16px 12px;
    }

    .scene-grid {
        grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
        padding: 14px 16px 16px;
        min-height: 0;
        overflow: auto;
        align-content: start;
    }

    .effect-scene-grid {
        padding-top: 0;
    }

    .scene-card {
        gap: 12px;
    }

    .scene-preview {
        height: 68px;
        border-radius: 14px;
    }

    .scene-actions,
    .action-grid {
        gap: 8px;
    }

    .hero-sync-grid .payload-panel pre {
        max-height: 112px;
        min-height: 88px;
        padding: 8px;
        font-size: 0.72rem;
    }

    .action-grid button,
    .scene-actions button,
    .chip-button {
        min-height: 36px;
        padding: 0 12px;
    }

    .sticky {
        top: 0;
        height: 100%;
        display: grid;
        grid-template-rows: auto minmax(0, 1fr);
    }

    .payload-panel pre {
        max-height: 260px;
        min-height: 120px;
        padding: 10px;
        font-size: 0.74rem;
    }

}

@media (max-width: 1100px) {
    .workspace,
    .effect-layout {
        grid-template-columns: 1fr;
    }

    .hero-meta {
        justify-content: flex-start;
    }

    .hero-utility {
        grid-template-columns: 1fr;
    }

    .hero-sync-grid {
        grid-template-columns: 1fr;
    }

    .segment-strip-head {
        align-items: start;
        flex-direction: column;
    }

    .sticky {
        position: static;
    }
}

@media (max-width: 940px) {
    .color-layout {
        grid-template-columns: 1fr;
    }

    .picker-stage {
        grid-template-columns: minmax(0, 360px);
        justify-content: start;
    }

    .color-target-stack {
        width: min(100%, 198px);
    }
}

@media (max-width: 780px) {
    .app-shell {
        width: min(100vw - 16px, 100%);
        margin: 8px auto 18px;
    }

    .hero,
    .panel-head,
    .color-layout,
    .effect-layout,
    .scene-grid {
        padding-left: 16px;
        padding-right: 16px;
    }

    .hero {
        padding-top: 20px;
    }

    .panel-head {
        display: grid;
    }

    .preview-readout,
    .control-grid,
    .parameter-grid,
    .action-grid,
    .scene-actions {
        grid-template-columns: 1fr;
    }

    .parameter-toggle-card {
        grid-template-columns: 1fr;
        gap: 10px;
    }

    .parameter-toggle-card .toggle-button {
        justify-self: start;
    }

    .effect-scenes-bar {
        display: grid;
        align-items: start;
    }

    .parameter-range-row {
        grid-template-columns: 1fr;
    }

    .rgb-field-grid {
        grid-template-columns: 1fr;
    }

    .picker-stage {
        grid-template-columns: 1fr;
    }

    .color-target-stack {
        width: min(100%, 198px);
        max-width: none;
    }

    .color-entry-head {
        display: grid;
    }

    .swatch-grid {
        grid-template-columns: repeat(4, minmax(0, 1fr));
    }

    .segment-strip {
        grid-template-columns: 1fr;
    }

    .section-tabs {
        overflow-x: auto;
        padding-bottom: 2px;
    }
}
)NPXCSS";

static const char kNeoPixelWebUiAppJs[] = R"NPXJS(
(function () {
    const defaultSwatches = [
        "#ff9360",
        "#ffd166",
        "#f4f1e8",
        "#8ee3c8",
        "#44c8ff",
        "#5f8cff",
        "#ff6f91",
        "#b0ff57",
        "#ff9f1c",
        "#0f1722"
    ];

    const mockMeta = {
        device: {
            product: "OAM NeoPixel",
            firmware: "preview",
            hardwareName: "Mock Hardware",
            platformFamily: "file-preview",
            hostname: "preview.local",
            networkMedium: "mock"
        },
        capabilities: {
            maxSegments: 16,
            maxScenesPerSegment: 10,
            supportsScenes: true,
            supportsPersist: true,
            supportsEtsReadback: true,
            supportsHcl: true,
            supportsSecondaryColor: true,
            supportsRgbw: true,
            supportsRgbcct: true
        },
        segments: [
            { id: 1, label: "Facade", configuredScenes: 4 },
            { id: 2, label: "Shelf", configuredScenes: 4 },
            { id: 3, label: "Cove", configuredScenes: 3 }
        ],
        effects: [
            { id: "solid", name: "Solid", family: "Base", mood: "static", description: "Single-colour output with white mix.", parameters: [] },
            {
                id: "gradient",
                name: "Gradient",
                family: "Built-in",
                mood: "animated",
                description: "Soft gradient movement across the selected segment.",
                parameters: [
                    { key: "speed", label: "Speed", type: "range", min: 0, max: 255, step: 1 },
                    { key: "scale", label: "Scale", type: "range", min: 0, max: 255, step: 1 }
                ]
            },
            {
                id: "theater-chase",
                name: "Theater Chase",
                family: "Built-in",
                mood: "animated",
                description: "Stepped chase effect with tunable rhythm.",
                parameters: [
                    { key: "speed", label: "Speed", type: "range", min: 0, max: 255, step: 1 },
                    { key: "spacing", label: "Spacing", type: "range", min: 1, max: 12, step: 1 },
                    { key: "fade", label: "Fade", type: "range", min: 0, max: 100, step: 1 }
                ]
            },
            {
                id: "candle",
                name: "Candle",
                family: "Built-in",
                mood: "ambient",
                description: "Warm shimmer with a candle-like flicker.",
                parameters: [
                    { key: "flicker", label: "Flicker", type: "range", min: 0, max: 255, step: 1 },
                    { key: "depth", label: "Depth", type: "range", min: 0, max: 255, step: 1 }
                ]
            }
        ]
    };

    const mockStateSeed = {
        connection: { online: true, runtimeMode: "runtime-only", source: "mock" },
        segments: [
            {
                id: 1,
                name: "Facade",
                runtime: {
                    power: true,
                    brightness: 184,
                    hsv: { h: 208, s: 72, v: 92 },
                    white: 18,
                    temperature: 4300,
                    effectId: "gradient",
                    parameters: { speed: 132, scale: 88 }
                },
                scenes: [
                    { id: 1, name: "Arrival", effectId: "gradient", hsv: { h: 24, s: 62, v: 100 }, white: 16, temperature: 3800, brightness: 192, preview: ["#ffcf9d", "#ff9560"] },
                    { id: 2, name: "Calm", effectId: "solid", hsv: { h: 186, s: 38, v: 92 }, white: 22, temperature: 5200, brightness: 164, preview: ["#bffcf4", "#2db6ff"] },
                    { id: 3, name: "Showtime", effectId: "theater-chase", hsv: { h: 336, s: 56, v: 100 }, white: 0, temperature: 3200, brightness: 210, preview: ["#ff7ca7", "#ffd166"] },
                    { id: 4, name: "Night", effectId: "candle", hsv: { h: 36, s: 66, v: 84 }, white: 34, temperature: 2600, brightness: 114, preview: ["#ffb45d", "#ffe2ab"] }
                ]
            },
            {
                id: 2,
                name: "Shelf",
                runtime: {
                    power: true,
                    brightness: 152,
                    hsv: { h: 28, s: 64, v: 98 },
                    white: 36,
                    temperature: 3600,
                    effectId: "solid",
                    parameters: {}
                },
                scenes: [
                    { id: 1, name: "Reading", effectId: "solid", hsv: { h: 38, s: 44, v: 96 }, white: 92, temperature: 3700, brightness: 176, preview: ["#ffe8b0", "#fff5dc"] },
                    { id: 2, name: "Focus", effectId: "gradient", hsv: { h: 210, s: 14, v: 100 }, white: 128, temperature: 5200, brightness: 190, preview: ["#f2f6ff", "#b7d6ff"] },
                    { id: 3, name: "Amber", effectId: "candle", hsv: { h: 32, s: 72, v: 90 }, white: 44, temperature: 2900, brightness: 140, preview: ["#ffba66", "#ffdf96"] },
                    { id: 4, name: "Accent", effectId: "theater-chase", hsv: { h: 188, s: 36, v: 94 }, white: 16, temperature: 5000, brightness: 166, preview: ["#d8f7ff", "#7dd3df"] }
                ]
            },
            {
                id: 3,
                name: "Cove",
                runtime: {
                    power: false,
                    brightness: 120,
                    hsv: { h: 330, s: 52, v: 78 },
                    white: 0,
                    temperature: 4000,
                    effectId: "theater-chase",
                    parameters: { speed: 64, spacing: 4, fade: 24 }
                },
                scenes: [
                    { id: 1, name: "Wallwash", effectId: "gradient", hsv: { h: 214, s: 28, v: 88 }, white: 36, temperature: 5400, brightness: 148, preview: ["#d9e7ff", "#6bcff2"] },
                    { id: 2, name: "Party", effectId: "theater-chase", hsv: { h: 290, s: 74, v: 100 }, white: 0, temperature: 3800, brightness: 188, preview: ["#ff67c8", "#7f6fff"] },
                    { id: 3, name: "Standby", effectId: "candle", hsv: { h: 30, s: 60, v: 58 }, white: 30, temperature: 2400, brightness: 70, preview: ["#ff9350", "#ffd8a3"] }
                ]
            }
        ]
    };

    const appState = {
        api: null,
        meta: null,
        liveState: null,
        draftState: null,
        swatches: loadStoredSwatches(),
        swatchSaveMode: false,
        selectedSegmentId: 1,
        activeTab: "section-color",
        effectSearch: "",
        selectedEffectSceneId: 1,
        effectScenesExpanded: true,
        payloadMode: "runtime",
        activeColorTarget: "primary",
        pendingSceneAction: null,
        draftDirtyReason: "",
        scenesLoaded: false,
        scenesLoading: false,
        syncFeedback: { phase: "idle", mode: null, message: "" },
        syncFeedbackTimer: null
    };

    const refs = {};

    document.addEventListener("DOMContentLoaded", function () {
        init().catch(showError);
    });

    async function init() {
        cacheRefs();
        bindStaticEvents();
        appState.api = window.OamNeopixelApi || (window.location.protocol === "file:" ? createMockApi() : createHttpApi());
        await loadAll();
    }

    function cacheRefs() {
        refs.segmentStrip = document.getElementById("segmentStrip");
        refs.segmentSelectionBadge = document.getElementById("segmentSelectionBadge");
        refs.connectionState = document.getElementById("connectionState");
        refs.persistenceState = document.getElementById("persistenceState");
        refs.svCanvas = document.getElementById("svCanvas");
        refs.pickerCrosshair = document.getElementById("pickerCrosshair");
        refs.hueSlider = document.getElementById("hueSlider");
        refs.primaryColorField = document.getElementById("primaryColorField");
        refs.secondaryColorField = document.getElementById("secondaryColorField");
        refs.rgbColorInput = document.getElementById("rgbColorInput");
        refs.secondaryColorInput = document.getElementById("secondaryColorInput");
        refs.saveSwatchButton = document.getElementById("saveSwatchButton");
        refs.hexInput = document.getElementById("hexInput");
        refs.redInput = document.getElementById("redInput");
        refs.greenInput = document.getElementById("greenInput");
        refs.blueInput = document.getElementById("blueInput");
        refs.brightnessSlider = document.getElementById("brightnessSlider");
        refs.whiteSlider = document.getElementById("whiteSlider");
        refs.temperatureSlider = document.getElementById("temperatureSlider");
        refs.brightnessValue = document.getElementById("brightnessValue");
        refs.whiteValue = document.getElementById("whiteValue");
        refs.temperatureValue = document.getElementById("temperatureValue");
        refs.powerToggle = document.getElementById("powerToggle");
        refs.colorPreview = document.getElementById("colorPreview");
        refs.segmentName = document.getElementById("segmentName");
        refs.activeEffectName = document.getElementById("activeEffectName");
        refs.activeColorLabel = document.getElementById("activeColorLabel");
        refs.activeColorHex = document.getElementById("activeColorHex");
        refs.activeBrightness = document.getElementById("activeBrightness");
        refs.miniPreviewBar = document.getElementById("miniPreviewBar");
        refs.swatchGrid = document.getElementById("swatchGrid");
        refs.effectSearch = document.getElementById("effectSearch");
        refs.effectList = document.getElementById("effectList");
        refs.effectSelect = document.getElementById("effectSelect");
        refs.effectSceneSelect = document.getElementById("effectSceneSelect");
        refs.effectSceneStoreButton = document.getElementById("effectSceneStoreButton");
        refs.effectScenesSummary = document.getElementById("effectScenesSummary");
        refs.sceneSectionToggleButton = document.getElementById("sceneSectionToggleButton");
        refs.parameterGrid = document.getElementById("parameterGrid");
        refs.sceneGrid = document.getElementById("sceneGrid");
        refs.draftStateCard = document.getElementById("draftStateCard");
        refs.draftStatus = document.getElementById("draftStatus");
        refs.draftSummary = document.getElementById("draftSummary");
        refs.applyRuntimeButton = document.getElementById("applyRuntimeButton");
        refs.savePersistedButton = document.getElementById("savePersistedButton");
        refs.refreshStateButton = document.getElementById("refreshStateButton");
        refs.resetDraftButton = document.getElementById("resetDraftButton");
        refs.payloadModeButton = document.getElementById("payloadModeButton");
        refs.payloadPreview = document.getElementById("payloadPreview");
        refs.tabButtons = Array.from(document.querySelectorAll(".section-tabs button"));
        refs.tabPanels = Array.from(document.querySelectorAll(".tab-panel"));
    }

    function bindStaticEvents() {
        const activateColorTarget = function (target) {
            const nextTarget = target === "secondary" && supportsSecondaryColor() ? "secondary" : "primary";
            if (appState.activeColorTarget === nextTarget) {
                return;
            }

            appState.activeColorTarget = nextTarget;
            renderColorSection();
        };

        refs.tabButtons.forEach(function (button) {
            button.addEventListener("click", function () {
                appState.activeTab = button.dataset.panel;
                renderTabs();
            });
        });

        refs.hueSlider.addEventListener("input", function () {
            updateSelectedColor(function (color) {
                color.hsv.h = clamp(Number(refs.hueSlider.value), 0, 360);
            }, "Adjusted hue");
        });

        refs.brightnessSlider.addEventListener("input", function () {
            updateSelectedSegment(function (segment) {
                segment.runtime.brightness = clamp(Number(refs.brightnessSlider.value), 1, 255);
            }, "Adjusted brightness");
        });

        refs.whiteSlider.addEventListener("input", function () {
            updateSelectedColor(function (color) {
                color.white = clamp(Number(refs.whiteSlider.value), 0, 255);
            }, "Adjusted white mix");
        });

        refs.temperatureSlider.addEventListener("input", function () {
            updateSelectedColor(function (color) {
                color.temperature = clamp(Number(refs.temperatureSlider.value), 1900, 9000);
            }, "Adjusted temperature");
        });

        refs.powerToggle.addEventListener("click", function () {
            updateSelectedSegment(function (segment) {
                segment.runtime.power = !segment.runtime.power;
            }, "Toggled segment power");
        });

        refs.primaryColorField.addEventListener("click", function () {
            activateColorTarget("primary");
        });

        refs.rgbColorInput.addEventListener("click", function () {
            activateColorTarget("primary");
        });

        refs.rgbColorInput.addEventListener("focus", function () {
            activateColorTarget("primary");
        });

        refs.rgbColorInput.addEventListener("input", function () {
            activateColorTarget("primary");
            applyRgbColor(hexToRgb(refs.rgbColorInput.value), "Picked primary RGB colour");
        });

        refs.secondaryColorField.addEventListener("click", function () {
            activateColorTarget("secondary");
        });

        refs.secondaryColorInput.addEventListener("click", function () {
            activateColorTarget("secondary");
        });

        refs.secondaryColorInput.addEventListener("focus", function () {
            activateColorTarget("secondary");
        });

        refs.secondaryColorInput.addEventListener("input", function () {
            activateColorTarget("secondary");
            applyRgbColor(hexToRgb(refs.secondaryColorInput.value), "Picked secondary RGB colour");
        });

        refs.hexInput.addEventListener("change", commitHexInput);
        refs.hexInput.addEventListener("blur", commitHexInput);
        refs.hexInput.addEventListener("keydown", function (event) {
            if (event.key === "Enter") {
                event.preventDefault();
                commitHexInput();
            }
        });

        [refs.redInput, refs.greenInput, refs.blueInput].forEach(function (input) {
            input.addEventListener("change", commitRgbInputs);
            input.addEventListener("blur", commitRgbInputs);
            input.addEventListener("keydown", function (event) {
                if (event.key === "Enter") {
                    event.preventDefault();
                    commitRgbInputs();
                }
            });
        });

        refs.saveSwatchButton.addEventListener("click", function () {
            appState.swatchSaveMode = !appState.swatchSaveMode;
            renderSwatches();
        });

        refs.effectSelect.addEventListener("change", function () {
            const nextEffect = effectById(refs.effectSelect.value);
            updateSelectedSegment(function (selectedSegment) {
                selectedSegment.runtime.effectId = nextEffect.id;
                selectedSegment.runtime.parameters = createDefaultParameters(nextEffect, selectedSegment.runtime.parameters);
            }, "Selected effect " + nextEffect.name);
        });

        refs.effectSceneSelect.addEventListener("change", function () {
            appState.selectedEffectSceneId = Number(refs.effectSceneSelect.value || 1);
            renderEffects();
        });

        refs.effectSceneStoreButton.addEventListener("click", function () {
            stageSelectedSceneStore().catch(showError);
        });

        refs.sceneSectionToggleButton.addEventListener("click", function () {
            appState.effectScenesExpanded = !appState.effectScenesExpanded;
            renderApp();
        });

        refs.applyRuntimeButton.addEventListener("click", function () {
            applyPatch(false).catch(showError);
        });

        refs.savePersistedButton.addEventListener("click", function () {
            applyPatch(true).catch(showError);
        });

        refs.refreshStateButton.addEventListener("click", function () {
            refreshState().catch(showError);
        });

        refs.resetDraftButton.addEventListener("click", function () {
            resetDraft();
        });

        refs.payloadModeButton.addEventListener("click", togglePayloadMode);

        bindPicker();
    }

    async function loadAll() {
        const meta = normalizeMeta(await appState.api.loadMeta());
        appState.meta = meta;

        const liveState = normalizeState(await appState.api.loadState(true), true);
        appState.liveState = liveState;
        appState.draftState = deepClone(liveState);
        appState.scenesLoaded = true;
        appState.scenesLoading = false;
        resetSyncFeedback();

        if (!findSegment(appState.draftState, appState.selectedSegmentId) && appState.draftState.segments.length > 0) {
            appState.selectedSegmentId = appState.draftState.segments[0].id;
        }

        renderApp();
    }

    async function refreshState(includeScenes) {
        const shouldIncludeScenes = includeScenes !== false;
        const liveState = normalizeState(await appState.api.loadState(shouldIncludeScenes), shouldIncludeScenes);
        appState.liveState = liveState;
        appState.draftState = deepClone(liveState);
        appState.pendingSceneAction = null;
        appState.draftDirtyReason = shouldIncludeScenes ? "State refreshed from device." : "State refreshed from device without scenes.";
        appState.scenesLoaded = shouldIncludeScenes;
        appState.scenesLoading = false;
        resetSyncFeedback();
        if (!findSegment(appState.draftState, appState.selectedSegmentId) && appState.draftState.segments.length > 0) {
            appState.selectedSegmentId = appState.draftState.segments[0].id;
        }
        renderApp();
    }

    async function ensureScenesLoaded() {
        if (appState.scenesLoaded || appState.scenesLoading) {
            return;
        }

        appState.scenesLoading = true;
        renderScenes();

        try {
            const stateWithScenes = normalizeState(await appState.api.loadState(true), true);
            mergeScenesIntoState(appState.liveState, stateWithScenes);
            mergeScenesIntoState(appState.draftState, stateWithScenes);
            appState.scenesLoaded = true;
            appState.scenesLoading = false;
            renderApp();
        } catch (error) {
            appState.scenesLoading = false;
            renderScenes();
            throw error;
        }
    }

    function mergeScenesIntoState(targetState, sourceState) {
        if (!targetState || !sourceState || !Array.isArray(targetState.segments) || !Array.isArray(sourceState.segments)) {
            return;
        }

        sourceState.segments.forEach(function (sourceSegment) {
            const targetSegment = targetState.segments.find(function (segment) { return segment.id === sourceSegment.id; });
            if (!targetSegment) {
                return;
            }

            targetSegment.scenes = deepClone(sourceSegment.scenes || []);
            targetSegment.configuredScenes = Number(sourceSegment.configuredScenes || targetSegment.configuredScenes || targetSegment.scenes.length || 0);
            targetSegment.scenesLoaded = true;
        });

        targetState.scenesLoaded = true;
    }

    function normalizeMeta(rawMeta) {
        const meta = deepClone(rawMeta || {});
        meta.device = Object.assign({
            product: "OAM NeoPixel",
            firmware: "unknown",
            hardwareName: "Unknown",
            platformFamily: "device",
            hostname: "unknown",
            networkMedium: "unknown"
        }, meta.device || {});

        meta.capabilities = Object.assign({
            maxSegments: 16,
            maxScenesPerSegment: 10,
            supportsScenes: true,
            supportsPersist: true,
            supportsEtsReadback: true,
            supportsHcl: true,
            supportsSecondaryColor: true,
            supportsRgbw: true,
            supportsRgbcct: true
        }, meta.capabilities || {});

        meta.segments = Array.isArray(meta.segments) && meta.segments.length > 0
            ? meta.segments.map(function (segment, index) {
                return {
                    id: Number(segment.id || (index + 1)),
                    label: String(segment.label || ("Segment " + (index + 1))),
                    configuredScenes: Number(segment.configuredScenes || 0)
                };
            })
            : [{ id: 1, label: "Segment 1", configuredScenes: 0 }];

        meta.effects = Array.isArray(meta.effects) && meta.effects.length > 0
            ? meta.effects.map(normalizeEffect)
            : [normalizeEffect({ id: "solid", name: "Solid", family: "Base", mood: "static", description: "Single-colour output.", parameters: [] })];

        return meta;
    }

    function normalizeEffect(effect) {
        const normalized = Object.assign({
            id: "solid",
            name: "Solid",
            family: "Base",
            mood: "static",
            description: "",
            parameters: []
        }, effect || {});

        normalized.parameters = Array.isArray(normalized.parameters)
            ? normalized.parameters.map(function (parameter, index) {
                const result = Object.assign({
                    key: "parameter-" + index,
                    label: "Parameter " + (index + 1),
                    type: "range",
                    min: 0,
                    max: 255,
                    step: 1,
                    options: []
                }, parameter || {});

                if (!Array.isArray(result.options)) {
                    result.options = [];
                }
                return result;
            })
            : [];

        return normalized;
    }

    function normalizeColorModel(rawColor, fallbackColor) {
        const fallback = fallbackColor || {
            hsv: { h: 0, s: 0, v: 0 },
            white: 0,
            temperature: 4200
        };
        const source = rawColor || {};
        const sourceHsv = source.hsv || {};
        const fallbackHsv = fallback.hsv || {};

        return {
            hsv: {
                h: clamp(Number(sourceHsv.h != null ? sourceHsv.h : (fallbackHsv.h != null ? fallbackHsv.h : 0)), 0, 360),
                s: clamp(Number(sourceHsv.s != null ? sourceHsv.s : (fallbackHsv.s != null ? fallbackHsv.s : 0)), 0, 100),
                v: clamp(Number(sourceHsv.v != null ? sourceHsv.v : (fallbackHsv.v != null ? fallbackHsv.v : 0)), 0, 100)
            },
            white: clamp(Number(source.white == null ? (fallback.white == null ? 0 : fallback.white) : source.white), 0, 255),
            temperature: clamp(Number(source.temperature == null ? (fallback.temperature == null ? 4200 : fallback.temperature) : source.temperature), 1900, 9000)
        };
    }

    function normalizeState(rawState, scenesLoaded) {
        const state = deepClone(rawState || {});
        state.connection = Object.assign({
            online: false,
            runtimeMode: "runtime-only",
            source: "device"
        }, state.connection || {});

        state.scenesLoaded = scenesLoaded !== false;

        const sourceSegments = Array.isArray(state.segments) && state.segments.length > 0
            ? state.segments
            : appState.meta.segments.map(function (segment) {
                return { id: segment.id, name: segment.label, runtime: {}, scenes: [] };
            });

        state.segments = sourceSegments.map(function (segment, index) {
            return normalizeSegment(segment, index, state.scenesLoaded);
        });

        if (state.segments.length === 0) {
            state.segments.push(normalizeSegment({ id: 1, name: "Segment 1", runtime: {}, scenes: [] }, 0, state.scenesLoaded));
        }

        return state;
    }

    function normalizeSegment(segment, index, scenesLoaded) {
        const metaSegment = appState.meta.segments[index] || { id: index + 1, label: "Segment " + (index + 1), configuredScenes: 0 };
        const runtime = segment.runtime || {};
        const effect = effectById(runtime.effectId || "solid");
        const primaryColor = normalizeColorModel(runtime.primaryColor || {
            hsv: runtime.hsv,
            white: runtime.white,
            temperature: runtime.temperature
        }, {
            hsv: { h: 0, s: 100, v: 100 },
            white: 0,
            temperature: 4200
        });
        const secondaryColor = normalizeColorModel(runtime.secondaryColor, {
            hsv: { h: primaryColor.hsv.h, s: primaryColor.hsv.s, v: 0 },
            white: 0,
            temperature: primaryColor.temperature
        });
        const normalizedRuntime = {
            power: runtime.power !== false,
            brightness: clamp(Number(runtime.brightness == null ? 160 : runtime.brightness), 1, 255),
            primaryColor: primaryColor,
            secondaryColor: secondaryColor,
            effectId: effect.id,
            parameters: createDefaultParameters(effect, runtime.parameters || {})
        };

        const scenes = Array.isArray(segment.scenes) ? segment.scenes.map(function (scene, sceneIndex) {
            return normalizeScene(scene, sceneIndex, normalizedRuntime);
        }) : [];
        const configuredScenes = Number(
            segment.configuredScenes != null
                ? segment.configuredScenes
                : (metaSegment.configuredScenes != null ? metaSegment.configuredScenes : (scenes.length || 0))
        );

        return {
            id: Number(segment.id || metaSegment.id || (index + 1)),
            name: String(segment.name || metaSegment.label || ("Segment " + (index + 1))),
            runtime: normalizedRuntime,
            scenes: scenes,
            configuredScenes: configuredScenes,
            scenesLoaded: scenesLoaded !== false
        };
    }

    function normalizeScene(scene, index, runtimeFallback) {
        const effect = effectById(scene.effectId || runtimeFallback.effectId);
        const primaryColor = normalizeColorModel(scene.primaryColor || {
            hsv: scene.hsv,
            white: scene.white,
            temperature: scene.temperature
        }, runtimeFallback.primaryColor);
        const secondaryColor = normalizeColorModel(scene.secondaryColor, runtimeFallback.secondaryColor);
        const normalized = {
            id: Number(scene.id || (index + 1)),
            name: String(scene.name || ("Scene " + (index + 1))),
            effectId: effect.id,
            primaryColor: primaryColor,
            secondaryColor: secondaryColor,
            brightness: clamp(Number(scene.brightness == null ? runtimeFallback.brightness : scene.brightness), 1, 255),
            preview: Array.isArray(scene.preview) && scene.preview.length >= 2 ? scene.preview.slice(0, 2) : null
        };

        if (!normalized.preview) {
            const rgb = hsvToRgb(normalized.primaryColor.hsv.h, normalized.primaryColor.hsv.s, normalized.primaryColor.hsv.v);
            const palette = createPreviewPalette(rgb, {
                white: normalized.primaryColor.white,
                temperature: normalized.primaryColor.temperature,
                brightness: normalized.brightness
            });
            normalized.preview = [palette.shadow, palette.highlight];
        }

        return normalized;
    }

    function renderApp() {
        if (!appState.meta || !appState.draftState) {
            return;
        }

        renderHeader();
        renderTabs();
        renderSegments();
        renderColorSection();
        renderSwatches();
        renderEffects();
        renderScenes();
        renderDock();
    }

    function renderHeader() {
        const feedback = appState.syncFeedback || { phase: "idle", mode: null, message: "" };
        refs.connectionState.textContent = appState.draftState.connection.online ? "Connected" : "Offline";
        refs.connectionState.classList.toggle("online", appState.draftState.connection.online);
        refs.persistenceState.classList.toggle("accent", feedback.phase === "pending" || feedback.phase === "success");

        if (feedback.phase === "pending") {
            refs.persistenceState.textContent = feedback.mode === "persist" ? "Saving..." : "Applying...";
            return;
        }

        if (feedback.phase === "success") {
            refs.persistenceState.textContent = feedback.mode === "persist" ? "Saved" : "Applied";
            return;
        }

        if (feedback.phase === "error") {
            refs.persistenceState.textContent = "Action failed";
            return;
        }

        refs.persistenceState.textContent = appState.payloadMode === "persist" ? "Persist preview" : "Runtime focus";
    }

    function renderTabs() {
        refs.tabButtons.forEach(function (button) {
            const isActive = button.dataset.panel === appState.activeTab;
            button.classList.toggle("is-active", isActive);
            button.setAttribute("aria-selected", String(isActive));
            button.tabIndex = isActive ? 0 : -1;
        });

        refs.tabPanels.forEach(function (panel) {
            const isActive = panel.id === appState.activeTab;
            panel.hidden = !isActive;
            panel.classList.toggle("is-active", isActive);
        });
    }

    function renderSegments() {
        refs.segmentStrip.innerHTML = "";
        appState.draftState.segments.forEach(function (segment) {
            const button = document.createElement("button");
            button.type = "button";
            button.className = "segment-chip" + (segment.id === appState.selectedSegmentId ? " active" : "");
            const sceneCount = segment.scenesLoaded ? segment.scenes.length : segment.configuredScenes;
            button.innerHTML = `<div><strong>${escapeHtml(segment.name)}</strong><span>Scene slots: ${sceneCount}</span></div><div><span>Bri ${segment.runtime.brightness}</span></div>`;
            button.addEventListener("click", function () {
                appState.selectedSegmentId = segment.id;
                renderApp();
            });
            refs.segmentStrip.appendChild(button);
        });

        const activeSegment = getSelectedSegment();
        refs.segmentSelectionBadge.textContent = activeSegment ? "Active: " + activeSegment.name : "No active segment";
    }

    function renderColorSection() {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const activeColor = getActiveColor(segment);
        if (!activeColor) {
            return;
        }

        const rgb = hsvToRgb(activeColor.hsv.h, activeColor.hsv.s, activeColor.hsv.v);
        const colorHex = rgbToHex(rgb.r, rgb.g, rgb.b);
        const previewPalette = createPreviewPalette(rgb, {
            white: activeColor.white,
            temperature: activeColor.temperature,
            brightness: segment.runtime.brightness
        });

        refs.segmentName.textContent = segment.name;
        refs.activeEffectName.textContent = effectById(segment.runtime.effectId).name;
        refs.activeColorLabel.textContent = appState.activeColorTarget === "secondary" ? "Secondary colour" : "Primary colour";
        refs.activeColorHex.textContent = colorHex;
        refs.activeBrightness.textContent = String(segment.runtime.brightness);
        refs.hueSlider.value = String(activeColor.hsv.h);
        refs.brightnessSlider.value = String(segment.runtime.brightness);
        refs.whiteSlider.value = String(activeColor.white);
        refs.temperatureSlider.value = String(activeColor.temperature);
        refs.brightnessValue.textContent = String(segment.runtime.brightness);
        refs.whiteValue.textContent = String(activeColor.white);
        refs.temperatureValue.textContent = activeColor.temperature + " K";
        refs.powerToggle.textContent = segment.runtime.power ? "On" : "Off";
        refs.powerToggle.classList.toggle("is-off", !segment.runtime.power);
        refs.powerToggle.setAttribute("aria-pressed", String(segment.runtime.power));
        renderColorTargetInputs(segment);

        refs.colorPreview.style.background = `radial-gradient(circle at 30% 30%, ${previewPalette.highlight} 0%, ${previewPalette.surface} 68%, ${previewPalette.shadow} 100%)`;
        refs.miniPreviewBar.style.background = `linear-gradient(90deg, ${previewPalette.shadow} 0%, ${previewPalette.surface} 52%, ${previewPalette.highlight} 100%)`;

        syncDirectColorInputs(rgb, colorHex);
        drawSvCanvas(activeColor.hsv.h);
        updatePickerCrosshair();
    }

    function syncDirectColorInputs(rgb, colorHex) {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const activeColor = getActiveColor(segment);
        if (!activeColor) {
            return;
        }

        const resolvedRgb = rgb || hsvToRgb(activeColor.hsv.h, activeColor.hsv.s, activeColor.hsv.v);
        const resolvedHex = colorHex || rgbToHex(resolvedRgb.r, resolvedRgb.g, resolvedRgb.b);
        refs.hexInput.value = resolvedHex.toUpperCase();
        refs.redInput.value = String(resolvedRgb.r);
        refs.greenInput.value = String(resolvedRgb.g);
        refs.blueInput.value = String(resolvedRgb.b);
    }

    function renderColorTargetInputs(segment) {
        const primaryRgb = hsvToRgb(segment.runtime.primaryColor.hsv.h, segment.runtime.primaryColor.hsv.s, segment.runtime.primaryColor.hsv.v);
        const secondaryRgb = hsvToRgb(segment.runtime.secondaryColor.hsv.h, segment.runtime.secondaryColor.hsv.s, segment.runtime.secondaryColor.hsv.v);
        const secondaryVisible = supportsSecondaryColor();
        const primaryHex = rgbToHex(primaryRgb.r, primaryRgb.g, primaryRgb.b);
        const secondaryHex = rgbToHex(secondaryRgb.r, secondaryRgb.g, secondaryRgb.b);

        refs.rgbColorInput.value = primaryHex;
        refs.secondaryColorInput.value = secondaryHex;
        refs.primaryColorField.style.setProperty("--swatch-color", primaryHex);
        refs.secondaryColorField.style.setProperty("--swatch-color", secondaryHex);
        refs.primaryColorField.classList.toggle("is-active", appState.activeColorTarget !== "secondary");
        refs.secondaryColorField.classList.toggle("is-active", appState.activeColorTarget === "secondary");
        refs.secondaryColorField.hidden = !secondaryVisible;
        refs.secondaryColorInput.disabled = !secondaryVisible;
    }

    function commitHexInput() {
        const normalized = normalizeHexInput(refs.hexInput.value);
        if (!normalized) {
            syncDirectColorInputs();
            return;
        }
        applyRgbColor(hexToRgb(normalized), "Typed HEX colour");
    }

    function commitRgbInputs() {
        const rgb = parseRgbInputs();
        if (!rgb) {
            syncDirectColorInputs();
            return;
        }
        applyRgbColor(rgb, "Adjusted RGB channels");
    }

    function parseRgbInputs() {
        const values = [refs.redInput.value, refs.greenInput.value, refs.blueInput.value];
        if (values.some(function (value) { return value === "" || value === null; })) {
            return null;
        }

        const channels = values.map(function (value) { return Number(value); });
        if (channels.some(function (value) { return Number.isNaN(value); })) {
            return null;
        }

        return {
            r: clamp(Math.round(channels[0]), 0, 255),
            g: clamp(Math.round(channels[1]), 0, 255),
            b: clamp(Math.round(channels[2]), 0, 255)
        };
    }

    function applyRgbColor(rgb, reason) {
        const safeRgb = {
            r: clamp(Math.round(rgb.r), 0, 255),
            g: clamp(Math.round(rgb.g), 0, 255),
            b: clamp(Math.round(rgb.b), 0, 255)
        };

        updateSelectedColor(function (color) {
            color.hsv = rgbToHsv(safeRgb.r, safeRgb.g, safeRgb.b);
        }, reason);
    }

    function renderSwatches() {
        refs.swatchGrid.innerHTML = "";
        refs.saveSwatchButton.textContent = appState.swatchSaveMode ? "Tap a slot" : "Save current";
        refs.saveSwatchButton.setAttribute("aria-pressed", String(appState.swatchSaveMode));

        appState.swatches.forEach(function (hex, index) {
            const button = document.createElement("button");
            button.type = "button";
            button.style.background = hex;
            if (appState.swatchSaveMode) {
                button.className = "is-save-target";
                const slotLabel = document.createElement("span");
                slotLabel.textContent = String(index + 1);
                button.appendChild(slotLabel);
            }

            button.addEventListener("click", function () {
                if (appState.swatchSaveMode) {
                    saveCurrentColorToSwatch(index);
                    return;
                }

                const rgb = hexToRgb(hex);
                const hsv = rgbToHsv(rgb.r, rgb.g, rgb.b);
                updateSelectedColor(function (color) {
                    color.hsv = hsv;
                }, "Applied quick swatch " + hex);
            });

            refs.swatchGrid.appendChild(button);
        });
    }

    function saveCurrentColorToSwatch(index) {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const activeColor = getActiveColor(segment);
        if (!activeColor) {
            return;
        }

        const rgb = hsvToRgb(activeColor.hsv.h, activeColor.hsv.s, activeColor.hsv.v);
        appState.swatches[index] = rgbToHex(rgb.r, rgb.g, rgb.b);
        appState.swatchSaveMode = false;
        persistSwatches(appState.swatches);
        renderSwatches();
    }

    function renderEffects() {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const activeEffect = effectById(segment.runtime.effectId);
        refs.effectSelect.innerHTML = "";
        appState.meta.effects.forEach(function (effect) {
            const option = document.createElement("option");
            option.value = effect.id;
            option.textContent = effect.name;
            option.selected = effect.id === activeEffect.id;
            refs.effectSelect.appendChild(option);
        });

        renderEffectSceneAssignment(segment);
        renderEffectScenesToggle(segment);
        renderParameters(activeEffect, segment.runtime.parameters);
    }

    function renderEffectScenesToggle(segment) {
        const sceneCount = getSceneSlotCount(segment);
        const sceneLabel = sceneCount === 1 ? "1 scene slot" : (sceneCount + " scene slots");
        refs.effectScenesSummary.textContent = sceneCount > 0 ? sceneLabel : "No scenes configured yet";
        refs.sceneSectionToggleButton.textContent = appState.effectScenesExpanded ? "Hide scenes" : "Show scenes";
        refs.sceneSectionToggleButton.setAttribute("aria-expanded", String(appState.effectScenesExpanded));
        refs.sceneSectionToggleButton.disabled = false;
    }

    function renderEffectSceneAssignment(segment) {
        const sceneCount = getSceneSlotCount(segment);
        refs.effectSceneSelect.innerHTML = "";

        if (sceneCount <= 0) {
            const option = document.createElement("option");
            option.value = "";
            option.textContent = "No scenes configured";
            refs.effectSceneSelect.appendChild(option);
            refs.effectSceneSelect.disabled = true;
            refs.effectSceneStoreButton.disabled = true;
            return;
        }

        const selectedSceneId = getSelectedEffectSceneId(sceneCount);
        for (let sceneId = 1; sceneId <= sceneCount; sceneId += 1) {
            const scene = findSceneById(segment, sceneId);
            const option = document.createElement("option");
            option.value = String(sceneId);
            option.textContent = scene ? (scene.name + " - " + effectById(scene.effectId).name) : ("Scene " + sceneId);
            option.selected = sceneId === selectedSceneId;
            refs.effectSceneSelect.appendChild(option);
        }

        refs.effectSceneSelect.disabled = false;
        refs.effectSceneStoreButton.disabled = false;
    }

    function getSceneSlotCount(segment) {
        if (!segment) {
            return 0;
        }

        const maxScenes = getMaxScenesPerSegment();
        const configuredCount = Number(segment.configuredScenes || 0);
        const loadedCount = Array.isArray(segment.scenes) ? segment.scenes.length : 0;
        return clamp(Math.max(configuredCount, loadedCount), 0, maxScenes);
    }

    function getMaxScenesPerSegment() {
        return Math.max(0, Number(
            appState.meta && appState.meta.capabilities && appState.meta.capabilities.maxScenesPerSegment != null
                ? appState.meta.capabilities.maxScenesPerSegment
                : appState.maxScenesPerSegment
        ) || 0);
    }

    function getSelectedEffectSceneId(sceneCount) {
        if (sceneCount <= 0) {
            appState.selectedEffectSceneId = 1;
            return 0;
        }

        const requestedId = Number(appState.selectedEffectSceneId || 1);
        const normalizedId = clamp(Math.round(requestedId), 1, sceneCount);
        appState.selectedEffectSceneId = normalizedId;
        return normalizedId;
    }

    function findSceneById(segment, sceneId) {
        if (!segment || !Array.isArray(segment.scenes)) {
            return null;
        }

        return segment.scenes.find(function (scene) { return scene.id === sceneId; }) || null;
    }

    function reindexScenes(segment) {
        if (!segment || !Array.isArray(segment.scenes)) {
            return;
        }

        segment.scenes = segment.scenes
            .filter(Boolean)
            .sort(function (left, right) { return left.id - right.id; })
            .map(function (scene, index) {
                scene.id = index + 1;
                scene.name = "Scene " + scene.id;
                return scene;
            });
    }

    function syncSceneWithRuntime(segment, scene) {
        if (!segment || !scene) {
            return;
        }

        const rgb = hsvToRgb(segment.runtime.primaryColor.hsv.h, segment.runtime.primaryColor.hsv.s, segment.runtime.primaryColor.hsv.v);
        const previewPalette = createPreviewPalette(rgb, {
            white: segment.runtime.primaryColor.white,
            temperature: segment.runtime.primaryColor.temperature,
            brightness: segment.runtime.brightness
        });

        scene.name = "Scene " + scene.id;
        scene.effectId = segment.runtime.effectId;
        scene.primaryColor = deepClone(segment.runtime.primaryColor);
        scene.secondaryColor = deepClone(segment.runtime.secondaryColor);
        scene.brightness = segment.runtime.brightness;
        scene.preview = [previewPalette.shadow, previewPalette.highlight];
    }

    function ensureDraftScene(segment, sceneId) {
        const existingScene = findSceneById(segment, sceneId);
        if (existingScene) {
            return existingScene;
        }

        if (!segment || sceneId < 1 || sceneId > getSceneSlotCount(segment)) {
            return null;
        }

        const scene = normalizeScene({
            id: sceneId,
            name: "Scene " + sceneId,
            effectId: segment.runtime.effectId,
            primaryColor: deepClone(segment.runtime.primaryColor),
            secondaryColor: deepClone(segment.runtime.secondaryColor),
            brightness: segment.runtime.brightness
        }, sceneId - 1, segment.runtime);

        segment.scenes.push(scene);
        reindexScenes(segment);
        return findSceneById(segment, sceneId);
    }

    async function stageSelectedSceneStore() {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const sceneCount = getSceneSlotCount(segment);
        if (sceneCount <= 0) {
            return;
        }

        const sceneId = clamp(Number(refs.effectSceneSelect.value || appState.selectedEffectSceneId || 1), 1, sceneCount);
        appState.selectedEffectSceneId = sceneId;

        storeScene(sceneId);
    }

    async function addScene() {
        const selectedSegment = getSelectedSegment();
        if (!selectedSegment) {
            return;
        }

        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const sceneCount = getSceneSlotCount(segment);
        const maxScenes = getMaxScenesPerSegment();
        if (sceneCount >= maxScenes) {
            return;
        }

        const sceneId = sceneCount + 1;
        mutateSelectedSegment(function (draftSegment) {
            draftSegment.configuredScenes = sceneId;
            const scene = ensureDraftScene(draftSegment, sceneId);
            syncSceneWithRuntime(draftSegment, scene);
        }, "Added scene " + sceneId);

        appState.selectedEffectSceneId = sceneId;
        appState.pendingSceneAction = { action: "add", segmentId: segment.id, sceneId: sceneId, sceneName: "Scene " + sceneId };
        renderApp();
    }

    async function deleteScene(sceneId) {
        const selectedSegment = getSelectedSegment();
        if (!selectedSegment) {
            return;
        }

        if (!selectedSegment.scenesLoaded) {
            await ensureScenesLoaded();
        }

        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const scene = findSceneById(segment, sceneId);
        if (!scene) {
            return;
        }

        const sceneCount = getSceneSlotCount(segment);
        if (sceneId !== sceneCount) {
            throw new Error("Delete the highest-numbered scene first.");
        }

        const nextSceneCount = Math.max(0, sceneCount - 1);
        mutateSelectedSegment(function (draftSegment) {
            draftSegment.scenes = draftSegment.scenes.filter(function (item) { return item.id !== sceneId; });
            draftSegment.configuredScenes = nextSceneCount;
        }, "Deleted " + scene.name);

        appState.selectedEffectSceneId = nextSceneCount > 0
            ? clamp(Number(appState.selectedEffectSceneId || 1), 1, nextSceneCount)
            : 1;
        appState.pendingSceneAction = { action: "delete", segmentId: segment.id, sceneId: sceneId, sceneName: scene.name };
        renderApp();
    }

    function renderParameters(effect, values) {
        refs.parameterGrid.innerHTML = "";
        if (effect.parameters.length === 0) {
            const emptyCard = document.createElement("div");
            emptyCard.className = "parameter-card";
            emptyCard.innerHTML = "<strong>No extra parameters</strong><p>This effect uses only the core color and brightness controls.</p>";
            refs.parameterGrid.appendChild(emptyCard);
            return;
        }

        effect.parameters.forEach(function (parameter) {
            const card = document.createElement("div");
            card.className = "parameter-card";
            const currentValue = values[parameter.key] !== undefined ? values[parameter.key] : defaultParameterValue(parameter);

            if (parameter.type === "range") {
                const label = document.createElement("span");
                label.textContent = parameter.label;
                card.appendChild(label);

                const controlRow = document.createElement("div");
                controlRow.className = "parameter-range-row";

                const slider = document.createElement("input");
                slider.type = "range";
                slider.min = String(parameter.min);
                slider.max = String(parameter.max);
                slider.step = String(parameter.step);
                slider.value = String(currentValue);

                const numberInput = document.createElement("input");
                numberInput.type = "number";
                numberInput.className = "parameter-value-input";
                numberInput.min = String(parameter.min);
                numberInput.max = String(parameter.max);
                numberInput.step = String(parameter.step);
                numberInput.value = String(currentValue);

                const applyRangeValue = function (rawValue, renderFully) {
                    const normalizedValue = normalizeRangeValue(parameter, rawValue);
                    const fallbackValue = values[parameter.key] !== undefined ? values[parameter.key] : defaultParameterValue(parameter);
                    if (normalizedValue === null) {
                        numberInput.value = String(fallbackValue);
                        slider.value = String(fallbackValue);
                        return;
                    }

                    slider.value = String(normalizedValue);
                    numberInput.value = String(normalizedValue);

                    mutateSelectedSegment(function (segment) {
                        segment.runtime.parameters[parameter.key] = normalizedValue;
                    }, "Updated " + parameter.label.toLowerCase());

                    if (renderFully) {
                        renderApp();
                    } else {
                        renderDock();
                    }
                };

                slider.addEventListener("input", function () {
                    applyRangeValue(slider.value, false);
                });
                slider.addEventListener("change", renderApp);
                numberInput.addEventListener("change", function () {
                    applyRangeValue(numberInput.value, true);
                });
                numberInput.addEventListener("blur", function () {
                    applyRangeValue(numberInput.value, true);
                });

                controlRow.appendChild(slider);
                controlRow.appendChild(numberInput);
                card.appendChild(controlRow);
            } else if (parameter.type === "select") {
                card.innerHTML = `<span>${escapeHtml(parameter.label)}</span>`;
                const select = document.createElement("select");
                parameter.options.forEach(function (option) {
                    const element = document.createElement("option");
                    element.value = option;
                    element.textContent = option;
                    element.selected = option === currentValue;
                    select.appendChild(element);
                });
                select.addEventListener("change", function () {
                    updateParameter(parameter.key, select.value, "Changed " + parameter.label.toLowerCase());
                });
                card.appendChild(select);
            } else if (parameter.type === "toggle") {
                card.classList.add("parameter-toggle-card");
                const button = document.createElement("button");
                button.type = "button";
                button.className = "toggle-button" + (currentValue ? "" : " is-off");
                button.textContent = currentValue ? "Enabled" : "Disabled";
                button.addEventListener("click", function () {
                    updateParameter(parameter.key, !currentValue, "Toggled " + parameter.label.toLowerCase());
                });
                card.innerHTML = `<span>${escapeHtml(parameter.label)}</span>`;
                card.appendChild(button);
            }

            refs.parameterGrid.appendChild(card);
        });
    }

    function renderScenes() {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        refs.sceneGrid.hidden = !appState.effectScenesExpanded;
        if (!appState.effectScenesExpanded) {
            refs.sceneGrid.innerHTML = "";
            return;
        }

        refs.sceneGrid.innerHTML = "";
        const sceneCount = getSceneSlotCount(segment);
        const maxScenes = getMaxScenesPerSegment();

        const toolbarCard = document.createElement("article");
        toolbarCard.className = "scene-card scene-toolbar-card";
        toolbarCard.innerHTML = `<div class="scene-head"><div><strong>Scene slots</strong><p>${sceneCount > 0 ? sceneCount + " stored scene" + (sceneCount === 1 ? "" : "s") + " available for this segment." : "Add a scene to capture the current effect, colours and parameters."}</p></div><span class="scene-tag">${sceneCount}/${maxScenes}</span></div>`;

        const toolbarActions = document.createElement("div");
        toolbarActions.className = "scene-actions";
        toolbarActions.innerHTML = '<button type="button" class="secondary">Add scene</button>';
        toolbarActions.children[0].disabled = sceneCount >= maxScenes;
        toolbarActions.children[0].addEventListener("click", function () {
            addScene().catch(showError);
        });
        toolbarCard.appendChild(toolbarActions);
        refs.sceneGrid.appendChild(toolbarCard);

        if (sceneCount <= 0) {
            const card = document.createElement("article");
            card.className = "scene-card";
            card.innerHTML = '<div class="scene-head"><div><strong>No scenes stored yet</strong><p>Add a scene to capture the current runtime state into the first slot.</p></div></div>';
            refs.sceneGrid.appendChild(card);
            return;
        }

        if (!appState.scenesLoaded) {
            const card = document.createElement("article");
            card.className = "scene-card";
            card.innerHTML = `<div class="scene-head"><div><strong>${appState.scenesLoading ? "Loading scenes..." : "Scenes not loaded yet"}</strong><p>${appState.scenesLoading ? "Fetching scene snapshots from the device." : "Use the button below to load scene snapshots on demand."}</p></div><span class="scene-tag">Deferred</span></div>`;

            if (!appState.scenesLoading) {
                const actions = document.createElement("div");
                actions.className = "scene-actions";
                actions.innerHTML = '<button type="button" class="primary">Load scenes</button>';
                actions.children[0].addEventListener("click", function () {
                    ensureScenesLoaded().catch(showError);
                });
                card.appendChild(actions);
            }

            refs.sceneGrid.appendChild(card);
            return;
        }

        if (segment.scenes.length === 0) {
            const card = document.createElement("article");
            card.className = "scene-card";
            card.innerHTML = '<div class="scene-head"><div><strong>No scene snapshots available</strong><p>No stored scenes were returned for this segment.</p></div></div>';
            refs.sceneGrid.appendChild(card);
            return;
        }

        segment.scenes.forEach(function (scene) {
            const isLastScene = scene.id === sceneCount;
            const card = document.createElement("article");
            card.className = "scene-card";
            card.innerHTML = `<div class="scene-head"><div><p class="panel-kicker">Scene ${scene.id}</p><strong>${escapeHtml(scene.name)}</strong><p>${escapeHtml(effectById(scene.effectId).name)}</p></div><span class="scene-tag">Segment local</span></div>`;

            const preview = document.createElement("div");
            preview.className = "scene-preview";
            preview.style.background = `linear-gradient(135deg, ${scene.preview[0]}, ${scene.preview[1]})`;
            card.appendChild(preview);

            const actions = document.createElement("div");
            actions.className = "scene-actions";
            actions.innerHTML = "<button type=\"button\" class=\"primary\">Recall</button><button type=\"button\" class=\"secondary\">Store current</button><button type=\"button\" class=\"ghost\">Delete</button>";
            actions.children[0].addEventListener("click", function () {
                recallScene(scene.id);
            });
            actions.children[1].addEventListener("click", function () {
                storeScene(scene.id);
            });
            actions.children[2].disabled = !isLastScene;
            actions.children[2].title = isLastScene ? "Delete this scene" : "Delete the highest-numbered scene first";
            actions.children[2].addEventListener("click", function () {
                deleteScene(scene.id).catch(showError);
            });
            card.appendChild(actions);

            refs.sceneGrid.appendChild(card);
        });
    }

    function formatParameterValue(parameter, value) {
        if (parameter.type === "toggle") {
            return value ? "Enabled" : "Disabled";
        }
        return String(value);
    }

    function normalizeRangeValue(parameter, value) {
        if (value === "" || value === null || value === undefined) {
            return null;
        }

        const numericValue = Number(value);
        if (Number.isNaN(numericValue)) {
            return null;
        }

        const step = Number(parameter.step || 1);
        const min = Number(parameter.min == null ? 0 : parameter.min);
        const max = Number(parameter.max == null ? 255 : parameter.max);
        let normalizedValue = clamp(numericValue, min, max);

        if (Number.isFinite(step) && step > 0) {
            normalizedValue = min + Math.round((normalizedValue - min) / step) * step;
            normalizedValue = clamp(normalizedValue, min, max);
        }

        return normalizedValue;
    }

    function renderDock() {
        const patch = buildPatch(appState.payloadMode === "persist");
        const dirty = isDraftDirty();
        const feedback = appState.syncFeedback || { phase: "idle", mode: null, message: "" };
        const isPending = feedback.phase === "pending";

        refs.draftStateCard.classList.toggle("is-pending", feedback.phase === "pending");
        refs.draftStateCard.classList.toggle("is-success", feedback.phase === "success");
        refs.draftStateCard.classList.toggle("is-error", feedback.phase === "error");

        refs.applyRuntimeButton.disabled = isPending;
        refs.savePersistedButton.disabled = isPending;
        refs.refreshStateButton.disabled = isPending;
        refs.resetDraftButton.disabled = isPending;
        refs.payloadModeButton.disabled = isPending;

        refs.applyRuntimeButton.textContent = isPending && feedback.mode === "runtime" ? "Applying..." : "Apply live";
        refs.savePersistedButton.textContent = isPending && feedback.mode === "persist" ? "Saving..." : "Save to device";

        if (feedback.phase === "pending") {
            refs.draftStatus.textContent = feedback.mode === "persist" ? "Saving..." : "Applying...";
            refs.draftSummary.textContent = feedback.message || "Sending patch to device.";
        } else if (feedback.phase === "success") {
            refs.draftStatus.textContent = feedback.mode === "persist" ? "Saved" : "Applied";
            refs.draftSummary.textContent = feedback.message || "Action completed successfully.";
        } else if (feedback.phase === "error") {
            refs.draftStatus.textContent = "Error";
            refs.draftSummary.textContent = feedback.message || "Action failed.";
        } else {
            refs.draftStatus.textContent = dirty ? "Pending changes" : "Clean";
            refs.draftSummary.textContent = dirty ? appState.draftDirtyReason : "No pending changes.";
        }

        refs.payloadModeButton.textContent = appState.payloadMode === "persist" ? "Persist patch" : "Runtime patch";
        refs.payloadPreview.textContent = JSON.stringify(patch, null, 2);
    }

    function clearSyncFeedbackTimer() {
        if (appState.syncFeedbackTimer) {
            window.clearTimeout(appState.syncFeedbackTimer);
            appState.syncFeedbackTimer = null;
        }
    }

    function resetSyncFeedback() {
        clearSyncFeedbackTimer();
        appState.syncFeedback = { phase: "idle", mode: null, message: "" };
    }

    function setSyncFeedback(phase, mode, message) {
        clearSyncFeedbackTimer();
        appState.syncFeedback = {
            phase: phase,
            mode: mode || null,
            message: message || ""
        };

        if (phase === "success") {
            appState.syncFeedbackTimer = window.setTimeout(function () {
                if (appState.syncFeedback.phase === "success" && appState.syncFeedback.mode === mode) {
                    resetSyncFeedback();
                    renderApp();
                }
            }, 2400);
        }
    }

    function bindPicker() {
        let dragging = false;

        const handlePointer = function (event) {
            if (!dragging) {
                return;
            }

            const rect = refs.svCanvas.getBoundingClientRect();
            const clientX = event.clientX;
            const clientY = event.clientY;
            const x = clamp(clientX - rect.left, 0, rect.width);
            const y = clamp(clientY - rect.top, 0, rect.height);
            const saturation = Math.round((x / Math.max(rect.width, 1)) * 100);
            const value = Math.round((1 - (y / Math.max(rect.height, 1))) * 100);

            updateSelectedColor(function (color) {
                color.hsv.s = saturation;
                color.hsv.v = value;
            }, "Adjusted saturation and brightness field");
        };

        refs.svCanvas.addEventListener("pointerdown", function (event) {
            dragging = true;
            refs.svCanvas.setPointerCapture(event.pointerId);
            handlePointer(event);
        });
        refs.svCanvas.addEventListener("pointermove", handlePointer);
        refs.svCanvas.addEventListener("pointerup", function () {
            dragging = false;
        });
        refs.svCanvas.addEventListener("pointerleave", function () {
            dragging = false;
        });
    }

    function drawSvCanvas(hue) {
        const ctx = refs.svCanvas.getContext("2d");
        if (!ctx) {
            return;
        }

        const width = refs.svCanvas.width;
        const height = refs.svCanvas.height;
        const hueRgb = hsvToRgb(hue, 100, 100);
        const hueHex = rgbToHex(hueRgb.r, hueRgb.g, hueRgb.b);

        ctx.clearRect(0, 0, width, height);
        const saturationGradient = ctx.createLinearGradient(0, 0, width, 0);
        saturationGradient.addColorStop(0, "#ffffff");
        saturationGradient.addColorStop(1, hueHex);
        ctx.fillStyle = saturationGradient;
        ctx.fillRect(0, 0, width, height);

        const valueGradient = ctx.createLinearGradient(0, 0, 0, height);
        valueGradient.addColorStop(0, "rgba(0,0,0,0)");
        valueGradient.addColorStop(1, "rgba(0,0,0,1)");
        ctx.fillStyle = valueGradient;
        ctx.fillRect(0, 0, width, height);
    }

    function updatePickerCrosshair() {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const activeColor = getActiveColor(segment);
        if (!activeColor) {
            return;
        }

        const canvasRect = refs.svCanvas.getBoundingClientRect();
        const frameRect = refs.pickerCrosshair.offsetParent
            ? refs.pickerCrosshair.offsetParent.getBoundingClientRect()
            : canvasRect;
        const markerSize = Math.max(refs.pickerCrosshair.offsetWidth, refs.pickerCrosshair.offsetHeight, 18);
        const markerRadius = markerSize / 2;
        const canvasOffsetLeft = canvasRect.left - frameRect.left;
        const canvasOffsetTop = canvasRect.top - frameRect.top;
        const usableWidth = Math.max(canvasRect.width - markerSize, 0);
        const usableHeight = Math.max(canvasRect.height - markerSize, 0);
        const x = canvasOffsetLeft + markerRadius + ((activeColor.hsv.s / 100) * usableWidth);
        const y = canvasOffsetTop + markerRadius + ((1 - activeColor.hsv.v / 100) * usableHeight);

        refs.pickerCrosshair.style.left = x + "px";
        refs.pickerCrosshair.style.top = y + "px";
    }

    function mutateSelectedSegment(mutator, reason) {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        mutator(segment);
        appState.draftDirtyReason = reason;
    }

    function updateParameter(key, value, reason) {
        updateSelectedSegment(function (segment) {
            segment.runtime.parameters[key] = value;
        }, reason);
    }

    function recallScene(sceneId) {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const scene = segment.scenes.find(function (item) { return item.id === sceneId; });
        if (!scene) {
            return;
        }

        updateSelectedSegment(function (selectedSegment) {
            selectedSegment.runtime.brightness = scene.brightness;
            selectedSegment.runtime.primaryColor = deepClone(scene.primaryColor);
            selectedSegment.runtime.secondaryColor = deepClone(scene.secondaryColor);
            selectedSegment.runtime.effectId = scene.effectId;
            selectedSegment.runtime.parameters = createDefaultParameters(effectById(scene.effectId), {});
        }, "Recalled scene " + scene.name);

        appState.selectedEffectSceneId = scene.id;
        appState.pendingSceneAction = { action: "recall", segmentId: segment.id, sceneId: scene.id, sceneName: scene.name };
        renderApp();
    }

    function storeScene(sceneId) {
        const segment = getSelectedSegment();
        if (!segment) {
            return;
        }

        const scene = ensureDraftScene(segment, sceneId);
        if (!scene) {
            return;
        }

        syncSceneWithRuntime(segment, scene);
        appState.selectedEffectSceneId = sceneId;

        const pendingAction = appState.pendingSceneAction;
        const keepPendingAdd = pendingAction
            && pendingAction.segmentId === segment.id
            && pendingAction.action === "add"
            && pendingAction.sceneId === scene.id;

        if (keepPendingAdd) {
            pendingAction.sceneName = scene.name;
            appState.draftDirtyReason = "Prepared new scene " + scene.name + " from current draft";
        } else {
            appState.pendingSceneAction = { action: "store", segmentId: segment.id, sceneId: scene.id, sceneName: scene.name };
            appState.draftDirtyReason = "Stored current draft into scene " + scene.name;
        }

        renderApp();
    }

    async function applyPatch(persist) {
        if (appState.syncFeedback && appState.syncFeedback.phase === "pending") {
            return;
        }

        const mode = persist ? "persist" : "runtime";
        const patch = buildPatch(persist);
        setSyncFeedback("pending", mode, persist ? "Saving current patch to the device..." : "Applying current patch to the live runtime...");
        renderApp();

        try {
            const updatedState = persist ? await appState.api.savePersistedPatch(patch) : await appState.api.applyRuntimePatch(patch);
            if (persist && updatedState && updatedState.queued) {
                appState.liveState = deepClone(appState.draftState);
                appState.pendingSceneAction = null;
                appState.draftState = deepClone(appState.liveState);
                appState.draftDirtyReason = "Device save queued.";
                setSyncFeedback("success", mode, updatedState.message || "Patch queued for device save.");
                renderApp();
                return;
            }

            appState.liveState = normalizeState(updatedState);
            appState.draftState = deepClone(appState.liveState);
            appState.pendingSceneAction = null;
            appState.draftDirtyReason = persist ? "Device state saved." : "Runtime state applied.";
            setSyncFeedback("success", mode, persist ? "Patch saved to the device." : "Patch applied to the live runtime.");
            renderApp();
        } catch (error) {
            setSyncFeedback("error", mode, error && error.message ? error.message : "Unable to apply patch.");
            renderApp();
            throw error;
        }
    }

    function resetDraft() {
        appState.draftState = deepClone(appState.liveState);
        appState.pendingSceneAction = null;
        appState.draftDirtyReason = "Draft reset to live state.";
        resetSyncFeedback();
        renderApp();
    }

    function togglePayloadMode() {
        appState.payloadMode = appState.payloadMode === "persist" ? "runtime" : "persist";
        renderDock();
        refs.persistenceState.textContent = appState.payloadMode === "persist" ? "Persist preview" : "Runtime focus";
    }

    function buildPatch(persist) {
        const segment = getSelectedSegment();
        if (!segment) {
            return { persist: persist, target: persist ? "runtime-and-persisted" : "runtime-only", segments: [] };
        }

        const effect = effectById(segment.runtime.effectId);

        const patch = {
            persist: persist,
            target: persist ? "runtime-and-persisted" : "runtime-only",
            segments: [
                {
                    id: segment.id,
                    power: segment.runtime.power,
                    brightness: segment.runtime.brightness,
                    primaryColor: deepClone(segment.runtime.primaryColor),
                    secondaryColor: deepClone(segment.runtime.secondaryColor),
                    effect: {
                        id: segment.runtime.effectId,
                        parameters: createDefaultParameters(effect, segment.runtime.parameters)
                    }
                }
            ]
        };

        if (appState.pendingSceneAction && appState.pendingSceneAction.segmentId === segment.id) {
            patch.sceneAction = deepClone(appState.pendingSceneAction);
        }

        return patch;
    }

    function updateSelectedSegment(mutator, reason) {
        mutateSelectedSegment(mutator, reason);
        renderApp();
    }

    function updateSelectedColor(mutator, reason) {
        updateSelectedSegment(function (segment) {
            const color = getActiveColor(segment);
            if (!color) {
                return;
            }

            mutator(color, segment);
        }, reason);
    }

    function getSelectedSegment() {
        return findSegment(appState.draftState, appState.selectedSegmentId);
    }

    function supportsSecondaryColor() {
        return !!(appState.meta && appState.meta.capabilities && appState.meta.capabilities.supportsSecondaryColor);
    }

    function getRuntimeColor(segment, target) {
        if (!segment || !segment.runtime) {
            return null;
        }

        if (target === "secondary" && supportsSecondaryColor()) {
            return segment.runtime.secondaryColor;
        }

        return segment.runtime.primaryColor;
    }

    function getActiveColor(segment) {
        if (!supportsSecondaryColor() && appState.activeColorTarget !== "primary") {
            appState.activeColorTarget = "primary";
        }

        return getRuntimeColor(segment, appState.activeColorTarget);
    }

    function findSegment(state, segmentId) {
        if (!state || !Array.isArray(state.segments)) {
            return null;
        }
        return state.segments.find(function (segment) { return segment.id === segmentId; }) || null;
    }

    function effectById(effectId) {
        if (!appState.meta || !Array.isArray(appState.meta.effects) || appState.meta.effects.length === 0) {
            return normalizeEffect({ id: "solid", name: "Solid", family: "Base", mood: "static", description: "", parameters: [] });
        }

        return appState.meta.effects.find(function (effect) { return effect.id === effectId; }) || appState.meta.effects[0];
    }

    function isDraftDirty() {
        return JSON.stringify(appState.liveState) !== JSON.stringify(appState.draftState) || !!appState.pendingSceneAction;
    }

    function createDefaultParameters(effect, currentValues) {
        const sourceValues = currentValues || {};
        const nextValues = {};
        effect.parameters.forEach(function (parameter) {
            if (sourceValues[parameter.key] !== undefined) {
                nextValues[parameter.key] = sourceValues[parameter.key];
            } else {
                nextValues[parameter.key] = defaultParameterValue(parameter);
            }
        });
        return nextValues;
    }

    function defaultParameterValue(parameter) {
        if (parameter.type === "select") {
            return parameter.options.length > 0 ? parameter.options[0] : "";
        }
        if (parameter.type === "toggle") {
            return false;
        }
        return Math.round((Number(parameter.min || 0) + Number(parameter.max || 255)) / 2);
    }

    function createHttpApi() {
        const apiBaseUrl = new URL("/neopixel/api/", window.location.href);

        async function requestJson(path, options) {
            const response = await fetch(new URL(path, apiBaseUrl), options);
            const text = await response.text();
            let payload = null;

            if (text) {
                try {
                    payload = JSON.parse(text);
                } catch (error) {
                    throw new Error(text || ("HTTP " + response.status));
                }
            }

            if (!response.ok) {
                throw new Error(payload && payload.error ? payload.error : (text || ("HTTP " + response.status)));
            }

            return payload;
        }

        return {
            loadMeta: function () {
                return requestJson("meta");
            },
            loadState: function (includeScenes) {
                const query = includeScenes === false ? "state?includeScenes=0" : "state";
                return requestJson(query);
            },
            applyRuntimePatch: function (patch) {
                return requestJson("state", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(Object.assign({}, patch, { persist: false, target: "runtime-only" }))
                });
            },
            savePersistedPatch: function (patch) {
                return requestJson("state", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify(Object.assign({}, patch, { persist: true, target: "runtime-and-persisted" }))
                });
            }
        };
    }

    function createMockApi() {
        let liveState = deepClone(mockStateSeed);
        let persistedState = deepClone(mockStateSeed);

        return {
            loadMeta: function () {
                return Promise.resolve(deepClone(mockMeta));
            },
            loadState: function (includeScenes) {
                if (includeScenes === false) {
                    const stateWithoutScenes = deepClone(liveState);
                    stateWithoutScenes.segments = stateWithoutScenes.segments.map(function (segment) {
                        const nextSegment = deepClone(segment);
                        nextSegment.scenes = [];
                        return nextSegment;
                    });
                    return Promise.resolve(stateWithoutScenes);
                }
                return Promise.resolve(deepClone(liveState));
            },
            applyRuntimePatch: function (patch) {
                liveState = mergePatch(liveState, patch);
                liveState.connection.runtimeMode = "runtime-only";
                return Promise.resolve(deepClone(liveState));
            },
            savePersistedPatch: function (patch) {
                liveState = mergePatch(liveState, patch);
                persistedState = mergePatch(persistedState, patch);
                liveState.connection.runtimeMode = "runtime-and-persisted";
                return Promise.resolve(deepClone(liveState));
            }
        };
    }

    function mergePatch(state, patch) {
        const nextState = deepClone(state);

        (patch.segments || []).forEach(function (segmentPatch) {
            const target = nextState.segments.find(function (segment) { return segment.id === segmentPatch.id; });
            if (!target) {
                return;
            }

            target.runtime.power = segmentPatch.power;
            target.runtime.brightness = segmentPatch.brightness;
            target.runtime.primaryColor = normalizeColorModel(segmentPatch.primaryColor || segmentPatch.color, target.runtime.primaryColor);
            target.runtime.secondaryColor = normalizeColorModel(segmentPatch.secondaryColor, target.runtime.secondaryColor);
            target.runtime.effectId = segmentPatch.effect.id;
            target.runtime.parameters = deepClone(segmentPatch.effect.parameters || {});
        });

        if (patch.sceneAction) {
            const targetSegment = nextState.segments.find(function (segment) { return segment.id === patch.sceneAction.segmentId; });
            if (targetSegment) {
                const targetScene = targetSegment.scenes.find(function (scene) { return scene.id === patch.sceneAction.sceneId; });
                if (patch.sceneAction.action === "add") {
                    const currentSceneCount = getSceneSlotCount(targetSegment);
                    const nextSceneId = Math.min(getMaxScenesPerSegment(), currentSceneCount + 1);
                    if (nextSceneId > currentSceneCount) {
                        targetSegment.configuredScenes = nextSceneId;
                        const addedScene = ensureDraftScene(targetSegment, nextSceneId);
                        syncSceneWithRuntime(targetSegment, addedScene);
                    }
                } else if (targetScene && patch.sceneAction.action === "store") {
                    syncSceneWithRuntime(targetSegment, targetScene);
                } else if (targetScene && patch.sceneAction.action === "recall") {
                    targetSegment.runtime.brightness = targetScene.brightness;
                    targetSegment.runtime.primaryColor = deepClone(targetScene.primaryColor);
                    targetSegment.runtime.secondaryColor = deepClone(targetScene.secondaryColor);
                    targetSegment.runtime.effectId = targetScene.effectId;
                    targetSegment.runtime.parameters = createDefaultParameters(effectById(targetScene.effectId), {});
                } else if (patch.sceneAction.action === "delete") {
                    const sceneCount = getSceneSlotCount(targetSegment);
                    if (patch.sceneAction.sceneId === sceneCount) {
                        const nextSceneCount = Math.max(0, sceneCount - 1);
                        targetSegment.scenes = targetSegment.scenes.filter(function (scene) { return scene.id !== patch.sceneAction.sceneId; });
                        targetSegment.configuredScenes = nextSceneCount;
                    }
                }
            }
        }

        return nextState;
    }

    function hsvToRgb(h, s, v) {
        const hue = ((h % 360) + 360) % 360;
        const sat = clamp(s, 0, 100) / 100;
        const val = clamp(v, 0, 100) / 100;
        const chroma = val * sat;
        const section = hue / 60;
        const x = chroma * (1 - Math.abs(section % 2 - 1));
        let red = 0;
        let green = 0;
        let blue = 0;

        if (section >= 0 && section < 1) {
            red = chroma;
            green = x;
        } else if (section < 2) {
            red = x;
            green = chroma;
        } else if (section < 3) {
            green = chroma;
            blue = x;
        } else if (section < 4) {
            green = x;
            blue = chroma;
        } else if (section < 5) {
            red = x;
            blue = chroma;
        } else {
            red = chroma;
            blue = x;
        }

        const match = val - chroma;
        return {
            r: Math.round((red + match) * 255),
            g: Math.round((green + match) * 255),
            b: Math.round((blue + match) * 255)
        };
    }

    function rgbToHsv(r, g, b) {
        const red = r / 255;
        const green = g / 255;
        const blue = b / 255;
        const max = Math.max(red, green, blue);
        const min = Math.min(red, green, blue);
        const delta = max - min;
        let hue = 0;

        if (delta !== 0) {
            if (max === red) {
                hue = 60 * (((green - blue) / delta) % 6);
            } else if (max === green) {
                hue = 60 * (((blue - red) / delta) + 2);
            } else {
                hue = 60 * (((red - green) / delta) + 4);
            }
        }

        if (hue < 0) {
            hue += 360;
        }

        return {
            h: Math.round(hue),
            s: Math.round(max === 0 ? 0 : (delta / max) * 100),
            v: Math.round(max * 100)
        };
    }

    function rgbToHex(r, g, b) {
        return "#" + [r, g, b].map(function (value) {
            return value.toString(16).padStart(2, "0");
        }).join("");
    }

    function hexToRgb(hex) {
        const normalized = hex.replace("#", "");
        const value = normalized.length === 3 ? normalized.split("").map(function (char) {
            return char + char;
        }).join("") : normalized;
        return {
            r: parseInt(value.slice(0, 2), 16),
            g: parseInt(value.slice(2, 4), 16),
            b: parseInt(value.slice(4, 6), 16)
        };
    }

    function normalizeHexInput(value) {
        const normalized = String(value).trim().replace(/^#/, "");
        if (!/^[0-9a-fA-F]{3}$|^[0-9a-fA-F]{6}$/.test(normalized)) {
            return null;
        }

        return "#" + (normalized.length === 3 ? normalized.split("").map(function (char) {
            return char + char;
        }).join("") : normalized).toLowerCase();
    }

    function createPreviewPalette(baseRgb, runtime) {
        const surface = composePreviewRgb(baseRgb, runtime.white, runtime.temperature, runtime.brightness);
        const highlight = composePreviewRgb(baseRgb, Math.min(255, runtime.white + 56), runtime.temperature, Math.min(255, runtime.brightness + 30));
        const shadow = composePreviewRgb(baseRgb, Math.max(0, runtime.white - 18), runtime.temperature, Math.max(24, runtime.brightness - 56));

        return {
            surface: rgbToHex(surface.r, surface.g, surface.b),
            highlight: rgbToHex(highlight.r, highlight.g, highlight.b),
            shadow: rgbToHex(shadow.r, shadow.g, shadow.b)
        };
    }

    function composePreviewRgb(baseRgb, whiteAmount, temperature, brightness) {
        const whiteRgb = kelvinToRgb(temperature);
        const whiteFactor = clamp(whiteAmount, 0, 255) / 255 * 0.72;
        const brightnessFactor = 0.24 + (clamp(brightness, 1, 255) / 255) * 0.76;
        const mixed = blendRgb(baseRgb, whiteRgb, whiteFactor);
        return scaleRgb(mixed, brightnessFactor);
    }

    function kelvinToRgb(temperature) {
        const temp = clamp(temperature, 1900, 9000) / 100;
        let red;
        let green;
        let blue;

        if (temp <= 66) {
            red = 255;
            green = 99.4708025861 * Math.log(temp) - 161.1195681661;
            blue = temp <= 19 ? 0 : 138.5177312231 * Math.log(temp - 10) - 305.0447927307;
        } else {
            red = 329.698727446 * Math.pow(temp - 60, -0.1332047592);
            green = 288.1221695283 * Math.pow(temp - 60, -0.0755148492);
            blue = 255;
        }

        return {
            r: clamp(Math.round(red), 0, 255),
            g: clamp(Math.round(green), 0, 255),
            b: clamp(Math.round(blue), 0, 255)
        };
    }

    function blendRgb(source, target, factor) {
        return {
            r: Math.round(source.r + (target.r - source.r) * factor),
            g: Math.round(source.g + (target.g - source.g) * factor),
            b: Math.round(source.b + (target.b - source.b) * factor)
        };
    }

    function scaleRgb(rgb, factor) {
        return {
            r: clamp(Math.round(rgb.r * factor), 0, 255),
            g: clamp(Math.round(rgb.g * factor), 0, 255),
            b: clamp(Math.round(rgb.b * factor), 0, 255)
        };
    }

    function clamp(value, min, max) {
        return Math.min(Math.max(value, min), max);
    }

    function deepClone(value) {
        return JSON.parse(JSON.stringify(value));
    }

    function loadStoredSwatches() {
        try {
            const stored = window.localStorage.getItem("oam-neopixel-swatches");
            if (!stored) {
                return defaultSwatches.slice();
            }

            const parsed = JSON.parse(stored);
            if (!Array.isArray(parsed) || parsed.length !== defaultSwatches.length) {
                return defaultSwatches.slice();
            }

            return parsed.map(function (value, index) {
                return normalizeHexInput(value) || defaultSwatches[index];
            });
        } catch (error) {
            return defaultSwatches.slice();
        }
    }

    function persistSwatches(values) {
        try {
            window.localStorage.setItem("oam-neopixel-swatches", JSON.stringify(values));
        } catch (error) {
            return;
        }
    }

    function escapeHtml(value) {
        return String(value)
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/\"/g, "&quot;")
            .replace(/'/g, "&#39;");
    }

    function showError(error) {
        const message = error && error.message ? error.message : String(error);
        console.error(error);
        if (refs.connectionState) {
            refs.connectionState.textContent = "Error";
            refs.connectionState.classList.remove("online");
        }
        if (refs.draftStatus) {
            refs.draftStatus.textContent = "Error";
        }
        if (refs.draftSummary) {
            refs.draftSummary.textContent = message;
        }
        if (refs.payloadPreview) {
            refs.payloadPreview.textContent = message;
        }
    }
})();
)NPXJS";