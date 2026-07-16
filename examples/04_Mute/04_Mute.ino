//=====================================================================
// Mute
//
// Demonstrates muting and unmuting the buzzer.
//
// The sketch plays a tone every second. After three tones, the buzzer
// is muted for three playback intervals, then unmuted again.
//=====================================================================

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>

// Connect a passive piezo buzzer between pin 9 and GND.
Buzzer buzzer(9);

uint32_t lastToneTime = 0;
uint8_t toneCount = 0;

void setup()
{
    buzzer.begin();
}

void loop()
{
    // Keep the buzzer driver running.
    buzzer.update();

    // Play a tone every second.
    if (millis() - lastToneTime >= 1000)
    {
        lastToneTime = millis();

        buzzer.playTone(1000, 500);

        toneCount++;

        // Toggle mute every three tones.
        if (toneCount >= 3)
        {
            toneCount = 0;

            if (buzzer.isMuted())
            {
                buzzer.unmute();
            }
            else
            {
                buzzer.mute();
            }
        }
    }

    // Other application code can run here while the tone plays.
}