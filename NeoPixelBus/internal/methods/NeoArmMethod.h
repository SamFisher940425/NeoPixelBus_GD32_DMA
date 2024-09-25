/*-------------------------------------------------------------------------
NeoPixel library helper functions for ARM MCUs.
Teensy 3.0, 3.1, LC, Arduino Due

Written by Michael C. Miller.
Some work taken from the Adafruit NeoPixel library.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/NeoPixelBus)

-------------------------------------------------------------------------
This file is part of the Makuna/NeoPixelBus library.
The contents of this file were taken from the Adafruit NeoPixel library
and modified only to fit within individual calling functions.

NeoPixelBus is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#pragma once

#if defined(__arm__) && !defined(ARDUINO_ARCH_NRF52840) && !defined(ARDUINO_ARCH_RP2040)

#include "cstdint"
#include "dma.h"
#include "timer.h"
#include "systick.h" // for get timestamp

static void dma_pwm_init(uint32_t *data_buffer, uint16_t data_size)
{
    DMA0_CH2_TIMER2_Init(data_buffer, data_size);
}

static void dma_pwm_send(uint32_t *data_buffer, uint16_t data_size)
{
    DMA0_CH2_Restart(data_buffer, data_size);
}

template <typename T_SPEED>
class NeoArmMethodBase
{
public:
    typedef NeoNoSettings SettingsObject;

    NeoArmMethodBase(uint8_t pin, uint16_t pixelCount, size_t elementSize, size_t settingsSize) : _sizeData(pixelCount * elementSize + settingsSize),
                                                                                                  _pin(pin),
                                                                                                  _sizeDmaData(pixelCount * elementSize * 8 + 2)

    {
        // pinMode(pin, OUTPUT);

        _data = static_cast<uint8_t *>(malloc(_sizeData));
        // data cleared later in Begin()

        _dmaOutputDataBuf = static_cast<uint32_t *>(malloc(_sizeDmaData * sizeof(uint32_t)));
    }

    NeoArmMethodBase(uint16_t pixelCount, size_t elementSize, size_t settingsSize) : _sizeData(pixelCount * elementSize + settingsSize),
                                                                                     _sizeDmaData(pixelCount * elementSize * 8 + 2)

    {
        _data = static_cast<uint8_t *>(malloc(_sizeData));
        // data cleared later in Begin()

        _dmaOutputDataBuf = static_cast<uint32_t *>(malloc(_sizeDmaData * sizeof(uint32_t)));
    }

    ~NeoArmMethodBase()
    {
        // pinMode(_pin, INPUT);

        free(_dmaOutputDataBuf);
        free(_data);
    }

    bool IsReadyToUpdate() const
    {
        uint32_t delta = Get_Time_us() - _endTime;

        return (delta >= T_SPEED::ResetTimeUs);
    }

    void Initialize()
    {
        // digitalWrite(_pin, LOW);
        dma_pwm_init(_dmaOutputDataBuf, _sizeDmaData);

        _endTime = Get_Time_us();
    }

    void Update(bool)
    {
        // Data latch = 50+ microsecond pause in the output stream.  Rather than
        // put a delay at the end of the function, the ending time is noted and
        // the function will simply hold off (if needed) on issuing the
        // subsequent round of data until the latch time has elapsed.  This
        // allows the mainline code to start generating the next frame of data
        // rather than stalling for the latch.

        while (!IsReadyToUpdate())
        {
            // yield(); // allows for system yield if needed
            // asm("nop;");
            return;
        }
        // Delay_ms(1); // force insert a reset command

        // noInterrupts(); // Need 100% focus on instruction timing

        T_SPEED::send_pixels(_data, _sizeData, _dmaOutputDataBuf, _sizeDmaData);

        // interrupts();

        // save EOD time for latch on next call
        _endTime = Get_Time_us();
    }

    bool AlwaysUpdate()
    {
        // this method requires update to be called only if changes to buffer
        return false;
    }

    uint8_t *getData() const
    {
        return _data;
    };

    size_t getDataSize() const
    {
        return _sizeData;
    };

    void applySettings([[maybe_unused]] const SettingsObject &settings)
    {
    }

private:
    const size_t _sizeData; // Size of '_data' buffer below

    uint32_t _endTime; // Latch timing reference
    uint8_t *_data;    // Holds LED color values
    uint8_t _pin;      // output pin number

    uint32_t *_dmaOutputDataBuf;
    const size_t _sizeDmaData;
};

// Teensy 3.0 or 3.1 (3.2) or 3.5 or 3.6
#if defined(ARDUINO_STM32_FEATHER) || defined(ARDUINO_ARCH_STM32L4) || defined(ARDUINO_ARCH_STM32F4) || defined(ARDUINO_ARCH_STM32F1) // FEATHER WICED (120MHz)

class NeoArmStm32SpeedProps800KbpsBase
{
public:
    static void BitT1hWait()
    {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop;");
    }
    static void BitT1lWait()
    {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop;");
    }
    static void BitT0hWait()
    {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop;");
    }
    static void BitT0lWait()
    {
        asm("nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop; nop; nop; nop; nop;"
            "nop; nop; nop; nop;");
    }
};

class NeoArmStm32SpeedPropsWs2812x : public NeoArmStm32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 300;
};

class NeoArmStm32SpeedPropsSk6812 : public NeoArmStm32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 80;
};

class NeoArmStm32SpeedPropsTm1814 : public NeoArmStm32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmStm32SpeedPropsTm1829 : public NeoArmStm32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmStm32SpeedProps800Kbps : public NeoArmStm32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 50;
};

/* TODO - not found in Adafruit library
class NeoArmStm32SpeedProps400Kbps
{
static void BitT1hWait()
{
}
static void BitT1lWait()
{
}
static void BitT0hWait()
{
}
static void BitT0lWait()
{
}
};
*/

template <typename T_SPEEDPROPS>
class NeoArmStm32SpeedBase
{
public:
    static const uint32_t ResetTimeUs = T_SPEEDPROPS::ResetTimeUs;

    static void send_pixels(uint8_t *pixels, size_t sizePixels, uint8_t pin)
    {
        // Tried this with a timer/counter, couldn't quite get adequate
        // resolution.  So yay, you get a load of goofball NOPs...

        uint8_t *ptr = pixels;
        uint8_t *end = ptr + sizePixels;
        uint8_t p = *ptr++;
        uint8_t bitMask = 0x80;

#if defined(ARDUINO_STM32_FEATHER)
        uint32_t pinMask = BIT(PIN_MAP[pin].gpio_bit);

        volatile uint16_t *set = &(PIN_MAP[pin].gpio_device->regs->BSRRL);
        volatile uint16_t *clr = &(PIN_MAP[pin].gpio_device->regs->BSRRH);

#elif defined(ARDUINO_ARCH_STM32F4)
        uint32_t pinMask = BIT(pin & 0x0f);

        volatile uint16_t *set = &(PIN_MAP[pin].gpio_device->regs->BSRRL);
        volatile uint16_t *clr = &(PIN_MAP[pin].gpio_device->regs->BSRRH);

#elif defined(ARDUINO_ARCH_STM32F1)

        uint32_t pinMask = BIT(PIN_MAP[pin].gpio_bit);

        volatile uint32_t *set = &(PIN_MAP[pin].gpio_device->regs->BRR);
        volatile uint32_t *clr = &(PIN_MAP[pin].gpio_device->regs->BSRR);

#elif defined(ARDUINO_ARCH_STM32L4)

        uint32_t pinMask = g_APinDescription[pin].bit;

        GPIO_TypeDef *GPIO = static_cast<GPIO_TypeDef *>(g_APinDescription[pin].GPIO);

        volatile uint32_t *set = &(GPIO->BRR);
        volatile uint32_t *clr = &(GPIO->BSRR);

#endif
        for (;;)
        {
            if (p & bitMask)
            {
                // ONE
                // High 800ns
                *set = pinMask;
                T_SPEEDPROPS::BitT1hWait();
                // Low 450ns
                *clr = pinMask;
                T_SPEEDPROPS::BitT1lWait();
            }
            else
            {
                // ZERO
                // High 400ns
                *set = pinMask;
                T_SPEEDPROPS::BitT0hWait();
                // Low 850ns
                *clr = pinMask;
                T_SPEEDPROPS::BitT0lWait();
            }
            if (bitMask >>= 1)
            {
                // Move on to the next pixel
                asm("nop;");
            }
            else
            {
                if (ptr >= end)
                {
                    break;
                }

                p = *ptr++;
                bitMask = 0x80;
            }
        }
    }
};

typedef NeoArmMethodBase<NeoArmStm32SpeedBase<NeoArmStm32SpeedPropsWs2812x>> NeoArmWs2812xMethod;
typedef NeoArmMethodBase<NeoArmStm32SpeedBase<NeoArmStm32SpeedPropsSk6812>> NeoArmSk6812Method;
typedef NeoArmMethodBase<NeoArmStm32SpeedBase<NeoArmStm32SpeedPropsTm1814>> NeoArmTm1814InvertedMethod;
typedef NeoArmMethodBase<NeoArmStm32SpeedBase<NeoArmStm32SpeedPropsTm1829>> NeoArmTm1829InvertedMethod;
typedef NeoArmMethodBase<NeoArmStm32SpeedBase<NeoArmStm32SpeedProps800Kbps>> NeoArm800KbpsMethod;
typedef NeoArm800KbpsMethod NeoArmApa106Method;
typedef NeoArmTm1814InvertedMethod NeoArmTm1914InvertedMethod;

#elif defined(GD32E1) || defined(GD32F1) || defined(GD32F3) // FEATHER WICED (120MHz)

#define TIMING_ONE 90
#define TIMING_ZERO 36

class NeoArmGd32SpeedProps800KbpsBase
{
public:
};

class NeoArmGd32SpeedPropsWs2812x : public NeoArmGd32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 300;
};

class NeoArmGd32SpeedPropsSk6812 : public NeoArmGd32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 80;
};

class NeoArmGd32SpeedPropsTm1814 : public NeoArmGd32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmGd32SpeedPropsTm1829 : public NeoArmGd32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmGd32SpeedProps800Kbps : public NeoArmGd32SpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 50;
};

template <typename T_SPEEDPROPS>
class NeoArmGd32SpeedBase
{
public:
    static const uint32_t ResetTimeUs = T_SPEEDPROPS::ResetTimeUs;

    static void send_pixels(uint8_t *pixels, size_t sizePixels, uint32_t *dmaBuf, size_t sizeDmaBuf)
    {
        uint8_t *ptr = pixels;
        uint8_t *end = ptr + sizePixels;
        uint8_t p = *ptr++;
        uint8_t bitMask = 0x80;

        uint32_t butter_index = 0;

        dmaBuf[butter_index] = 0;

        for (;;)
        {
            butter_index++;
            if (p & bitMask)
            {
                // ONE
                dmaBuf[butter_index] = TIMING_ONE;
            }
            else
            {
                // ZERO
                dmaBuf[butter_index] = TIMING_ZERO;
            }
            if (bitMask >>= 1)
            {
                // Move on to the next pixel bit
                // asm("nop;");
                ;
            }
            else
            {
                if (ptr >= end)
                {
                    break;
                }

                p = *ptr++;
                bitMask = 0x80;
            }
        }

        dmaBuf[butter_index + 1] = 0;
        dma_pwm_send(dmaBuf, sizeDmaBuf);
    }
};

typedef NeoArmMethodBase<NeoArmGd32SpeedBase<NeoArmGd32SpeedPropsWs2812x>> NeoArmWs2812xMethod;
typedef NeoArmMethodBase<NeoArmGd32SpeedBase<NeoArmGd32SpeedPropsSk6812>> NeoArmSk6812Method;
typedef NeoArmMethodBase<NeoArmGd32SpeedBase<NeoArmGd32SpeedPropsTm1814>> NeoArmTm1814InvertedMethod;
typedef NeoArmMethodBase<NeoArmGd32SpeedBase<NeoArmGd32SpeedPropsTm1829>> NeoArmTm1829InvertedMethod;
typedef NeoArmMethodBase<NeoArmGd32SpeedBase<NeoArmGd32SpeedProps800Kbps>> NeoArm800KbpsMethod;
typedef NeoArm800KbpsMethod NeoArmApa106Method;
typedef NeoArmWs2812xMethod NeoArmWs2805Method;
typedef NeoArmWs2805Method NeoArmWs2814Method;
typedef NeoArmTm1814InvertedMethod NeoArmTm1914InvertedMethod;

#else // Other ARM architecture -- Presumed Arduino Due

#define ARM_OTHER_SCALE VARIANT_MCK / 2UL / 1000000UL
#define ARM_OTHER_INST (2UL * F_CPU / VARIANT_MCK)

class NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t CyclesT0h = static_cast<uint32_t>((0.40 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t CyclesT1h = static_cast<uint32_t>((0.80 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t Cycles = static_cast<uint32_t>((1.25 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
};

class NeoArmOtherSpeedPropsWs2812x : public NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 300;
};

class NeoArmOtherSpeedPropsWs2805
{
public:
    static const uint32_t CyclesT0h = static_cast<uint32_t>((0.30 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t CyclesT1h = static_cast<uint32_t>((0.79 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t Cycles = static_cast<uint32_t>((1.09 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t ResetTimeUs = 300; // spec is 280, intentionally longer for compatiblity use
};

class NeoArmOtherSpeedPropsSk6812 : public NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 80;
};

class NeoArmOtherSpeedPropsTm1814 : public NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmOtherSpeedPropsTm1829 : public NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 200;
};

class NeoArmOtherSpeedProps800Kbps : public NeoArmOtherSpeedProps800KbpsBase
{
public:
    static const uint32_t ResetTimeUs = 50;
};

class NeoArmOtherSpeedProps400Kbps
{
public:
    static const uint32_t CyclesT0h = static_cast<uint32_t>((0.50 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t CyclesT1h = static_cast<uint32_t>((1.20 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t Cycles = static_cast<uint32_t>((2.50 * ARM_OTHER_SCALE + 0.5) - (5 * ARM_OTHER_INST));
    static const uint32_t ResetTimeUs = 50;
};

template <typename T_SPEEDPROPS>
class NeoArmOtherSpeedBase
{
public:
    static const uint32_t ResetTimeUs = T_SPEEDPROPS::ResetTimeUs;

    static void send_pixels(uint8_t *pixels, size_t sizePixels, uint8_t pin)
    {
        uint32_t pinMask;
        uint32_t t;
        Pio *port;
        volatile WoReg *portSet;
        volatile WoReg *portClear;
        volatile WoReg *timeValue;
        volatile WoReg *timeReset;
        uint8_t *p;
        uint8_t *end;
        uint8_t pix;
        uint8_t mask;

        pmc_set_writeprotect(false);
        pmc_enable_periph_clk(static_cast<uint32_t>(TC3_IRQn));

        TC_Configure(TC1, 0,
                     TC_CMR_WAVE | TC_CMR_WAVSEL_UP | TC_CMR_TCCLKS_TIMER_CLOCK1);
        TC_Start(TC1, 0);

        pinMask = g_APinDescription[pin].ulPin;  // Don't 'optimize' these into
        port = g_APinDescription[pin].pPort;     // declarations above.  Want to
        portSet = &(port->PIO_SODR);             // burn a few cycles after
        portClear = &(port->PIO_CODR);           // starting timer to minimize
        timeValue = &(TC1->TC_CHANNEL[0].TC_CV); // the initial 'while'.
        timeReset = &(TC1->TC_CHANNEL[0].TC_CCR);
        p = pixels;
        end = p + sizePixels;
        pix = *p++;
        mask = 0x80;

        for (;;)
        {
            if (pix & mask)
            {
                t = T_SPEEDPROPS::CyclesT1h;
            }
            else
            {
                t = T_SPEEDPROPS::CyclesT0h;
            }

            // wait for the end of the previous cycle
            while (*timeValue < T_SPEEDPROPS::Cycles)
                ;

            *portSet = pinMask;
            *timeReset = TC_CCR_CLKEN | TC_CCR_SWTRG;

            while (*timeValue < t)
                ;

            *portClear = pinMask;
            if (!(mask >>= 1))
            {
                // This 'inside-out' loop logic utilizes
                if (p >= end)
                {
                    break; // idle time to minimize inter-byte delays.
                }
                pix = *p++;
                mask = 0x80;
            }
        }

        // not really needed as the wait for latch does this and
        // while (*timeValue < T_SPEEDPROPS::Cycles); // Wait for last bit

        TC_Stop(TC1, 0);
    }
};

typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedPropsWs2812x>> NeoArmWs2812xMethod;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedPropsWs2805>> NeoArmWs2805Method;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedPropsSk6812>> NeoArmSk6812Method;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedPropsTm1814>> NeoArmTm1814InvertedMethod;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedPropsTm1829>> NeoArmTm1829InvertedMethod;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedProps800Kbps>> NeoArm800KbpsMethod;
typedef NeoArmMethodBase<NeoArmOtherSpeedBase<NeoArmOtherSpeedProps400Kbps>> NeoArm400KbpsMethod;
typedef NeoArm400KbpsMethod NeoArmApa106Method;
typedef NeoArmWs2805Method NeoArmWs2814Method;
typedef NeoArmTm1814InvertedMethod NeoArmTm1914InvertedMethod;

#endif

// Arm doesn't have alternatives methods yet, so only one to make the default
typedef NeoArmWs2812xMethod NeoWs2813Method;
typedef NeoArmWs2812xMethod NeoWs2812xMethod;
typedef NeoArmWs2812xMethod NeoWs2811Method;
typedef NeoArmWs2812xMethod NeoWs2816Method;
typedef NeoArmWs2805Method NeoWs2805Method;
typedef NeoArmWs2805Method NeoWs2814Method;
typedef NeoArmSk6812Method NeoSk6812Method;
typedef NeoArmSk6812Method NeoLc8812Method;
typedef NeoArm800KbpsMethod NeoWs2812Method;
typedef NeoArmApa106Method NeoApa106Method;
typedef NeoArmWs2812xMethod Neo800KbpsMethod;
#ifdef NeoArm400KbpsMethod // this is needed due to missing 400Kbps for some platforms
typedef NeoArm400KbpsMethod Neo400KbpsMethod;
#endif
// there is no non-invert methods for arm, but the norm for TM1814 is inverted, so
typedef NeoArmTm1814InvertedMethod NeoTm1814InvertedMethod;
typedef NeoArmTm1914InvertedMethod NeoTm1914InvertedMethod;
typedef NeoArmTm1829InvertedMethod NeoTm1829InvertedMethod;

#endif // defined(__arm__)
