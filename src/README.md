Based on the code we currently have, let's review the main goals and assess the progress:

### Main Goals:

1. **Core Function: Merges MIDI data from two MIDI inputs and sends it to MIDI output**

   - **Status**: Achieved. The code reads MIDI messages from `MIDIINPUT1` and `MIDIINPUT2` and merges them into a single output (`MIDIOUTPUT`). MIDI messages are forwarded appropriately to combine data from both inputs.

2. **Master Clock Selection: An SPDT switch allows you to choose which MIDI input provides the master clock**

   - **Status**: Achieved. The function `isMasterInput1()` reads the state of the master switch (`MASTER_SWITCH_PIN`) to determine which MIDI input is the master clock source. This condition is used throughout the code to determine the active master.

3. **Analog Clock Output: Generates an analog clock signal synchronized with the MIDI clock**

   - **Status**: Achieved. The function `generateDigitalClockPulse()` is responsible for outputting a pulse on the clock output pin (`CLOCK_OUT_PIN`). However, software-based timing could be less precise, and switching to hardware-based timers for pulse generation is recommended.

4. **Clock Pulse Rate: A knob to control the clock pulse per quarter note (1 to 24 ppqn) for the analog clock output**

   - **Status**: Achieved. The knob input (`RESOLUTION_KNOB_PIN`) is read and mapped to different pulse-per-quarter-note values using the `handleClockOut()` function. The mapped values are snapped to specific options, which align with the requirements.

5. **Active Master Input Display: Shows the active MIDI Input acting as the Master Clock on an OLED display**

   - **Status**: Achieved. The function `printDisplay()` updates the OLED display with information about which input is the active master, depending on the `isMasterInput1()` function result. It also displays BPM and clock rate settings.

6. **BPM Display: Displays the Beats Per Minute (BPM) in 4/4 time signature based on selected master clock**

   - **Status**: Achieved. The function `calculateBPM()` calculates the BPM based on the `clockCount`, and `handleDisplay()` updates the OLED display every second with the calculated BPM.

7. **Clock Rate Display: based on the selected resolution setting**

   - **Status**: Achieved. The function `printDisplay()` shows the clock rate and provides an indicator for the current pulse-per-quarter-note value. The `resolution` variable is used for determining and displaying the current setting.

8. **Analog Clock Input: Override MIDI master clock**

   - **Status**: Achieved. The `handleClockIn()` function manages clock input detection. When a clock pulse is detected on `CLOCK_IN_PIN`, it sets `clockInputPulse` and sends a MIDI clock message (`sendRealTimeMessage(midi::Clock)`), effectively overriding the MIDI master clock.

### &#x20;Features:

1. **2 MIDI Inputs (as Input 1 and Input 2)**

   - **Status**: Achieved. The code uses `Serial1` and `Serial2` as MIDI inputs, which are then processed and forwarded as needed.

2. **2 MIDI Thru (from MIDI Input 1 and MIDI Input 2)**

   - **Status**: Implicitly Achieved. The messages from `MIDIINPUT1` and `MIDIINPUT2` are forwarded, effectively acting as MIDI Thru functionality.

3. **A MIDI Output (Merged from both MIDI Inputs)**

   - **Status**: Achieved. The merged MIDI messages are sent to `MIDIOUTPUT`.

4. **A 3.5mm TRS Socket as a second MIDI Output (independent copy of the first MIDI Output)**

   - **Status**: Not explicitly mentioned in the code. If this feature is still desired, it may need additional implementation, likely involving copying the `MIDIOUTPUT` to another UART or pin.

5. **Analog Clock Out based on Master clock**

   - **Status**: Achieved. The clock output is generated using software-based timing (`generateDigitalClockPulse()`), but improvements to use hardware timers are recommended for better precision.

6. **Analog Clock In that overrides the MIDI Clock when plugged**

   - **Status**: Achieved. The `handleClockIn()` function detects clock input pulses and overrides the MIDI clock when active.

7. **A Master Clock switch between Input 1 or Input 2**

   - **Status**: Achieved. The function `isMasterInput1()` reads the `MASTER_SWITCH_PIN` to determine which input is the master clock.

8. **A Knob to control Clock Out PPQN**

   - **Status**: Achieved. The `RESOLUTION_KNOB_PIN` is used to set the clock rate (`resolution`), which affects the pulse-per-quarter-note value.

9. **USB should power the system and should be recognized by Computer as MIDI Device transmitting merged MIDI Output**

   - **Status**: Not explicitly Achieved. The code does not mention USB MIDI functionality. To achieve this, additional USB-MIDI code would need to be implemented to allow the Arduino to act as a USB MIDI device.

10. **An OLED Display to view Active Master information**

    - **Status**: Achieved. The `printDisplay()` function displays the current master clock source, BPM, and clock rate settings on the OLED.

### Summary of Improvements:

- **Precision for Clock Output**: Consider using hardware timers for generating clock pulses (`generateDigitalClockPulse()`) to improve accuracy.
- **USB MIDI Functionality**: If USB MIDI is required, additional implementation is necessary.
- **Refactor Complex Functions**: Functions like `forwardMessage()` are handling many different cases, and breaking them into smaller functions would improve readability and maintainability.
- **Analog Read Noise Filtering**: Adding noise filtering or averaging to `analogRead(RESOLUTION_KNOB_PIN)` can help stabilize the value and avoid erratic changes in clock rate.

Overall, the core functionality has been achieved, with some areas for improvement in terms of precision, maintainability, and additional desired features like USB MIDI support.

