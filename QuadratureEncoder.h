
#pragma once

#include <cstdint>

#include "EncoderRingBuffer.h"
#include "hardware/pio.h"

class QuadratureEncoder {
    public:
    static constexpr int MAX_SMS = 4;
    static constexpr uint32_t NO_LED = UINT32_MAX;

    QuadratureEncoder(PIO pio, uint sm, uint pinA, float clkdiv = 250000.0f);

    void init();

    void enableDebugLed(uint32_t led_pin);
    void disableDebugLed();

    EncoderRingBuffer& buffer() { return buffer_; }

    void process_irq();

    private:
    void configure_gpio();
    void setup_pio();
    void install_irq();

    PIO pio_;
    uint sm_;
    uint pinA_, pinB_;
    float clkdiv_;
    uint offset_;
    EncoderRingBuffer buffer_;
    uint32_t debugLedPin_;
};
