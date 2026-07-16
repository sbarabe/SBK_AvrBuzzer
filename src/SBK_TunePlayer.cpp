//=====================================================================
// TunePlayer - Implementation
//=====================================================================
//
// SPDX-License-Identifier: MIT
//
// Non-blocking melody player for the SBK_AvrBuzzer library.
//
// Copyright (c) 2026 Samuel Barabe
// Licensed under the MIT License.
//=====================================================================

#include "SBK_TunePlayer.h"
#include "SBK_Tune.h"

#include <avr/pgmspace.h>

//=====================================================================
// Constructor
//=====================================================================

TunePlayer::TunePlayer(Buzzer &buzzer)
    : _buzzer(buzzer),
      _activeTune(nullptr),
      _lastPlayedTune(nullptr),
      _noteIndex(0),
      _currentNoteDuration(0),
      _nextNoteTime(0),
      _currentNotePitch(0)
{
}

//=====================================================================
// Public Methods
//=====================================================================

void TunePlayer::play(const Tune &tune)
{
    _buzzer.stop();
    resetPlaybackState();

    _activeTune = &tune;
    _lastPlayedTune = &tune;
    _nextNoteTime = millis();
}

void TunePlayer::playRandom(const Tune *const tunes[], uint8_t tuneCount)
{
    if (tunes == nullptr || tuneCount == 0)
    {
        return;
    }

    const Tune *selectedTune = nullptr;

    do
    {
        selectedTune = tunes[random(tuneCount)];
    }
    while (tuneCount > 1 && selectedTune == _lastPlayedTune);

    if (selectedTune != nullptr)
    {
        play(*selectedTune);
    }
}

void TunePlayer::repeat()
{
    if (_lastPlayedTune != nullptr)
    {
        play(*_lastPlayedTune);
    }
}

void TunePlayer::update()
{
    if (_activeTune == nullptr)
    {
        return;
    }

    const uint32_t now = millis();

    // Rollover-safe time comparison.
    if (static_cast<int32_t>(now - _nextNoteTime) < 0)
    {
        return;
    }

    if (_buzzer.isPlaying())
    {
        return;
    }

    if (_noteIndex >= _activeTune->length)
    {
        _activeTune = nullptr;
        return;
    }

    const uint16_t sequenceOffset =
        static_cast<uint16_t>(_noteIndex) * 2U;

    _currentNotePitch =
        pgm_read_word(&_activeTune->sequence[sequenceOffset]);

    const uint16_t durationUnit =
        pgm_read_word(&_activeTune->sequence[sequenceOffset + 1U]);

    const uint32_t quarterNoteMs =
        60000UL / _activeTune->bpm;

    _currentNoteDuration =
        (quarterNoteMs * durationUnit) / D_4;

    if (_currentNoteDuration == 0)
    {
        _currentNoteDuration = 1;
    }

    if (_currentNotePitch != NOTE_REST)
    {
        _buzzer.playTone(
            _currentNotePitch,
            _currentNoteDuration);
    }

    _nextNoteTime = now + _currentNoteDuration;
    _noteIndex++;
}

void TunePlayer::stop()
{
    _buzzer.stop();
    resetPlaybackState();
}

bool TunePlayer::isPlaying() const
{
    return (_activeTune != nullptr);
}

uint32_t TunePlayer::getCurrentNoteDurationMs() const
{
    if (_activeTune == nullptr)
    {
        return 0;
    }

    return _currentNoteDuration;
}

uint16_t TunePlayer::getCurrentNotePitch() const
{
    return _currentNotePitch;
}

uint8_t TunePlayer::getCurrentNoteIndex() const
{
    if (_activeTune == nullptr || _noteIndex == 0)
    {
        return 0;
    }

    return _noteIndex - 1U;
}

//=====================================================================
// Private Methods
//=====================================================================

void TunePlayer::resetPlaybackState()
{
    _activeTune = nullptr;
    _noteIndex = 0;
    _currentNoteDuration = 0;
    _nextNoteTime = 0;
    _currentNotePitch = 0;
}