#pragma once

#include <Arduino.h>
#include "SBK_AvrBuzzer.h"
#include "SBK_Tune.h"

// Forward declaration.
// The Tune structure will later be moved into its own Tune.h file.
struct Tune;

//=====================================================================
// TunePlayer
//=====================================================================
//
// SPDX-License-Identifier: MIT
//
// Non-blocking melody player for the SBK_AvrBuzzer library.
//
// TunePlayer schedules sequences of notes and rests while a Buzzer
// instance handles the hardware-timed sound generation.
//
// The application must call update() regularly.
//
// Copyright (c) 2026 Samuel Barabe
// Licensed under the MIT License.
//=====================================================================

class TunePlayer
{
public:
    explicit TunePlayer(Buzzer &buzzer);

    void play(const Tune &tune);
    void playRandom(const Tune *const tunes[], uint8_t tuneCount);
    void repeat();
    void update();
    void stop();

    bool isPlaying() const;

    uint32_t getCurrentNoteDurationMs() const;
    uint16_t getCurrentNotePitch() const;
    uint8_t getCurrentNoteIndex() const;

private:
    Buzzer &_buzzer;

    const Tune *_activeTune;
    const Tune *_lastPlayedTune;

    uint8_t _noteIndex;
    uint32_t _currentNoteDuration;
    uint32_t _nextNoteTime;
    uint16_t _currentNotePitch;

    void resetPlaybackState();
};