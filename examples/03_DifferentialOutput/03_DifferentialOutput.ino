//=====================================================================
// DifferentialOutput
//
// Demonstrates differential (2-pin) output using the SBK_AvrBuzzer
// library.
//
// In differential mode, the buzzer is connected between two GPIO pins.
// The library drives both pins with opposite polarities, producing a
// larger voltage across a passive piezo buzzer for increased sound
// output.
//=====================================================================

#include <Arduino.h>
#include <SBK_AvrBuzzer.h>

// Connect a passive piezo buzzer between pins 9 and 10.
Buzzer buzzer(9, 10, OutputMode::DIFFERENTIAL);

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

// Wiring:
//
//    Pin 9 ----+
//              |
//          Passive
//           Piezo
//              |
//   Pin 10 ----+
//
// Do NOT connect either buzzer terminal to GND when using
// differential mode.