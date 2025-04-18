#include <stdio.h>

#include <array>
#include <cstddef>
#include <cstdint>

#include "LED.h"
#include "hardware/clocks.h"

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

#include <cstdio>
#include <memory>

#include "QuadratureEncoder.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    sleep_ms(500);
    std::array<std::unique_ptr<LED>, 3> leds = {std::make_unique<LED>(22),
                                                std::make_unique<LED>(21),
                                                std::make_unique<LED>(20)};
    static QuadratureEncoder encoder(pio0, 0, 27, 250000.0f);
    encoder.init();
    const uint8_t led_mask = 0x01;
    int64_t position = 0;
    uint8_t raw = 0;
    while (true) {
        while (encoder.buffer().pop(raw)) {
            int8_t step = static_cast<int8_t>(raw);
            position += step;
            size_t i = 0;
            for (auto& led : leds) {
                led->value(position & (led_mask << i));
                i++;
            }

            printf("Step: %+d  Position: %lld\n", step, position);
        }
    }
}

#endif
