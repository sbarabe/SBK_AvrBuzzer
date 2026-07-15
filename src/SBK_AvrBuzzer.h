#pragma once

#include <Arduino.h>

//=====================================================================
// SBK_AvrBuzzer Library
//=====================================================================
//
// SPDX-License-Identifier: MIT
//
// Non-blocking passive piezo buzzer library for AVR-based Arduino
// boards.
//
// SBK_AvrBuzzer uses Timer1 to generate accurate square-wave tones
// without blocking the main application. Tone generation is performed
// by a hardware timer interrupt, allowing the rest of the sketch to
// continue running while audio is playing.
//
// Features:
//   - Non-blocking tone playback
//   - Hardware-timed square-wave generation using Timer1
//   - Frequency chirps
//   - Single-ended (1-pin) and differential (2-pin) output
//   - Mute support
//
// Notes:
//   - Designed for passive piezoelectric buzzers.
//   - Differential mode must not be used with active buzzers,
//     as they contain internal drive electronics.
//   - Uses Timer1 exclusively. Do not use Timer1 for another feature
//     at the same time.
//
// Copyright (c) 2026 Samuel Barabe
// Licensed under the MIT License.
//=====================================================================

//=====================================================================
// Compile-Time Safeguards
//=====================================================================

// Ensure the selected Arduino board defines the CPU clock frequency.
#ifndef F_CPU
#error "SBK_AvrBuzzer requires F_CPU to be defined by the selected Arduino board."
#endif

// This library directly accesses AVR Timer1 registers.
#if !defined(__AVR__)
#error "SBK_AvrBuzzer supports AVR microcontrollers only."
#endif

#if !defined(TCCR1A) || !defined(TCCR1B)
#error "SBK_AvrBuzzer requires an AVR microcontroller with Timer1."
#endif

// Special value indicating that no second GPIO pin is used.
static constexpr uint8_t NO_PIN = 255;

enum class OutputMode
{
    SINGLE_ENDED,
    DIFFERENTIAL
};

class Buzzer
{
public:
    // Creates a buzzer object.
    //
    // pin1
    //     Primary GPIO connected to the buzzer.
    //
    // pin2
    //     Optional second GPIO connected to the buzzer.
    //     Default = NO_PIN.
    //     Use NO_PIN for standard 1-pin operation.
    //
    // mode
    //     Operating mode.
    //     Default = OutputMode::SINGLE_ENDED.
    //
    //     OutputMode::SINGLE_ENDED = Standard 1-pin operation.
    //     OutputMode::DIFFERENTIAL = Differential 2-pin operation. The two outputs are
    //                                driven in opposite states, producing a larger
    //                                voltage across a passive piezoelectric buzzer for
    //                                increased sound output.
    //
    //                                If no valid second GPIO pin is provided, the buzzer
    //                                automatically operates in 1-pin mode.
    Buzzer(uint8_t pin1, uint8_t pin2 = NO_PIN, OutputMode mode = OutputMode::SINGLE_ENDED);

    // Configures the buzzer GPIO pin(s).
    void begin();

    // Changes the operating mode.
    //
    // SINGLE_ENDED = Standard 1-pin operation.
    // DIFFERENTIAL  = Differential 2-pin operation. The two outputs are
    //                 driven in opposite states, producing a larger voltage
    //                 across the piezo for increased sound output.
    //
    // If no valid second pin is available, the buzzer
    // automatically remains in 1-pin mode.
    //
    // Any tone currently playing is stopped.
    void setMode(OutputMode mode);

    // Enables or disables differential mode.
    // If no valid second pin is available, the buzzer
    // automatically remains in 1-pin mode.
    void enableDifferential(bool enable)
    {
        setMode(enable ? OutputMode::DIFFERENTIAL : OutputMode::SINGLE_ENDED);
    }

    // Current operating mode.
    //
    // SINGLE_ENDED = Single GPIO output.
    // DIFFERENTIAL  = Differential 2-pin output.
    bool isDifferential() const { return _mode == OutputMode::DIFFERENTIAL; }

    // Updates tone duration and chirp frequency.
    //
    // Tone generation itself is handled by Timer1. This function still
    // must be called every loop() iteration so the buzzer can stop when
    // the requested duration ends and chirps can sweep frequency.
    void update();

    // Starts playing a fixed-frequency tone.
    //
    // frequency
    //     Tone frequency in Hertz (Hz).
    //
    // duration
    //     Playback duration in milliseconds (ms).
    //
    // Playback is asynchronous. This function returns immediately.
    void playTone(uint16_t frequency, uint32_t duration);

    // Starts playing a chirp.
    //
    // startFreq
    //     Initial frequency in Hertz (Hz).
    //
    // endFreq
    //     Final frequency in Hertz (Hz).
    //
    // duration
    //     Sweep duration in milliseconds (ms).
    //
    // During playback, the frequency changes from startFreq to endFreq.
    void playChirp(uint16_t startFreq, uint16_t endFreq, uint32_t duration);

    // Stops the current playback immediately.
    //
    // Any tone or chirp currently being generated is stopped and all
    // buzzer outputs are driven LOW.
    void stop();

    // Returns true while the buzzer is generating a tone or chirp.
    bool isPlaying() const;

    // Mutes the buzzer and silences any current playback immediately.
    //
    // Playback timing continues while muted. If unmuted before the playback
    // duration expires, the tone or chirp resumes.
    void mute();

    // Unmutes the buzzer.
    void unmute();

    // Returns true if the buzzer is muted.
    bool isMuted() const { return _isMuted; }

private:
    // Handles Timer1 compare-match interrupts.
    friend void _handleTimerInterrupt();

    // Toggles the buzzer output(s).
    //
    // Called by the Timer1 interrupt each time the waveform changes state.
    void _toggleOutput();

    // Updates the frequency of the current chirp based on the elapsed
    // playback time, then generates the corresponding square wave.
    void _updateChirp(uint32_t elapsedTime);

    // Writes the specified state to the buzzer output(s).
    void _writeOutputs(bool state);

    // Starts Timer1 so it interrupts at the requested tone frequency.
    void _startHardwareTimer(uint16_t frequency);

    // Updates Timer1 interrupt frequency without restarting playback.
    void _updateHardwareTimerFrequency(uint16_t frequency);

    // Stops Timer1 interrupts.
    void _stopHardwareTimer();

    // Silences the buzzer by driving all outputs LOW.
    void _silence();

    // GPIO pins connected to the buzzer.
    uint8_t _pin1;
    uint8_t _pin2;

    // Fast GPIO access data used by the Timer1 ISR.
    volatile uint8_t *_port1;
    volatile uint8_t *_port2;
    uint8_t _mask1;
    uint8_t _mask2;

    // Current operating mode.
    //
    // SINGLE_ENDED = Single GPIO output.
    // DIFFERENTIAL  = Differential 2-pin output.
    OutputMode _mode;

    // Indicates whether a tone is currently playing.
    bool _playingTone;

    // Indicates whether a chirp is currently playing.
    bool _playingChirp;

    // Current output frequency in Hertz.
    //
    // For a fixed-frequency tone, this value remains constant.
    // During a chirp, it is updated continuously.
    uint16_t _frequency;

    // Playback duration in milliseconds.
    //
    // Used for both tones and chirps.
    uint32_t _duration;

    // Initial chirp frequency in Hertz.
    uint16_t _startFreq;

    // Final chirp frequency in Hertz.
    uint16_t _endFreq;

    // Time (millis) when the tone started.
    uint32_t _startTime;

    // Current output state.
    //
    // false = LOW
    // true  = HIGH
    bool _outputState;

    // Indicates whether the buzzer is muted (no sound output).
    bool _isMuted;
};