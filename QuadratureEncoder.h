
#pragma once

#include <cstdint>

#include "EncoderRingBuffer.h"
#include "hardware/pio.h"

class QuadratureEncoder {
    public:
    static QuadratureEncoder* instance_;

    void process_irq();

    QuadratureEncoder(PIO pio, uint sm, uint pinA, float clkdiv = 250000.0f);

    void init();

    EncoderRingBuffer& buffer();

    private:
    PIO pio_;
    uint sm_;
    uint pinA_, pinB_;
    float clkdiv_;
    uint offset_;

    EncoderRingBuffer buffer_;
};
