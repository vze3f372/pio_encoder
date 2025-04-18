#include "QuadratureEncoder.h"

#include <cstdio>

#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "quadrature_encoder.pio.h"

#define DEBUG_LED 20

QuadratureEncoder* QuadratureEncoder::instance_ = nullptr;

extern "C" void pio0_irq_0_handler() {
    if (!QuadratureEncoder::instance_) return;
    QuadratureEncoder::instance_->process_irq();
}

QuadratureEncoder::QuadratureEncoder(PIO pio, uint sm, uint pinA, float clkdiv)
    : pio_(pio),
      sm_(sm),
      pinA_(pinA),
      pinB_(pinA + 1),
      clkdiv_(clkdiv),
      offset_(0) {}

void QuadratureEncoder::init() {
    gpio_init(pinA_);
    gpio_set_dir(pinA_, GPIO_IN);
    gpio_init(pinB_);
    gpio_set_dir(pinB_, GPIO_IN);

    //   gpio_init(DEBUG_LED);
    //   gpio_set_dir(DEBUG_LED, GPIO_OUT);
    //   gpio_put(DEBUG_LED, 0);

    offset_ = pio_add_program(pio_, &quadrature_encoder_program);
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset_);
    sm_config_set_in_pins(&c, pinA_);
    pio_sm_set_consecutive_pindirs(pio_, sm_, pinA_, 2, false);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio_, sm_, offset_, &c);
    pio_sm_set_clkdiv(pio_, sm_, clkdiv_);

    instance_ = this;
    pio_interrupt_clear(pio_, 0);
    uint irq_num = (pio_ == pio0 ? PIO0_IRQ_0 : PIO1_IRQ_0);
    irq_set_exclusive_handler(irq_num, pio0_irq_0_handler);
    irq_set_enabled(irq_num, true);
    pio_set_irq0_source_enabled(pio_, pis_interrupt0, true);

    pio_sm_clear_fifos(pio_, sm_);
    pio_sm_restart(pio_, sm_);
    pio_sm_set_enabled(pio_, sm_, true);

    printf("[init] PIO%s SM%u enabled, INTE0=0x%02lx\n",
           (pio_ == pio0 ? "0" : "1"), sm_, (uint32_t)pio_->inte0);
}

void QuadratureEncoder::process_irq() {
    pio_interrupt_clear(pio_, 0);

    gpio_xor_mask(1u << DEBUG_LED);

    while (!pio_sm_is_rx_fifo_empty(pio_, sm_)) {
        uint8_t ab = pio_sm_get(pio_, sm_) & 0x03;
        int8_t dir = ((ab >> 1) & 1) ? +1 : -1;
        buffer_.push(static_cast<uint8_t>(dir));
    }
}

EncoderRingBuffer& QuadratureEncoder::buffer() { return buffer_; }
