/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#define PICO_DEFAULT_LED_PIN 20

int main() {
#ifndef PICO_DEFAULT_LED_PIN

#warning blink example requires a board with a regular LED
#else
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  bool val = false;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (true) {
    val = !val;
    gpio_put(LED_PIN, val);
    sleep_ms(500);
    val = !val;
    gpio_put(LED_PIN, val);
    sleep_ms(500);
  }
#endif
}
