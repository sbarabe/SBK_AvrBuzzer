# SBK_AvrBuzzer
**SBK_AvrBuzzer** is a lightweight, non-blocking library for driving passive piezo buzzers on AVR-based Arduino boards. It uses Timer1 to generate accurate square waves without blocking your application, supports both single-ended and differential output modes, and provides simple functions for playing tones and frequency chirps.

**Important:** This library exclusively uses Timer1 and cannot be used simultaneously with other Timer1-based libraries (Servo, some motor libraries, etc.)

Tone generation automatically adapts to the board clock frequency through the Arduino F_CPU definition. The library has been primarily tested at 8 MHz and should also operate correctly on standard 16 MHz AVR Arduino boards

Supported boards

- Arduino Uno
- Arduino Nano
- Arduino Pro Mini
- ATmega328P
- ATmega328PB
- Other AVR boards providing Timer1

Not supported

- ESP32
- RP2040
- SAMD boards
