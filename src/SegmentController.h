#ifndef SEGMENT_CONTROLLER_H
#define SEGMENT_CONTROLLER_H

#include <cstdint>
#include <string>

// Forward declarations
class NeoPixelBusModule;
class GroupObject;
class Segment;

/**
 * @brief Handles segment-specific operations including dimming and KO processing
 *
 * This class manages:
 * - Start/stop dimming for segment channels (RGBW, HSV, Brightness)
 * - Processing KNX communication objects for segment control
 * - Color updates and effect management per segment
 */
class SegmentController
{
    friend class NeoPixelBusModule;

  public:
    /**
     * @brief Construct a new Segment Controller
     * @param module Parent NeoPixelBusModule instance
     */
    SegmentController(NeoPixelBusModule* module);

    /**
     * @brief Destroy the Segment Controller
     */
    ~SegmentController();

    /**
     * @brief Logger prefix for this class
     * @return Logger prefix string
     */
    inline const std::string logPrefix() { return "SegCtrl"; }

    /**
     * @brief Process active start/stop dimming for all segments
     *
     * Handles continuous dimming operations based on DPT3.007 telegrams.
     * Updates segment colors/brightness according to active dimming channel.
     * Automatically stops dimming after timeout (2 seconds).
     */
    void processActiveDimming();

    /**
     * @brief Process segment-specific KNX communication objects
     * @param ko GroupObject reference
     * @param koNumber KO number (for routing)
     * @param channel Segment/channel index
     * @return true if KO was handled by this controller
     * @return false if KO should be handled elsewhere
     */
    bool processSegmentKo(GroupObject& ko, uint16_t koNumber, uint8_t channel);

  private:
    NeoPixelBusModule* _module; ///< Reference to parent module

    /**
     * @brief Helper: Start or stop dimming for a segment channel
     * @param channel Segment index
     * @param dimmingChannel Which channel to dim (R/G/B/W/Brightness/HSV)
     * @param rel DPT3.007 relative control byte
     */
    void startStopDimming(uint8_t channel, uint8_t dimmingChannel, uint8_t rel);

    /**
     * @brief Process Red channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processRedKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process Green channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processGreenKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process Blue channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processBlueKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process White channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processWhiteKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process Hue channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processHueKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process Saturation channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processSaturationKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process Value (brightness) channel KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processValueKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process combined HSV KO (3-byte H/S/V)
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processHsvKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Helper: Apply color channel dimming (shared by R/G/B channels)
     * @param segment Target segment
     * @param colorChannel Which color channel (0=R, 1=G, 2=B)
     * @param delta Dimming delta value
     */
    void applyColorChannelDimming(Segment* segment, uint8_t colorChannel, int16_t delta);

    /**
     * @brief Helper: Process single color channel KO (shared by R/G/B/W)
     * @param channel Segment index
     * @param ko GroupObject reference
     * @param colorValue New color value
     * @param colorChannel Which color channel (0=R, 1=G, 2=B, 3=W)
     * @param channelName Channel name for logging
     */
    void processColorChannelKo(uint8_t channel, GroupObject& ko, uint8_t colorValue, uint8_t colorChannel, const char* channelName);

    /**
     * @brief Helper: Process single HSV channel KO (shared by H/S/V)
     * @param channel Segment index
     * @param ko GroupObject reference
     * @param hsvValue New HSV component value (0-255)
     * @param hsvChannel Which HSV channel (0=H, 1=S, 2=V)
     * @param channelName Channel name for logging
     */
    void processHsvChannelKo(uint8_t channel, GroupObject& ko, uint8_t hsvValue, uint8_t hsvChannel, const char* channelName);

    /**
     * @brief Helper: Send RGBW/RGB status feedback
     * @param channel Segment index
     */
    void sendColorStatusFeedback(uint8_t channel);
};

#endif // SEGMENT_CONTROLLER_H
