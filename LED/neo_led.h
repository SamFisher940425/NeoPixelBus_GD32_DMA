#ifndef NEO_LED_H
#define NEO_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        LED_EVER_BURNING = 0,
        LED_TWINKLE,
        LED_BREATHE,
        LED_METEOR,
        LED_TWINKLE_ONCE,
        LED_COMET,
        LED_LIGHTEN_L2R,
        LED_SHUTDOWN_R2L,
        LED_RAISE,
        LED_DECLINE,
    } LED_STATE;

    void NeoPixelsSetup(void);

    void LedsUpdate(void);
    void LedsStateSet(LED_STATE state);
    LED_STATE LedsStateGet(void);
    void LedsColorSet(uint8_t colorCode);
    void LedsAnimTimeSet(uint16_t time);
    void LedsForceFresh(void);

#ifdef __cplusplus
}
#endif

#endif /* NEO_LED_H */
