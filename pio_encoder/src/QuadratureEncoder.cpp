#include "QuadratureEncoder.h"

#include <cstdio>
#include <string>

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "quadrature_encoder.pio.h"

static QuadratureEncoder* table0[QuadratureEncoder::MAX_SMS] = {nullptr};
static QuadratureEncoder* table1[QuadratureEncoder::MAX_SMS] = {nullptr};

extern "C" void __isr pio0_irq_0_handler() {
    pio_interrupt_clear(pio0, 0);
    for (int i = 0; i < QuadratureEncoder::MAX_SMS; ++i) {
        if (table0[i]) table0[i]->process_irq();
    }
}

extern "C" void __isr pio1_irq_0_handler() {
    pio_interrupt_clear(pio1, 0);
    for (int i = 0; i < QuadratureEncoder::MAX_SMS; ++i) {
        if (table1[i]) table1[i]->process_irq();
    }
}

QuadratureEncoder::QuadratureEncoder(PIO pio, uint sm, uint pinA, float clkdiv)
    : pio_(pio),
      sm_(sm),
      pinA_(pinA),
      pinB_(pinA + 1),
      clkdiv_(clkdiv),
      offset_(0),
      buffer_(),
      debugLedPin_(NO_LED) {}

void QuadratureEncoder::enableDebugLed(uint32_t led_pin) {
    debugLedPin_ = led_pin;
}
void QuadratureEncoder::disableDebugLed() { debugLedPin_ = NO_LED; }

void QuadratureEncoder::init() {
    gpio_init(pinA_);
    gpio_set_dir(pinA_, GPIO_IN);
    gpio_init(pinB_);
    gpio_set_dir(pinB_, GPIO_IN);
    if (debugLedPin_ != NO_LED) {
        gpio_init(debugLedPin_);
        gpio_set_dir(debugLedPin_, GPIO_OUT);
        gpio_put(debugLedPin_, 0);
    }

    offset_ = pio_add_program(pio_, &quadrature_encoder_program);
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset_);
    sm_config_set_in_pins(&c, pinA_);
    pio_sm_set_consecutive_pindirs(pio_, sm_, pinA_, 2, false);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio_, sm_, offset_, &c);
    pio_sm_set_clkdiv(pio_, sm_, clkdiv_);

    auto* table = (pio_ == pio0 ? table0 : table1);
    table[sm_] = this;
    pio_interrupt_clear(pio_, 0);
    static bool inst0 = false, inst1 = false;
    if (pio_ == pio0 && !inst0) {
        irq_set_exclusive_handler(PIO0_IRQ_0, pio0_irq_0_handler);
        irq_set_enabled(PIO0_IRQ_0, true);
        pio_set_irq0_source_enabled(pio0, pis_interrupt0, true);
        inst0 = true;
    } else if (pio_ == pio1 && !inst1) {
        irq_set_exclusive_handler(PIO1_IRQ_0, pio1_irq_0_handler);
        irq_set_enabled(PIO1_IRQ_0, true);
        pio_set_irq0_source_enabled(pio1, pis_interrupt0, true);
        inst1 = true;
    }

    pio_sm_clear_fifos(pio_, sm_);
    pio_sm_restart(pio_, sm_);
    pio_sm_set_enabled(pio_, sm_, true);

    int idx = (pio_ == pio0 ? 0 : 1);
    printf("[init] PIO%d.SM%u → LED %s\n", idx, sm_,
           debugLedPin_ == NO_LED ? "disabled"
                                  : std::to_string(debugLedPin_).c_str());
}

void QuadratureEncoder::process_irq() {
    bool any = false;
    while (!pio_sm_is_rx_fifo_empty(pio_, sm_)) {
        any = true;
        uint8_t ab = pio_sm_get(pio_, sm_) & 0x03;
        int8_t dir = ((ab >> 1) & 1) ? +1 : -1;
        buffer_.push(static_cast<uint8_t>(dir));
    }
    if (any && debugLedPin_ != NO_LED) {
        gpio_xor_mask(1u << debugLedPin_);
    }
}
