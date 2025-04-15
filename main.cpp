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
#include "LED.h"
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
LED* isr_led = nullptr;
void quadrature_encoder_program_init(PIO pio, uint sm, uint offset,
                                     uint pin_a) {
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_a);
    pio_sm_set_consecutive_pindirs(pio, sm, pin_a, 2, false);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_clkdiv(pio0, sm, 250000.0f);
}
void __isr encoder_irq_handler() {
    pio_interrupt_clear(ENCODER_PIO, 0);

    isr_led->toggle();
    ++irq_hits;
    uint8_t direction = 0;
    while (!pio_sm_is_rx_fifo_empty(pio0, ENCODER_SM)) {
        uint8_t ab = pio_sm_get(pio0, ENCODER_SM) & 0x03;
        uint8_t b = (ab >> 1) & 0x01;

        direction = (b == 1) ? +1 : -1;
        encoder_buffer.push(static_cast<uint8_t>(direction));  // store +1 or
    }
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
    //   PIO pio = pio0;
    LED led_debug(20);
    isr_led = &led_debug;
    uint offset = pio_add_program(ENCODER_PIO, &quadrature_encoder_program);
    pio_sm_config c = quadrature_encoder_program_get_default_config(offset);

    gpio_init(ENCODER_PIN_A);
    gpio_set_dir(ENCODER_PIN_A, GPIO_IN);
    gpio_init(ENCODER_PIN_B);
    gpio_set_dir(ENCODER_PIN_B, GPIO_IN);
    stdio_init_all();
    sleep_ms(500);

    encoder_pio_init();

    int64_t encoder_position = 0;

    while (true) {
        uint8_t raw;
        while (encoder_buffer.pop(raw)) {
            int8_t value = static_cast<int8_t>(raw);
            encoder_position += value;
            if (encoder_position > 100) {
                encoder_position = 100;
            } else if (encoder_position < 0) {
                encoder_position = 0;
            }
            printf("Step: %+d | Position: %d\n", value, encoder_position);
        }
    }
}

#endif
#if 0

#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "quadrature_encoder.pio.h"
#include "test_loop_push.pio.h"  // <--- your test program

#define PIO pio0
#define SM 0
#define PIN_BASE 10

int main() {
    stdio_init_all();
    sleep_ms(500);

    gpio_init(PIN_BASE);
    gpio_set_dir(PIN_BASE, GPIO_IN);
    gpio_pull_up(PIN_BASE);

    gpio_init(PIN_BASE + 1);
    gpio_set_dir(PIN_BASE + 1, GPIO_IN);
    gpio_pull_up(PIN_BASE + 1);

    uint offset = pio_add_program(PIO, &test_loop_push_program);
    pio_sm_config c = test_loop_push_program_get_default_config(offset);
    sm_config_set_in_pins(&c, PIN_BASE);
    pio_sm_set_consecutive_pindirs(PIO, SM, PIN_BASE, 2, false);
    sm_config_set_in_shift(&c, false, false, 32);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(PIO, SM, offset, &c);
    pio_sm_set_clkdiv(PIO, SM, 250000.0f);
    pio_sm_clear_fifos(PIO, SM);
    pio_sm_restart(PIO, SM);
    pio_sm_set_enabled(PIO, SM, true);

    printf("Running PIO test...\n");

    while (true) {
        if (!pio_sm_is_rx_fifo_empty(PIO, SM)) {
            uint32_t raw = pio_sm_get(PIO, SM);
            printf("RAW: 0x%08lx  Pins: %02lx\n", raw, raw & 0x03);
        }
        sleep_ms(50);
    }
}

#endif
#if 0
#include "QuadratureEncoder.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "quadrature_encoder.pio.h"

QuadratureEncoder* encoder_ptr = nullptr;  // 🌟 global pointer

void __isr encoder_irq_handler() {
    if (encoder_ptr) {
        encoder_ptr->handle_interrupt();
    }
}

int main() {
    stdio_init_all();
    sleep_ms(500);

    static QuadratureEncoder encoder(pio0, 0, 10, 250000.0f);
    encoder_ptr = &encoder;  // 🧠 assign before enabling PIO

    // Instead of install_irq() in the class, do it here:
    // irq_set_exclusive_handler(PIO0_IRQ_0, encoder_irq_handler);
    // irq_set_enabled(PIO0_IRQ_0, true);

    encoder.init();

    int position = 0;
    while (true) {
        uint8_t val;
        while (encoder.buffer().pop(val)) {
            position += static_cast<int8_t>(val);
            printf("Step: %+d | Position: %d\n", val, position);
        }
    }
}
#endif
