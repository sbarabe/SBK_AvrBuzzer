# SBK_AvrBuzzer
**SBK_AvrBuzzer** is a lightweight, non-blocking library for driving passive piezo buzzers on AVR-based Arduino boards. It uses Timer1 to generate accurate square waves without blocking your application, supports both single-ended and differential output modes, and provides simple functions for playing tones and frequency chirps.

Tone generation automatically adapts to the board clock frequency through the Arduino F_CPU definition. The library has been primarily tested at 8 MHz and should also operate correctly on standard 16 MHz AVR Arduino boards
