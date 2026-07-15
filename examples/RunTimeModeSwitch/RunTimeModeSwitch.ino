//=====================================================================
// RuntimeModeSwitch
//
// Demonstrates changing the buzzer output mode while the application
// is running.
//
// A passive piezo buzzer must be connected between pins 9 and 10.
// The sketch alternates between SINGLE_ENDED and DIFFERENTIAL output,
// allowing the difference in sound level to be heard.
//=====================================================================

// In SINGLE_ENDED mode, pin 9 drives the buzzer while pin 10 is held LOW.
//
// In DIFFERENTIAL mode, both pins drive the buzzer with opposite
// polarities, increasing the sound output.

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>

// Connect a passive piezo buzzer between pins 9 and 10.

Buzzer buzzer(9, 10);

uint32_t lastToneTime = 0;
bool differentialMode = false;
uint8_t toneCount = 0;

void setup()
{
    buzzer.begin();
}

void loop()
{
    // Keep the buzzer driver running.
    buzzer.update();

    // Play a tone every second for 3 tones then switch mode.
    if (millis() - lastToneTime >= 1000)
    {
        // Record the time of the last tone.
        lastToneTime = millis();

        // Play a 1 kHz tone for 500 ms.
        buzzer.playTone(1000, 500);

        // Count the number of tones played in the current mode.
        toneCount++;

        // After 3 tones, switch between SINGLE_ENDED and DIFFERENTIAL output.
        if (toneCount >= 3)
        {
            toneCount = 0;

            // Toggle the output mode.
            differentialMode = !differentialMode;
            buzzer.enableDifferential(differentialMode);
        }
    }

    // Other application code can run here while the tone plays.
}

// For this example, wire the passive piezo buzzer as follows:
//
//    Pin 9 ----+
//              |
//          Passive
//           Piezo
//              |
//   Pin 10 ----+
//
// Do NOT connect either buzzer terminal to GND.
//
// In SINGLE_ENDED mode, the library automatically drives Pin 10 LOW,
// making it behave like a conventional single-ended connection.
//
// In DIFFERENTIAL mode, Pins 9 and 10 are driven with opposite
// polarities to increase the sound output.
