# SBK_AvrBuzzer

**SBK_AvrBuzzer** is a lightweight, non-blocking library for driving passive piezoelectric buzzers on AVR-based Arduino boards.

Unlike Arduino's built-in `tone()` function, SBK_AvrBuzzer generates hardware-timed square waves using **Timer1**, allowing your application to continue running while tones or frequency chirps are playing.

The library supports both **single-ended (1-pin)** and **differential (2-pin)** output modes, making it suitable for everything from simple buzzers to louder differential-drive piezo configurations.

---

## Features

- Non-blocking tone playback
- Hardware-timed waveform generation using Timer1
- Fixed-frequency tones
- Linear frequency chirps
- Single-ended (1-pin) output
- Differential (2-pin) output
- Runtime output mode switching
- Mute / unmute support
- Automatic adaptation to the board `F_CPU`

---

## Supported Boards

SBK_AvrBuzzer is designed for **AVR Arduino boards** that provide **Timer1**, including:

- Arduino Uno
- Arduino Nano
- Arduino Pro Mini
- ATmega328P
- ATmega328PB
- Most AVR boards with Timer1

### Not Supported

- ESP32
- RP2040
- SAMD boards
- Any non-AVR architecture

---

## Important

SBK_AvrBuzzer **exclusively uses Timer1**.

It cannot be used simultaneously with other libraries that also configure Timer1, such as:

- Servo
- Some motor-control libraries
- Other Timer1-based libraries

Create only one `Buzzer` instance per sketch.

All `Buzzer` objects share Timer1, so multiple instances are not supported.

---

## Installation

### Arduino Library Manager

Search for:

```
SBK_AvrBuzzer
```

or install the latest release manually from GitHub.

### PlatformIO

```ini
lib_deps =
    SBK_AvrBuzzer
```

---

## Basic Example

```cpp
#include <SBK_AvrBuzzer.h>

Buzzer buzzer(9);

uint32_t lastToneTime = 0;

void setup()
{
    buzzer.begin();
}

void loop()
{
    buzzer.update();

    if (millis() - lastToneTime >= 1000)
    {
        lastToneTime = millis();
        buzzer.playTone(1000, 250);
    }

    // Other application code can run here.
}
```

---

## Playing a Chirp

```cpp
buzzer.playChirp(500, 2000, 1000);
```

This produces a linear sweep from **500 Hz** to **2000 Hz** over **1 second**.

---

## Typical Usage Pattern

Call `update()` every iteration of `loop()`.

```cpp
void loop()
{
    buzzer.update();

    // Rest of your application
}
```

Tone generation itself is handled by Timer1 interrupts, while `update()` manages playback duration and chirp frequency changes.

---

## Wiring and Configuration

### Single-ended output (default)

Connect one side of the passive piezo buzzer to a GPIO pin and the other side to GND.

```cpp
Buzzer buzzer(9);
```

or

```cpp
Buzzer buzzer(9, NO_PIN, OutputMode::SINGLE_ENDED);
```

---

### Differential output

For increased sound output, connect the buzzer between two GPIO pins.

```cpp
Buzzer buzzer(9, 10, OutputMode::DIFFERENTIAL);
```

Both pins are driven with opposite polarities, approximately doubling the peak-to-peak voltage applied across the passive piezo element compared with single-ended operation.

> **Note:** Differential mode is enabled only when `pin2` is not set to `NO_PIN`. If `pin2` is omitted or set to `NO_PIN`, the library automatically falls back to single-ended operation. The supplied pin numbers must be valid digital GPIO pins for the selected board.

> **Warning:** Differential mode is intended **only for passive piezoelectric buzzers**. Do not use it with active buzzers, which contain internal drive electronics.

---

## API

### Playback

```cpp
begin()
playTone()
playChirp()
stop()
update()
```

### Status

```cpp
isPlaying()
isMuted()
isDifferential()
```

### Sound Control

```cpp
mute()
unmute()
```

---

## Limitations

- AVR microcontrollers only
- Requires Timer1
- One active buzzer instance at a time
- Designed for passive piezoelectric buzzers

---

## License

This project is released under the MIT License.

Copyright (c) 2026 Samuel Barabe.