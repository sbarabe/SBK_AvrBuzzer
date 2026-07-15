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

void setup()
{
    buzzer.begin();
}

void loop()
{
    buzzer.playTone(1000, 250);

    while (buzzer.isPlaying())
    {
        buzzer.update();
    }

    delay(500);
}
```

---

## Playing a Chirp

```cpp
buzzer.playChirp(500, 2000, 1000);
```

This produces a linear sweep from **500 Hz** to **2000 Hz** over **1 second**.

---

## Differential Output

For increased sound output, connect the buzzer between two GPIO pins.

```cpp
Buzzer buzzer(9, 10, OutputMode::DIFFERENTIAL);
```

The library automatically drives both pins in opposite states, effectively doubling the voltage across a passive piezo buzzer.

If the second pin is omitted (or `NO_PIN` is used), the library automatically falls back to standard single-ended operation.

> **Important:** Differential mode must only be used with **passive** piezo buzzers. Active buzzers contain internal drive electronics and must be driven from a single output.

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

## API

### Playback

```cpp
begin()
playTone()
playChirp()
stop()
update()
```

### Configuration

```cpp
setMode()
enableDifferential()
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