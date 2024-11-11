

# rusaaKKMODS Megamidi Merge
================

A MIDI merge project for Arduino.

## Overview
-----------

This project is designed to merge MIDI input from multiple sources, calculate beats per minute (BPM), and update clock resolution as needed.

### Features
------------

* Merges MIDI input from two sources (MIDI-1 and MIDI-2)
* Calculates BPM from MIDI clock messages
* Updates clock resolution based on user input
* Displays current BPM, clock resolution, and master clock source on an OLED display

### Functions
-------------

* `initMidi()`: Initializes MIDI input and output interfaces.
* `calculateBPM()`: Calculates the current BPM from MIDI clock messages.
* `changeClockResolution()`: Updates the clock resolution based on user input.
* `readMidiInput()`: Reads and processes MIDI input from both sources.
* `printDisplay()`: Prints the current display information, including master clock source, BPM, and clock resolution.
* `isMasterInput1()`: Checks if the master clock source is set to MIDI-1.
* `loop()`: The main loop function that updates BPM, checks for user input, and reads MIDI input.

### Dependencies
------------

* Arduino library (for `millis()` and `digitalRead()` functions)
* MIDI library (for MIDI input and output handling)

### Hardware Requirements
----------------------

* Arduino board (e.g. Arduino Mega)
* MIDI input and output interfaces
* OLED display
* Push button for changing clock resolution

### Usage
-----

1. Upload the sketch to the Arduino board.
2. Connect the MIDI input and output interfaces to the board.
3. Connect the OLED display to the board.
4. Press the push button to change the clock resolution.
