
#include "QuadratureEncoder.h"

#include <cstdio>

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "quadrature_encoder.pio.h"

// one slot per SM on PIO0
static QuadratureEncoder* instances[QuadratureEncoder::MAX_SMS] = {};

// ISR stub for PIO0 user‑IRQ0
extern "C" void __isr pio0_irq_0_handler() {
    pio_interrupt_clear(pio0, 0);
    for (int i = 0; i < QuadratureEncoder::MAX_SMS; ++i) {
        if (instances[i]) {
            instances[i]->process_irq();
        }
    }
}

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------
QuadratureEncoder::QuadratureEncoder(PIO pio, uint sm, uint pinA, float clkdiv)
    : pio_(pio),
      sm_(sm),
      pinA_(pinA),
      pinB_(pinA + 1),
      clkdiv_(clkdiv),
      offset_(0),
      buffer_(),
      debugLedPin_(NO_LED)  // LED disabled by default
{}

void QuadratureEncoder::enableDebugLed(uint32_t led_pin) {
    debugLedPin_ = led_pin;
}
void QuadratureEncoder::disableDebugLed() { debugLedPin_ = NO_LED; }

void QuadratureEncoder::init() {
    configure_gpio();
    setup_pio();
    install_irq();

    // flush & start
    pio_sm_clear_fifos(pio_, sm_);
    pio_sm_restart(pio_, sm_);
    pio_sm_set_enabled(pio_, sm_, true);

    printf("[init] PIO%u SM%u ready (LED %s)\n", (pio_ == pio0 ? 0 : 1), sm_,
           (debugLedPin_ == NO_LED ? "disabled"
                                   : "on pin " + std::to_string(debugLedPin_)));
}

void QuadratureEncoder::configure_gpio() {
    gpio_init(pinA_);
    gpio_set_dir(pinA_, GPIO_IN);
    gpio_init(pinB_);
    gpio_set_dir(pinB_, GPIO_IN);

    if (debugLedPin_ != NO_LED) {
        gpio_init(debugLedPin_);
        gpio_set_dir(debugLedPin_, GPIO_OUT);
        gpio_put(debugLedPin_, 0);
    }
}

void QuadratureEncoder::setup_pio() {
    offset_ = pio_add_program(pio_, &quadrature_encoder_program);
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset_);
    sm_config_set_in_pins(&c, pinA_);
    pio_sm_set_consecutive_pindirs(pio_, sm_, pinA_, 2, false);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

    pio_sm_init(pio_, sm_, offset_, &c);
    pio_sm_set_clkdiv(pio_, sm_, clkdiv_);
}

void QuadratureEncoder::install_irq() {
    instances[sm_] = this;  // register pointer

    pio_interrupt_clear(pio_, 0);  // clear stale flag

    static bool installed = false;
    if (!installed) {
        irq_set_exclusive_handler(PIO0_IRQ_0, pio0_irq_0_handler);
        irq_set_enabled(PIO0_IRQ_0, true);
        pio_set_irq0_source_enabled(pio_, pis_interrupt0, true);
        installed = true;
    }
}

void QuadratureEncoder::process_irq() {
    if (debugLedPin_ != NO_LED) {
        gpio_xor_mask(1u << debugLedPin_);
    }
    while (!pio_sm_is_rx_fifo_empty(pio_, sm_)) {
        uint8_t ab = pio_sm_get(pio_, sm_) & 0x03;
        int8_t dir = ((ab >> 1) & 1) ? +1 : -1;
        buffer_.push(static_cast<uint8_t>(dir));
    }
}
