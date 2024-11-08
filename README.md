

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

### License
-------

MIT License

Copyright (c) 2024 Pria Jatnika

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.