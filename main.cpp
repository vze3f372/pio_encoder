#include <stdio.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "LED.h"

#if 0

#include <memory>

int main() {
    stdio_init_all();

    std::array<std::unique_ptr<LED>, 3> leds = {std::make_unique<LED>(22),
                                                std::make_unique<LED>(21),
                                                std::make_unique<LED>(20)};

    const uint8_t led_mask = 0x01;
    uint64_t count = 0;

    while (true) {
        count = (count + 1) % 8;
        size_t i = 0;
        for (auto& led : leds) {
            led->value(count & (led_mask << i));
            i++;
        }

        sleep_ms(250);
    }
}
#endif

#if 0
int main() {
    stdio_init_all();
    const uint8_t led_mask = 0x01;
    std::array<LED, 3> leds = {LED(20), LED(21), LED(22)};
    uint64_t count = 0;

    while (true) {
        count = (count + 1) % 8;
        size_t i = 0;
        for (auto& led : leds) {
            leds[i].value(count & (led_mask << i));
            ++i;
        }
        sleep_ms(250);
    }
}

#endif

#if 1

#include "EncoderRingBuffer.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "quadrature_encoder.pio.h"

#define ENCODER_PIO pio0
#define ENCODER_SM 0
#define ENCODER_IRQ PIO0_IRQ_0
#define ENCODER_PIN_A 10
#define ENCODER_PIN_B 11

EncoderRingBuffer encoder_buffer;

volatile uint32_t irq_hits = 0;

void quadrature_encoder_program_init(PIO pio, uint sm, uint offset,
                                     uint pin_a) {
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_a);
    pio_sm_set_consecutive_pindirs(pio, sm, pin_a, 2, false);
    sm_config_set_in_shift(&c, true, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}

void __isr encoder_irq_handler() {
    ++irq_hits;
    pio_interrupt_clear(ENCODER_PIO, 0);
}

void encoder_pio_init() {
    uint offset = pio_add_program(ENCODER_PIO, &quadrature_encoder_program);
    quadrature_encoder_program_init(ENCODER_PIO, ENCODER_SM, offset,
                                    ENCODER_PIN_A);
    irq_set_exclusive_handler(ENCODER_IRQ, encoder_irq_handler);
    irq_set_enabled(ENCODER_IRQ, true);
    pio_set_irq0_source_enabled(ENCODER_PIO, pis_interrupt0, true);
    pio_sm_set_enabled(ENCODER_PIO, ENCODER_SM, true);
    pio_sm_clear_fifos(ENCODER_PIO, ENCODER_SM);
    pio_sm_restart(ENCODER_PIO, ENCODER_SM);
}

int main() {
    gpio_init(ENCODER_PIN_A);
    gpio_set_dir(ENCODER_PIN_A, GPIO_IN);

    gpio_init(ENCODER_PIN_B);
    gpio_set_dir(ENCODER_PIN_B, GPIO_IN);

    stdio_init_all();
    sleep_ms(500);
    encoder_pio_init();

    int encoder_position = 0;
    uint8_t last_state = 0;

    while (true) {
        printf("%d\n", irq_hits);
        sleep_ms(5);
    }
}
#endif
