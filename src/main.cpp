#include "setup.h"
#include "display.h"
#include "midi_serial.h"

void setup() {
    // Initialize OLED display
    initDisplay();
    initMidi();
}

void loop() {
    // Update BPM every second
    unsigned long currentTime = millis();
    if (currentTime - lastBPMTime >= BPM_INTERVAL_MS) { // Calculate BPM every second
        calculateBPM();
        lastBPMTime = currentTime;
        printDisplay(isMasterInput1() ? "MIDI-1" : "MIDI-2", currentBPM, clockPulseResolution);
    }

    //Change clock resolution if button is pressed
    if (digitalRead(CLOCK_BUTTON_PIN) == LOW) {
        changeClockResolution();
        delay(200); // Debounce delay
    }

    // Read all MIDI Input
    readMidiInput();
}
