//=====================================================================
// Chirp
//
// Demonstrates frequency chirp playback using the SBK_AvrBuzzer library.
//
// The sketch plays an upward chirp once every two seconds while the
// rest of the application remains responsive.
//=====================================================================

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>

// Connect a passive piezo buzzer between pin 9 and GND.
Buzzer buzzer(9);

uint32_t lastChirpTime = 0;

bool sweepUp = true;

void setup()
{
    buzzer.begin();
}

void loop()
{
    // Keep the buzzer driver running.
    buzzer.update();

    // Play a chirp every two seconds.
    if (millis() - lastChirpTime >= 2000)
    {
        // Record the time of the last chirp.
        lastChirpTime = millis();

        // Play upward chirp from 500 Hz to 2000 Hz over 1 second.
        if (sweepUp)
        {
            buzzer.playChirp(500, 2000, 1000);
        }

        // Play downward chirp from 2000 Hz to 500 Hz over 1 second.
        else 
        {
            buzzer.playChirp(2000, 500, 1000);
        }

        // Alternate between upward and downward chirps.
        sweepUp = !sweepUp;
    }

    // Other application code can run here while the chirp plays.
}