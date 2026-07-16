# SBK_AvrBuzzer

**SBK_AvrBuzzer** is a lightweight, non-blocking library for driving passive piezoelectric buzzers on AVR-based Arduino boards.

Unlike Arduino's built-in `tone()` function, SBK_AvrBuzzer generates hardware-timed square waves using **Timer1**, allowing your application to continue running while **tones**, **chirps**, and **complete musical melodies** are playing.

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
- Non-blocking melody playback with `TunePlayer`
- `PROGMEM` tune definitions
- Random and repeat tune playback

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
    sbarabe/SBK_AvrBuzzer
```

---

## Examples

The library includes the following example sketches:

- 01_PlayTone
- 02_PlayChirp
- ...
- 06_TunePlayer

### Basic Example

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

### Playing a Chirp

```cpp
buzzer.playChirp(500, 2000, 1000);
```

This produces a linear sweep from **500 Hz** to **2000 Hz** over **1 second**.

---

### TunePlayer

The optional `TunePlayer` class provides non-blocking playback of musical melodies.

Applications that only need tones or chirps do not need to include or instantiate `TunePlayer`.

```cpp
#include <SBK_AvrBuzzer.h>
#include <SBK_Tune.h>
#include <SBK_TunePlayer.h>

Buzzer buzzer(9);
TunePlayer tunePlayer(buzzer);

SBK_DEFINE_TUNE(TUNE_STARTUP, 140,
    NOTE_C5, D_8,
    NOTE_E5, D_8,
    NOTE_G5, D_4
);

void setup()
{
    buzzer.begin();

    tunePlayer.play(TUNE_STARTUP);
}

void loop()
{
    buzzer.update();
    tunePlayer.update();
}
```

Tune definitions are stored in program memory (`PROGMEM`) to minimize SRAM usage on AVR microcontrollers.

#### Defining Tunes

Tunes are declared using the `SBK_DEFINE_TUNE()` helper macro.

```cpp
SBK_DEFINE_TUNE(TUNE_NOTIFICATION, 120,
    NOTE_C5, D_8,
    NOTE_G5, D_8,
    NOTE_C6, D_4
);
```

A tune is declared by specifying its **name**, **tempo (BPM)** followed by alternating **note** and **duration** values.

```cpp
SBK_DEFINE_TUNE(name, bpm,
    NOTE, DURATION,
    NOTE, DURATION,
    ...
);
```

---

## Typical Usage Pattern

Call `update()` every iteration of `loop()`.

```cpp
void loop()
{
    buzzer.update();
    tunePlayer.update();

    // Rest of your application
}
```

Tone generation is handled by Timer1 interrupts. `Buzzer::update()` manages tone duration and chirp frequency changes, while `TunePlayer::update()` schedules melody playback.

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

### Buzzer

#### Playback

```cpp
begin()
playTone()
playChirp()
stop()
update()
```

#### Status

```cpp
isPlaying()
isMuted()
isDifferential()
```

#### Sound Control

```cpp
mute()
unmute()
```

### TunePlayer

`TunePlayer` controls melody sequencing and uses a `Buzzer` instance to generate each note.

#### Playback

```cpp
play()
playRandom()
repeat()
stop()
update()
```

#### Status

```cpp
isPlaying()
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

Copyright (c) 2026 Samuel Barabe (Smart Builds & Kits).