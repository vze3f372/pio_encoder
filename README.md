
# PIO Quadrature Encoder Library for Raspberry Pi Pico / Pico W / Pico 2 W

A lightweight C++ library and example for reading rotary quadrature encoders using the RP2040’s PIO hardware, with support for up to **4 state‑machines per PIO block** (8 total on PIO0 + PIO1), an optional per‑encoder debug LED, and a lock‑free ring buffer.

---

## Table of Contents

- [Features](#features)  
- [Hardware Setup](#hardware-setup)  
- [Software Requirements](#software-requirements)  
- [Building](#building)  
- [Usage](#usage)  
  - [Class Usage](#class-usage)  
  - [Instantiating an Encoder](#instantiating-an-encoder)  
  - [Optional Debug LED](#optional-debug-led)  
  - [Reading Steps](#reading-steps)  
- [Multiple Encoders (PIO0 + PIO1)](#multiple-encoders-pio0--pio1)  
- [API Reference](#api-reference)  
- [License (Beerware)](#license-beerware)

---

## Features

- Pure‑C++ wrapper around RP2040 PIO for quadrature decoding  
- Hardware‑driven: uses PIO IRQ0 vector, minimal CPU overhead  
- Supports up to 4 encoders on PIO0 and 4 on PIO1 (8 total)  
- Interrupt‑safe ring buffer (`EncoderRingBuffer`) for step events  
- Optional per‑encoder debug LED toggle on activity  
- Simple, idiomatic API with drop‑in `main.cpp` examples  

---

## Hardware Setup

1. **Encoder pins**  
   - Connect your encoder’s **phase A** to GP<n> and **phase B** to GP<n+1>.  
   - You can use up to four pairs on PIO0 and four on PIO1.

2. **(Optional) Debug LEDs**  
   - Wire each debug LED (with resistor) to a free GPIO.  
   - Call `enableDebugLed(pin)` before `init()` to have it blink on each step.

3. **Pull‑ups**  
   - On‑board or external pull‑ups on A and B lines help clean up the signal.

---

## Software Requirements

- Raspberry Pi Pico SDK (2024.xx or later)  
- CMake ≥ 3.13  
- GNU Make or Ninja  
- ARM GCC toolchain (arm-none-eabi‑gcc)  

---

## Building

```bash
git clone https://github.com/youruser/pico-quadrature-encoder.git
cd pico-quadrature-encoder
mkdir build && cd build
cmake -DPICO_BOARD=<board type> ..
make -j$(nproc)

