//=====================================================================
// 06_TunePlayer
//
// Demonstrates non-blocking melody playback with TunePlayer.
//
// Connect a passive piezo buzzer between pin 9 and GND.
//
// The sketch:
//   1. Plays a specific example tune.
//   2. Plays a randomly selected tune.
//   3. Repeats the last selected tune.
//
// Both Buzzer::update() and TunePlayer::update() must be called
// repeatedly from loop().
//=====================================================================

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>
#include <SBK_TunePlayer.h>

#include "SBK_ExampleTunes.h"

// Connect a passive piezo buzzer between pin 9 and GND.
constexpr uint8_t BUZZER_PIN = 9;

Buzzer buzzer(BUZZER_PIN);
TunePlayer tunePlayer(buzzer);

// Delay between melody demonstrations.
constexpr uint32_t PAUSE_BETWEEN_TUNES_MS = 1000;

enum class DemoStep : uint8_t
{
    PLAY_SPECIFIC,
    PLAY_RANDOM,
    REPEAT_LAST
};

DemoStep demoStep = DemoStep::PLAY_SPECIFIC;
uint32_t nextTuneTime = 0;
bool waitingForTuneToFinish = false;

//=====================================================================
// Setup
//=====================================================================

void setup()
{
    buzzer.begin();

    // Optional: seed random selection from an unconnected analog pin.
    randomSeed(analogRead(A0));
}

//=====================================================================
// Loop
//=====================================================================

void loop()
{
    // Keep the buzzer and tune player running.
    buzzer.update();
    tunePlayer.update();

    const uint32_t now = millis();

    // Wait until the active tune has finished.
    if (waitingForTuneToFinish)
    {
        if (tunePlayer.isPlaying())
        {
            return;
        }

        waitingForTuneToFinish = false;
        nextTuneTime = now + PAUSE_BETWEEN_TUNES_MS;
    }

    // Wait before starting the next demonstration.
    if (static_cast<int32_t>(now - nextTuneTime) < 0)
    {
        return;
    }

    switch (demoStep)
    {
    case DemoStep::PLAY_SPECIFIC:
        // Play one known tune directly.
        tunePlayer.play(TUNE_MEMOBOT_VICTORY);
        demoStep = DemoStep::PLAY_RANDOM;
        break;

    case DemoStep::PLAY_RANDOM:
        // Select a tune from the example tune table.
        tunePlayer.playRandom(
            MEMOBOT_TUNES,
            MEMOBOT_TUNE_COUNT);

        demoStep = DemoStep::REPEAT_LAST;
        break;

    case DemoStep::REPEAT_LAST:
        // Replay the tune selected during the previous step.
        tunePlayer.repeat();
        demoStep = DemoStep::PLAY_SPECIFIC;
        break;
    }

    waitingForTuneToFinish = true;
}