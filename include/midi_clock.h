#include <Arduino.h>

// Pin Definitions
#define CLOCK_OUT1_PIN 8  // Analog Clock Output 1
#define CLOCK_OUT2_PIN 9  // Analog Clock Output 2
#define CLOCK_BUTTON_PIN 10  // Button to change clock pulse resolution

// Variables for BPM
const int BPM_INTERVAL_MS = 1000;         // Interval in milliseconds for BPM calculation
volatile unsigned long clockCount = 0;   // Count of MIDI Clock messages
unsigned long lastBPMTime = 0;            // Last time BPM was calculated
float currentBPM = 0;                             // Calculated BPM
const int numerator = 4;                   // Fixed numerator for 4/4
const int denominator = 4;                 // Fixed denominator for 4/4 (quarter note)

// Variables for Clock Output
volatile int clockPulseCount = 0;          // Count of MIDI Clock pulses for analog clock
int clockPulseResolution = 4;                   // Clock pulse resolution (1, 2, 4, 8, 16, 32 pulses per beat)

/**
 * @brief Calculate the Beats Per Minute (BPM) based on MIDI clock messages.
 *
 * This function calculates the BPM using the number of MIDI clock messages received.
 * It uses a fixed time signature (4/4) to adjust the BPM calculation.
 * If the numerator is zero, the BPM is set to zero to avoid division by zero.
 * After calculation, the clock message count is reset.
 */
void calculateBPM() {
  // Calculate BPM based on the number of clock messages received
  if (numerator > 0) {
    currentBPM = (clockCount / 24.0) * 60.0 * (4.0 / numerator); // Adjust BPM based on fixed numerator
  } else {
    currentBPM = 0; // Prevent division by zero
  }
  clockCount = 0; // Reset the clock count after calculation
}

/**
 * @brief Cycle the clock pulse resolution on the press of the resolution button.
 *
 * This function doubles the current clock pulse resolution and cycles back to 1
 * if the maximum resolution (32) is reached. The resolutions are 1, 2, 4, 8, 16,
 * and 32 pulses per beat. The purpose of this function is to allow the user to
 * change the clock pulse resolution using a single button.
 */
void changeClockResolution() {
    clockPulseResolution *= 2;
    if (clockPulseResolution > 32) {
        clockPulseResolution = 1; // Cycle back to 1 after reaching 8
    }
}

// Function to handle clock pulse generation for analog clock output
void generateClockPulse() {
    clockPulseCount++;
    if (clockPulseCount >= (24 / clockPulseResolution)) { // Generate a pulse based on the selected resolution
        clockPulseCount = 0;
        // Toggle Clock Output Pins
        digitalWrite(CLOCK_OUT1_PIN, HIGH);
        digitalWrite(CLOCK_OUT2_PIN, HIGH);
        delay(5);  // Short pulse width
        digitalWrite(CLOCK_OUT1_PIN, LOW);
        digitalWrite(CLOCK_OUT2_PIN, LOW);
    }
}