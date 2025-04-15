
#include "QuadratureEncoder.h"

#include <cstdio>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "quadrature_encoder.pio.h"

#define DEBUG_LED_PIN 20

// QuadratureEncoder* QuadratureEncoder::instance_ = nullptr;
extern QuadratureEncoder* encoder_ptr;
QuadratureEncoder::QuadratureEncoder(PIO pio, uint sm, uint gpioA, float clkdiv)
    : pio_(pio), sm_(sm), gpioA_(gpioA), gpioB_(gpioA + 1), clkdiv_(clkdiv) {
    // instance_ = this;
}

void QuadratureEncoder::configure_gpio() {
    gpio_init(gpioA_);
    gpio_set_dir(gpioA_, GPIO_IN);
    gpio_pull_up(gpioA_);

    gpio_init(gpioB_);
    gpio_set_dir(gpioB_, GPIO_IN);
    gpio_pull_up(gpioB_);

    gpio_init(DEBUG_LED_PIN);
    gpio_set_dir(DEBUG_LED_PIN, GPIO_OUT);
    gpio_put(DEBUG_LED_PIN, 0);
}

void QuadratureEncoder::install_irq() {
    if (pio_ == pio0) {
        irq_set_exclusive_handler(PIO0_IRQ_0, irq_handler);
        irq_set_enabled(PIO0_IRQ_0, true);
        pio_set_irq0_source_enabled(pio_, pis_interrupt0, true);
    } else {
        irq_set_exclusive_handler(PIO1_IRQ_0, irq_handler);
        irq_set_enabled(PIO1_IRQ_0, true);
        pio_set_irq1_source_enabled(pio_, pis_interrupt0, true);
    }
}

void QuadratureEncoder::init() {
    configure_gpio();

    offset_ = pio_add_program(pio_, &quadrature_encoder_program);

    pio_sm_config c = quadrature_encoder_program_get_default_config(offset_);
    sm_config_set_in_pins(&c, gpioA_);
    pio_sm_set_consecutive_pindirs(pio_, sm_, gpioA_, 2, false);
    sm_config_set_in_shift(&c, true, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio_, sm_, offset_, &c);
    pio_sm_set_clkdiv(pio_, sm_, clkdiv_);
    pio_sm_clear_fifos(pio_, sm_);
    pio_sm_restart(pio_, sm_);

    install_irq();
    printf("GPIO %d = %d | GPIO %d = %d\n", gpioA_, gpio_get(gpioA_), gpioB_,
           gpio_get(gpioB_));
    printf("Enabling SM now\n");
    printf("SM enabled: %d\n", (pio_->ctrl >> sm_) & 1);

    pio_sm_set_enabled(pio_, sm_, true);
    printf("SM enabled: %d\n", (pio_->ctrl >> sm_) & 1);
}

void QuadratureEncoder::handle_interrupt() {
    pio_interrupt_clear(pio_, 0);
    gpio_xor_mask(1u << DEBUG_LED_PIN);  // Toggle LED

    while (!pio_sm_is_rx_fifo_empty(pio_, sm_)) {
        uint8_t ab = pio_sm_get(pio_, sm_) & 0x03;
        uint8_t b = (ab >> 1) & 0x01;

        int8_t direction = (b == 0) ? +1 : -1;
        buffer_.push(static_cast<uint8_t>(direction));
    }
}

void QuadratureEncoder::irq_handler() {
    if (encoder_ptr) {
        encoder_ptr->handle_interrupt();
    }
}

EncoderRingBuffer& QuadratureEncoder::buffer() { return buffer_; }
