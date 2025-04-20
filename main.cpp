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
    QuadratureEncoder encoder(pio0, 0, 10, 1.0f);
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
#if 0

#include <array>
#include <cstdio>

#include "QuadratureEncoder.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    sleep_ms(500);

    QuadratureEncoder enc0(pio0, 0, 10, 1.0f);
    QuadratureEncoder enc1(pio0, 3, 27, 1.0f);

    enc0.enableDebugLed(20);

    enc0.init();
    enc1.init();

    std::array<int64_t, 2> pos = {0, 0};
    while (true) {
        uint8_t step;
        if (enc0.buffer().pop(step)) {
            pos[0] += static_cast<int8_t>(step);
            printf("E0 %+d → %lld\n", static_cast<int8_t>(step), pos[0]);
        }
        if (enc1.buffer().pop(step)) {
            pos[1] += static_cast<int8_t>(step);
            printf("E1 %+d → %lld\n", static_cast<int8_t>(step), pos[1]);
        }
    }
    return 0;
}

#endif

#if 0
#include <cstdio>

#include "QuadratureEncoder.h"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    sleep_ms(500);

    QuadratureEncoder enc0(pio0, 0, 10);
    enc0.enableDebugLed(20);  // optional
    enc0.init();

    QuadratureEncoder enc1(pio1, 3, 27);
    enc1.enableDebugLed(22);  // optional
    enc1.init();

    int64_t pos0 = 0, pos1 = 0;
    while (true) {
        uint8_t step;
        while (enc0.buffer().pop(step)) {
            pos0 += static_cast<int8_t>(step);
            printf("E0 %+d → %lld\n", static_cast<int8_t>(step), pos0);
        }
        while (enc1.buffer().pop(step)) {
            pos1 += static_cast<int8_t>(step);
            printf("E1 %+d → %lld\n", static_cast<int8_t>(step), pos1);
        }
    }
}
#endif
