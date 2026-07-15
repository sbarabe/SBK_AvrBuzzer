//=====================================================================
// BasicTone
//
// Demonstrates the basic use of the SBK_AvrBuzzer library.
//
// The sketch plays a 1 kHz tone every second while the rest of the
// application remains responsive.
//=====================================================================

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>

// Connect a passive piezo buzzer between pin 9 and GND.
Buzzer buzzer(9);

uint32_t lastToneTime = 0;

void setup()
{
    buzzer.begin();
}

void loop()
{
    // Keep the buzzer driver running.
    buzzer.update();

    // Play a tone once every second.
    if (millis() - lastToneTime >= 1000)
    {
        lastToneTime = millis();

        // Play a 1 kHz tone for 250 ms.
        buzzer.playTone(1000, 250);
    }

    // Other application code can run here while the tone plays.
}