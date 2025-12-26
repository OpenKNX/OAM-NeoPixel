#ifndef SEGMENT_CONTROLLER_H
#define SEGMENT_CONTROLLER_H

#include <string>
#include <cstdint>

// Forward declarations
class NeoPixelBusModule;
class GroupObject;

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
     * @brief Process Color Temperature (CCT) KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processCctKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Process HSV (Hue/Saturation/Value) KO
     * @param channel Segment index
     * @param ko GroupObject reference
     */
    void processHsvKo(uint8_t channel, GroupObject& ko);

    /**
     * @brief Calculate DPT3.007 delta value from stepCode
     * @param stepCode Step code (1-7, higher = faster dimming)
     * @return Delta value for dimming step
     */
    int16_t dpt3_007_delta(uint8_t stepCode);
};

#endif // SEGMENT_CONTROLLER_H
