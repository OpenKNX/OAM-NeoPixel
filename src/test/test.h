#ifdef NEOPIXEL_MODULE_TEST_ENV
    #include "NeoPixel.h"

inline void setup_test_environment(NeoPixel& neoPixelModule)
{
    NeoPixelManager* mgr = neoPixelModule.getManager();
    if (!mgr)
    {
        openknx.logger.log("NeoPixel Module: Unable to get NeoPixel Manager");
        return;
    }

    #if defined(OKNXHW_OPENKNXIAO_KNEOPIX_RP2040_V1) || defined(OKNXHW_OPENKNXIAO_KNEOPIX_RP2350_V1) || defined(OKNXHW_OPENKNXIAO_RP2040_MINI_V1)
        #if defined(OKNXHW_OPENKNXIAO_RP2040_MINI_V1)
            #define NEOPIX_1 KNXIAO_RP2040_PIN1
            #define NEOPIX_1_LEDS 60
            #define NEOPIX_2 KNXIAO_RP2040_PIN2
            #define NEOPIX_2_LEDS 60
            #define NEOPIX_3_MOSI KNXIAO_RP2040_PIN4
            #define NEOPIX_3_SCK KNXIAO_RP2040_PIN3
            #define NEOPIX_3_LEDS 60
        #else
            #define NEOPIX_1 OKNXHW_OPENKNXIAO_NEOPIXEL
            #define NEOPIX_1_LEDS 1
            #define NEOPIX_2 OKNXHW_OPENKNXIAO_D4
            #define NEOPIX_2_LEDS 64
            #define NEOPIX_3 OKNXHW_OPENKNXIAO_D5
            #define NEOPIX_3_LEDS 64
            #define NEOPIX_4 OKNXHW_OPENKNXIAO_D1
            #define NEOPIX_4_LEDS 8
            #define NEOPIX_5 OKNXHW_OPENKNXIAO_D2
            #define NEOPIX_5_LEDS 64
            #define NEOPIX_6 OKNXHW_OPENKNXIAO_D3
            #define NEOPIX_6_LEDS 64

            #define NEOPIX_7_MOSI OKNXHW_OPENKNXIAO_D9
            #define NEOPIX_7_SCK OKNXHW_OPENKNXIAO_D8
            #define NEOPIX_7_LEDS 150
    // Enable power of the onboard NeoPixel
    pinMode(OKNXHW_OPENKNXIAO_NEOPIXEL_PWR, OUTPUT);
    digitalWrite(OKNXHW_OPENKNXIAO_NEOPIXEL_PWR, HIGH);
        #endif

        #if defined(OKNXHW_OPENKNXIAO_RP2040_MINI_V1)
    auto strip0 = mgr->addStrip(NEOPIX_1, NEOPIX_1_LEDS, LedProtocol::WS2812B, ColorOrder::GRB, TimingMode::AUTO_LEGACY); // use 125MHz timing
    strip0->init();

    auto strip1 = mgr->addStrip(NEOPIX_2, NEOPIX_2_LEDS, LedProtocol::SK6812, ColorOrder::GRB); // External: use AUTO timing (default)
    strip1->init();

    auto strip2 = mgr->addSpiStrip(NEOPIX_3_MOSI, NEOPIX_3_SCK, NEOPIX_3_LEDS, LedProtocol::APA102, ColorOrder::BGR, 4000000); //
    strip2->init();
        #else
    // Physical strips WITH ColorOrder
    // Note: These specific LEDs are RGB-native (not GRB/BGR as typical for these chips)
    // NEOPIX_1 is the onboard NeoPixel (GPIO12) - needs LEGACY_125MHZ timing on RP2350
    auto strip0 = mgr->addStrip(NEOPIX_1, NEOPIX_1_LEDS, LedProtocol::WS2812B, ColorOrder::GRB, TimingMode::AUTO_LEGACY); // Onboard: use 125MHz timing
    strip0->init();

    auto strip1 = mgr->addStrip(NEOPIX_2, NEOPIX_2_LEDS, LedProtocol::WS2812B, ColorOrder::GRB); // External: use AUTO timing (default)
    strip1->init();
    auto strip2 = mgr->addStrip(NEOPIX_3, NEOPIX_3_LEDS, LedProtocol::WS2812B, ColorOrder::GRB); // External: use AUTO timing (default)
    strip2->init();
    auto strip3 = mgr->addStrip(NEOPIX_4, NEOPIX_4_LEDS, LedProtocol::WS2812B, ColorOrder::GRB); // External: use AUTO timing (default)
    strip3->init();
    auto strip4 = mgr->addStrip(NEOPIX_5, NEOPIX_5_LEDS, LedProtocol::WS2812B, ColorOrder::GRB); // External: use AUTO timing (default)
    strip4->init();
    auto strip5 = mgr->addStrip(NEOPIX_6, NEOPIX_6_LEDS, LedProtocol::WS2812B, ColorOrder::GRB); // External: use AUTO timing (default)
    strip5->init();

    // APA102 Clone: Use APA102_CLONE protocol with minRgbValue=8 to prevent color update bug
    auto strip6 = mgr->addSpiStrip(NEOPIX_7_MOSI, NEOPIX_7_SCK, NEOPIX_7_LEDS, LedProtocol::APA102_CLONE, ColorOrder::BGR, 4000000);

    // Configure clone-specific settings (minRgbValue is auto-set by APA102_CLONE, but can be adjusted)
    auto* spiCfg = dynamic_cast<SpiStripConfig*>(strip6->getConfig());
    if (spiCfg)
    {
        spiCfg->setMinRgbValue(8); // Minimum RGB value for clone chips
        strip6->applyConfig();
    }
    strip6->init();
        #endif

        // ONE VirtualStrip for all
        #if defined(OKNXHW_OPENKNXIAO_RP2040_MINI_V1)
    auto virt0 = mgr->addVirtualStrip(
        NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS, ColorOrder::RGBW);
    // Attach all physical strips
    mgr->attachPhysicalToVirtual(virt0, strip0, 0);
    mgr->attachPhysicalToVirtual(virt0, strip1, NEOPIX_1_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip2, NEOPIX_1_LEDS + NEOPIX_2_LEDS);

            #if defined(TWO_SEGMENTS_TEST)
    Segment* seg0 = mgr->addSegment(virt0, 0, NEOPIX_1_LEDS);
    Segment* seg1 = mgr->addSegment(virt0, NEOPIX_1_LEDS + 1, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS - 1);
            #else
    // one segment for all LEDs
    Segment* seg0 = mgr->addSegment(virt0, 0, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS - 1);
            #endif
        #else
    auto virt0 = mgr->addVirtualStrip(
        NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS + NEOPIX_5_LEDS + NEOPIX_6_LEDS + NEOPIX_7_LEDS,
        ColorOrder::RGB);

    // Attach all physical strips
    mgr->attachPhysicalToVirtual(virt0, strip0, 0);
    mgr->attachPhysicalToVirtual(virt0, strip1, NEOPIX_1_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip2, NEOPIX_1_LEDS + NEOPIX_2_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip3, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip4, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip5, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS + NEOPIX_5_LEDS);
    mgr->attachPhysicalToVirtual(virt0, strip6, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS + NEOPIX_5_LEDS + NEOPIX_6_LEDS);

            #if defined(TWO_SEGMENTS_TEST)
    Segment* seg0 = mgr->addSegment(virt0, 0, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS);
    Segment* seg1 = mgr->addSegment(virt0, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + 1, NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS + NEOPIX_5_LEDS + NEOPIX_6_LEDS + NEOPIX_7_LEDS - 1);
            #else
    // ONE segment for all LEDs
    Segment* seg0 = mgr->addSegment(virt0, 0,
                                    NEOPIX_1_LEDS + NEOPIX_2_LEDS + NEOPIX_3_LEDS + NEOPIX_4_LEDS + NEOPIX_5_LEDS + NEOPIX_6_LEDS + NEOPIX_7_LEDS - 1);
            #endif
        #endif
    #else
        #define NEOPIX_1 22
        #define NEOPIX_1_LEDS 64

        #define NEOPIX_2 7
        #define NEOPIX_2_LEDS 64

        #define NEOPIX_3_MOSI 9
        #define NEOPIX_3_SCK 8
        #define NEOPIX_3_LEDS 40

    // Physical strips WITH ColorOrder
    // Note: These specific LEDs are RGB-native (not GRB/BGR as typical for these chips)
    auto strip0 = mgr->addStrip(NEOPIX_1, NEOPIX_1_LEDS, LedProtocol::WS2812B, ColorOrder::RGB); // Actual hardware: RGB
    strip0->init();
    auto strip1 = mgr->addStrip(NEOPIX_2, NEOPIX_2_LEDS, LedProtocol::WS2812B, ColorOrder::RGB); // Actual hardware: RGB
    strip1->init();
    auto strip2 = mgr->addSpiStrip(NEOPIX_3_MOSI, NEOPIX_3_SCK, NEOPIX_3_LEDS, LedProtocol::APA102, ColorOrder::RGB, 1000000); // 5 MHz - Slower for reliable frame detection

    // Configure dummy LED mode BEFORE init (0=none, 1=physical, 2=virtual)
    auto* spiCfg2 = dynamic_cast<SpiStripConfig*>(strip2->getConfig());
    if (spiCfg2)
    {
        spiCfg2->setDummyLedMode(1); // 1 = physical dummy LED (sacrifice LED#0)
        strip2->applyConfig();
    }

    strip2->init();

    // ONE VirtualStrip for all (168 LEDs total: 64+64+40)
    auto virt0 = mgr->addVirtualStrip(168, ColorOrder::RGB); // Default RGB, PhysicalStrips handle conversion

    // Attach all physical strips
    mgr->attachPhysicalToVirtual(virt0, strip0, 0);   // Offset 0-63
    mgr->attachPhysicalToVirtual(virt0, strip1, 64);  // Offset 64-127
    mgr->attachPhysicalToVirtual(virt0, strip2, 128); // Offset 128-167

    // ONE segment for all LEDs
    Segment* seg0 = mgr->addSegment(virt0, 0, 167); // All 168 LEDs

    #endif

    // seg0->setEffect(EffectPool::getSolid());
    // seg0->setPrimaryColor(0, 0, 0, 0);

    /*     seg0->getEffect()->setParameter(0, 0, 128); // Speed (0=Auto)
        seg0->getEffect()->setParameter(0, 1, 170); // Hue (0=Red, 85=Green, 170=Blue)
        seg0->getEffect()->setParameter(0, 2, 4);   // Eye size
        seg0->getEffect()->setParameter(0, 3, 40);  // Fade amount */

    #if defined(TWO_SEGMENTS_TEST)
    seg0->setEffect(EffectPool::getRainbow(), true);
    seg0->getEffect()->setParameter(seg0, 0, 0); // Speed (Default 1)
    seg0->getEffect()->setParameter(seg0, 1, 0); // Delta (Default 7)

    seg0->resume();

    seg1->setEffect(EffectPool::getWipe(), true);
    seg1->getEffect()->setParameter(seg1, 0, 0); // Speed (Default 0)
    seg1->resume();
    #else
    seg0->setEffect(EffectPool::getCylon(), true);
    seg0->resume();
    #endif

    // Enable auto-update
    mgr->updateAll();

    neoPixelModule.setAutoUpdate(true);
}

#endif // NEOPIXEL_MODULE_TEST_ENV

// =============================================================================
// Blink Code Unit Tests (on-device, serial log)
// Activate: -D NEOPIXEL_MODULE_TEST_ENV -D NEOPIXEL_BLINK_CODE_TEST
// Tests priority logic, suppression, and clearing — no visual output.
// =============================================================================
#if defined(NEOPIXEL_BLINK_CODE_TEST) && defined(NEOPIXEL_MODULE_TEST_ENV)
    #include "NeoPixelModule.h"

    #define BLINK_TEST_ASSERT(cond, msg)                                               \
        do                                                                             \
        {                                                                              \
            if (cond)                                                                  \
            {                                                                          \
                openknx.logger.logWithPrefixAndValues("BlinkTest", "  PASS: %s", msg); \
                passCount++;                                                           \
            }                                                                          \
            else                                                                       \
            {                                                                          \
                openknx.logger.logWithPrefixAndValues("BlinkTest", "  FAIL: %s", msg); \
                failCount++;                                                           \
            }                                                                          \
        }                                                                              \
        while (0)

inline void test_blink_codes(NeoPixelBusModule& module)
{
    int passCount = 0;
    int failCount = 0;

    openknx.logger.logWithPrefix("BlinkTest", "========================================");
    openknx.logger.logWithPrefix("BlinkTest", "Starting Blink Code Unit Tests");
    openknx.logger.logWithPrefix("BlinkTest", "========================================");

    // ── Phase 1: Error priority logic ────────────────────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "Phase 1: Error code priority");

    module.clearBlinkCodes();
    module.setErrorBlink(3); // GPIO conflict
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 3, "Error code 3 accepted");

    module.setErrorBlink(5); // Lower priority (higher number)
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 3, "Error code 5 rejected (3 already active)");

    module.setErrorBlink(1); // Higher priority (lower number)
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 1, "Error code 1 overrides 3");

    module.setErrorBlink(1); // Same code again
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 1, "Error code 1 stays on duplicate");

    // ── Phase 2: Warning priority logic ──────────────────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "Phase 2: Warning code priority");

    module.clearBlinkCodes();
    module.setWarningBlink(3, OpenKNX::Led::Color::Yellow); // Unknown effect
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 3, "Warn code 3 accepted");

    module.setWarningBlink(3, OpenKNX::Led::Color::Yellow); // Same again
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 3, "Warn code 3 stays on duplicate");

    module.setWarningBlink(1, OpenKNX::Led::Color::Orange); // Higher priority
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 1, "Warn code 1 overrides 3");

    module.setWarningBlink(2, OpenKNX::Led::Color::Purple); // Lower priority
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 1, "Warn code 2 rejected (1 already active)");

    // ── Phase 3: Error suppresses warning ────────────────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "Phase 3: Error suppresses warning");

    module.clearBlinkCodes();
    module.setWarningBlink(1, OpenKNX::Led::Color::Orange);
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 1, "Warning set when no error");
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 0, "No error active");

    module.setErrorBlink(2); // Error overrides
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 2, "Error overrides warning on LED");

    module.setWarningBlink(1, OpenKNX::Led::Color::Orange); // Try set warning while error active
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 1, "Warn code unchanged (error active, new warn ignored)");

    // ── Phase 4: Clear resets everything ─────────────────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "Phase 4: Clear resets");

    module.clearBlinkCodes();
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == 0, "Error cleared");
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 0, "Warning cleared");

    // ── Phase 5: Simulated error scenarios ───────────────────────────────
    // These simulate what StripConfiguration / NeoPixelModule would do
    // when detecting actual configuration problems.
    openknx.logger.logWithPrefix("BlinkTest", "Phase 5: Simulated error scenarios");

    // 5a: Simulate GPIO conflict detection (StripConfiguration)
    module.clearBlinkCodes();
    // Production: Two strips configured on same GPIO → StripConfiguration detects conflict
    module.setErrorBlink(NEO_ERROR_GPIO_CONFLICT);
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == NEO_ERROR_GPIO_CONFLICT, "GPIO conflict error (3) set");

    // 5b: Higher-severity error overrides (HW mismatch over GPIO conflict)
    module.setErrorBlink(NEO_ERROR_HW_MISMATCH);
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == NEO_ERROR_HW_MISMATCH, "HW mismatch (1) overrides GPIO conflict (3)");

    // 5c: Strip failure doesn't override HW mismatch
    module.setErrorBlink(NEO_ERROR_STRIP_FAILED);
    BLINK_TEST_ASSERT(module.getActiveErrorCode() == NEO_ERROR_HW_MISMATCH, "Strip failed (4) rejected (HW mismatch active)");

    // 5d: Warning while error active
    module.setWarningBlink(NEO_WARN_ABL_ACTIVE, OpenKNX::Led::Color::Orange);
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == 0, "ABL warning rejected while error active");

    // 5e: Warnings work standalone
    module.clearBlinkCodes();
    module.setWarningBlink(NEO_WARN_FLASH_DISCARDED, OpenKNX::Led::Color::Purple);
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == NEO_WARN_FLASH_DISCARDED, "Flash discarded warning (2) set");

    module.setWarningBlink(NEO_WARN_UNKNOWN_EFFECT, OpenKNX::Led::Color::Yellow);
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == NEO_WARN_FLASH_DISCARDED, "Unknown effect (3) rejected (flash discarded active)");

    module.setWarningBlink(NEO_WARN_ABL_ACTIVE, OpenKNX::Led::Color::Orange);
    BLINK_TEST_ASSERT(module.getActiveWarnCode() == NEO_WARN_ABL_ACTIVE, "ABL active (1) overrides flash discarded (2)");

    module.clearBlinkCodes();

    // ── Summary ──────────────────────────────────────────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "========================================");
    openknx.logger.logWithPrefixAndValues("BlinkTest", "Results: %d PASSED, %d FAILED", passCount, failCount);
    openknx.logger.logWithPrefix("BlinkTest", "========================================");

    // ── Phase 6: Visual cycle (observe on STATUS LED) ────────────────────
    openknx.logger.logWithPrefix("BlinkTest", "Phase 6: Visual cycle - watch STATUS LED");
    openknx.logger.logWithPrefix("BlinkTest", "Each error scenario shows on the LED for visual confirmation");

    // Cycle through all error codes with their real scenario names
    struct
    {
        uint8_t code;
        const char* name;
    } errors[] = {
        {NEO_ERROR_HW_MISMATCH, "HW Mismatch"},
        {NEO_ERROR_NO_HW_CONFIGURED, "No HW Configured"},
        {NEO_ERROR_GPIO_CONFLICT, "GPIO Conflict"},
        {NEO_ERROR_STRIP_FAILED, "Strip Failed"},
        {NEO_ERROR_VSTRIP_FAILED, "VStrip Failed"},
    };
    for (auto& e : errors)
    {
        module.clearBlinkCodes();
        module.setErrorBlink(e.code);
        openknx.logger.logWithPrefixAndValues("BlinkTest", "ERROR %d: %s (Red, %d blink) - 4s", e.code, e.name, e.code);
        delay(4000);
    }

    // Cycle through all warning codes with their real scenario names and colours
    struct
    {
        uint8_t code;
        OpenKNX::Led::Color color;
        const char* name;
    } warnings[] = {
        {NEO_WARN_ABL_ACTIVE, OpenKNX::Led::Color::Orange, "ABL Active (Orange)"},
        {NEO_WARN_FLASH_DISCARDED, OpenKNX::Led::Color::Purple, "Flash Discarded (Purple)"},
        {NEO_WARN_UNKNOWN_EFFECT, OpenKNX::Led::Color::Yellow, "Unknown Effect (Yellow)"},
    };
    for (auto& w : warnings)
    {
        module.clearBlinkCodes();
        module.setWarningBlink(w.code, w.color);
        openknx.logger.logWithPrefixAndValues("BlinkTest", "WARN %d: %s (pulsing) - 3s", w.code, w.name);
        delay(3000);
    }

    module.clearBlinkCodes();
    openknx.logger.logWithPrefix("BlinkTest", "Visual cycle complete - STATUS LED cleared");
}

#endif // NEOPIXEL_BLINK_CODE_TEST && NEOPIXEL_MODULE_TEST_ENV