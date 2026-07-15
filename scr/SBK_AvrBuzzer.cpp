//=====================================================================
// SBK_AvrBuzzer Library - Implementation
//=====================================================================
//
// SPDX-License-Identifier: MIT
//
// Implementation of the Buzzer class.
//
// Copyright (c) 2026 Samuel Barabe
// Licensed under the MIT License.
//=====================================================================

#include "SBK_AvrBuzzer.h"
#include <avr/interrupt.h>

// Pointer to the currently active buzzer instance.
static Buzzer *s_activeBuzzer = nullptr;

//=====================================================================
// Handles Timer1 compare-match interrupts.
// This function is called by the Timer1 ISR to update the buzzer output.
//=====================================================================
void _handleTimerInterrupt()
{
    if (s_activeBuzzer != nullptr)
    {
        s_activeBuzzer->_toggleOutput();
    }
}

//=====================================================================
// Timer1 Compare Match A interrupt service routine.
// This ISR is called each time Timer1 reaches the value in OCR1A.
// It toggles the buzzer output(s) to generate the square wave for the current tone.
//=====================================================================
ISR(TIMER1_COMPA_vect)
{
    _handleTimerInterrupt();
}

//=====================================================================
// Constructor
//=====================================================================
// Initializes the buzzer object and stores its configuration.
//
// If DIFFERENTIAL mode is requested but no valid second GPIO pin is provided,
// the buzzer automatically falls back to standard SINGLE_ENDED operation.
//
// The buzzer is initially stopped and all timing variables are reset.
//=====================================================================
Buzzer::Buzzer(uint8_t pin1, uint8_t pin2, OutputMode mode)
    : _pin1(pin1),
      _pin2(pin2),
      // Enable DIFFERENTIAL mode only if a valid second GPIO pin is available.
      _mode((mode == OutputMode::DIFFERENTIAL && (pin2 != NO_PIN))
                ? OutputMode::DIFFERENTIAL
                : OutputMode::SINGLE_ENDED),
      _playingTone(false),
      _playingChirp(false),
      _frequency(0),
      _duration(0),
      _startFreq(0),
      _endFreq(0),
      _startTime(0),
      _outputState(false),
      _isMuted(false)
{
}

//=====================================================================
// Initializes the buzzer GPIO pin(s).
//
// Both outputs are configured as outputs and driven LOW to ensure the
// buzzer is silent when the program starts.
//=====================================================================
void Buzzer::begin()
{
    pinMode(_pin1, OUTPUT);

    _port1 = portOutputRegister(digitalPinToPort(_pin1));
    _mask1 = digitalPinToBitMask(_pin1);

    if (_mode == OutputMode::DIFFERENTIAL)
    {
        pinMode(_pin2, OUTPUT);

        _port2 = portOutputRegister(digitalPinToPort(_pin2));
        _mask2 = digitalPinToBitMask(_pin2);
    }

    _silence();
}

//=====================================================================
// Changes the buzzer operating mode.
//
// Any tone currently playing is stopped before changing mode to avoid
// leaving the output pins in an inconsistent state.
//=====================================================================
void Buzzer::setMode(OutputMode mode)
{
    stop();
    if (mode == OutputMode::DIFFERENTIAL && _pin2 != NO_PIN)
    {
        _mode = OutputMode::DIFFERENTIAL;
    }
    else
    {
        _mode = OutputMode::SINGLE_ENDED;
    }
}

//=====================================================================
// Updates the buzzer.
//
// This function must be called repeatedly from loop().
//
// It performs the following tasks:
//
//  - Stops playback when the requested duration has elapsed.
//  - Updates the frequency of an active chirp.
//
// Tone generation itself is handled asynchronously by the Timer1
// interrupt.
//=====================================================================
void Buzzer::update()
{
    // Nothing to do if the buzzer is idle.
    if (!isPlaying())
    {
        return;
    }

    // Stop playback once the requested duration has elapsed.
    // Also stop immediately if the buzzer is muted.
    uint32_t elapsed = millis() - _startTime;

    if (elapsed >= _duration)
    {
        stop();
        return;
    }

    if (_playingChirp)
    {
        _updateChirp(elapsed);
    }
}

//=====================================================================
// Toggles the buzzer output.
//
// Called by the Timer1 interrupt each time the square wave changes
// state.
//
// The function toggles the current output state and writes the new
// state to the buzzer output pin(s).
//=====================================================================
void Buzzer::_toggleOutput()
{
    if (_isMuted)
    {
        _silence();
        return;
    }

    _outputState = !_outputState;
    _writeOutputs(_outputState);
}

//=====================================================================
// Updates a chirp.
//
// Calculates the current frequency from the elapsed playback time and
// updates the Timer1 interrupt rate accordingly.
//
// Called periodically from update() while a chirp is playing.
//======================================================================
void Buzzer::_updateChirp(uint32_t elapsedTime)
{

    int32_t freqRange = (int32_t)_endFreq - (int32_t)_startFreq;
    _frequency = _startFreq + ((int32_t)freqRange * elapsedTime) / _duration;

    // If the current frequency is zero, this part of the chirp is silent.
    // Drive both outputs LOW and skip square-wave generation to avoid
    // dividing by zero when calculating the half period.
    if (_frequency == 0)
    {
        _stopHardwareTimer();
        _silence();
        return;
    }

    if (!_isMuted)
    {
        _updateHardwareTimerFrequency(_frequency);
    }
}

//=====================================================================
// Starts Timer1.
//
// Configures Timer1 in CTC mode to generate compare-match interrupts
// at twice the requested tone frequency.
//
// Each interrupt toggles the buzzer output, producing a square wave.
//=====================================================================
void Buzzer::_startHardwareTimer(uint16_t frequency)
{
    // A frequency of 0 Hz represents silence.
    if (frequency == 0)
    {
        _stopHardwareTimer();
        return;
    }

    // Register this buzzer as the current Timer1 owner so the ISR
    // knows which instance to update.
    s_activeBuzzer = this;

    // Prevent Timer1 from generating interrupts while its registers
    // are being reconfigured.
    noInterrupts();

    // Stop Timer1 and reset its configuration.
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // Calculate the compare value required to generate the requested
    // tone frequency.
    //
    // Timer1 runs at:
    //     F_CPU / 8 (divided by the Timer1 prescaler)
    //
    // The buzzer output changes state on every interrupt, so the
    // interrupt frequency must be twice the desired tone frequency.
    uint32_t compareValue = (F_CPU / 8UL) / (frequency * 2UL);

    // OCR1A is zero-based, so subtract one timer count.
    if (compareValue > 0)
    {
        compareValue--;
    }

    // Clamp the compare value to the 16-bit range supported by Timer1.
    if (compareValue > 65535UL)
    {
        compareValue = 65535UL;
    }

    // Load the compare value.
    OCR1A = static_cast<uint16_t>(compareValue);

    // Configure Timer1 for Clear Timer on Compare Match (CTC) mode.
    // The timer resets automatically when OCR1A is reached.
    TCCR1B |= (1 << WGM12);

    // Start Timer1 with a prescaler of 8.
    TCCR1B |= (1 << CS11);

    // Enable the Timer1 Compare Match A interrupt.
    TIMSK1 |= (1 << OCIE1A);

    // Resume interrupt processing.
    interrupts();
}

//=====================================================================
// Updates the Timer1 interrupt frequency.
//
// Used while playing a chirp to change the tone frequency without
// restarting the timer.
//=====================================================================
void Buzzer::_updateHardwareTimerFrequency(uint16_t frequency)
{
    _startHardwareTimer(frequency);
}

//=====================================================================
// Stops Timer1.
//
// Disables Timer1 compare-match interrupts and releases the active
// buzzer instance.
//=====================================================================
void Buzzer::_stopHardwareTimer()
{
    noInterrupts();

    TIMSK1 &= ~(1 << OCIE1A);
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    if (s_activeBuzzer == this)
    {
        s_activeBuzzer = nullptr;
    }

    interrupts();
}

//=====================================================================
// Writes the specified output state to the buzzer.
//
// In DIFFERENTIAL mode, the second output is driven in the opposite
// state, producing a larger voltage across the passive piezoelectric
// buzzer.
//=====================================================================
void Buzzer::_writeOutputs(bool state)
{
    if (_isMuted)
    {
        _silence();
        return;
    }

    if (state)
    {
        *_port1 |= _mask1;

        if (_mode == OutputMode::DIFFERENTIAL)
        {
            *_port2 &= ~_mask2;
        }
    }
    else
    {
        *_port1 &= ~_mask1;

        if (_mode == OutputMode::DIFFERENTIAL)
        {
            *_port2 |= _mask2;
        }
    }
}

//=====================================================================
// Silences the buzzer by driving all outputs LOW.
//=====================================================================
void Buzzer::_silence()
{
    *_port1 &= ~_mask1;

    if (_mode == OutputMode::DIFFERENTIAL)
    {
        *_port2 &= ~_mask2;
    }
}

//=====================================================================
// Starts playing a fixed-frequency tone.
//
// frequency
//     Tone frequency in Hertz.
//
// duration
//     Playback duration in milliseconds.
//
// Playback is asynchronous.
//
// Timer1 generates the waveform while update() manages playback
// duration.
//=====================================================================
void Buzzer::playTone(uint16_t frequency, uint32_t duration)
{
    // Ignore invalid parameters.
    if (frequency == 0 || duration == 0)
    {
        stop();
        return;
    }

    _frequency = frequency;
    _duration = duration;

    // Record the playback start time.
    _startTime = millis();

    // Start the waveform immediately.
    _outputState = false;

    _playingTone = true;
    _playingChirp = false;

    if (!_isMuted)
    {
        _startHardwareTimer(_frequency);
    }
}

//=====================================================================
// Starts playing a chirp.
//
// startFreq
//     Initial frequency in Hertz.
//
// endFreq
//     Final frequency in Hertz.
//
// duration
//     Sweep duration in milliseconds.
//
// Playback is asynchronous.
//
// Timer1 generates the waveform while update() continuously adjusts
// the interrupt frequency to produce the frequency sweep.
//=====================================================================
void Buzzer::playChirp(uint16_t startFreq, uint16_t endFreq, uint32_t duration)
{
    // Ignore invalid parameters.
    if (duration == 0 || (startFreq == 0 && endFreq == 0))
    {
        stop();
        return;
    }

    _startFreq = startFreq;
    _endFreq = endFreq;
    _duration = duration;

    // Record the chirp playback start time.
    _startTime = millis();

    _frequency = _startFreq;
    _outputState = false;

    _playingChirp = true;
    _playingTone = false;

    // If the chirp starts at 0 Hz, begin silently and let _updateChirp()
    // drive the first audible transition once the interpolated frequency
    // becomes non-zero.
    if (_frequency == 0)
    {
        _stopHardwareTimer();
        _silence();
    }
    else if (!_isMuted)
    {
        _startHardwareTimer(_frequency);
    }
}

//=====================================================================
// Stops playback.
//
// Stops Timer1 and silences the buzzer outputs.
//=====================================================================
void Buzzer::stop()
{
    _stopHardwareTimer();
    _silence();
    _playingTone = false;
    _playingChirp = false;
}

//=====================================================================
// Returns true while the buzzer is active.
//
// Returns true if either a fixed-frequency tone or a chirp is currently
// being generated.
//=====================================================================
bool Buzzer::isPlaying() const
{
    return _playingTone || _playingChirp;
}

//=====================================================================
// Mutes the buzzer and stops any current playback immediately.
//=====================================================================
void Buzzer::mute()
{
    _isMuted = true;
    _stopHardwareTimer();
    _silence();
}

//=====================================================================
// Unmutes the buzzer.
//=====================================================================
void Buzzer::unmute()
{
    _isMuted = false;

    if (isPlaying() && _frequency > 0)
    {
        _startHardwareTimer(_frequency);
    }
}
