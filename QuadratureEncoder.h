
#pragma once

#include "EncoderRingBuffer.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/time.h"

class QuadratureEncoder {
    public:
    QuadratureEncoder(PIO pio, uint sm, uint gpioA, float clkdiv = 250000.0f);
    void init();
    void handle_interrupt();
    EncoderRingBuffer& buffer();

    private:
    PIO pio_;
    uint sm_;
    uint gpioA_;
    uint gpioB_;
    float clkdiv_;
    uint offset_;
    EncoderRingBuffer buffer_;
    //    static QuadratureEncoder* instance_;

    void configure_gpio();
    void install_irq();

    static void irq_handler();
};
