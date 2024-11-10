If you want to **multiply the MIDI output to 4 outputs**, you can use an **Active MIDI Thru circuit**. Below, I'll walk you through how you can design and implement a circuit that takes one MIDI output signal and splits it into **four independent outputs**, each capable of driving a separate MIDI device.

### **Components Required**:
- **1x 6N138 Optocoupler** or compatible opto-isolator
- **1x 74HC14 Hex Schmitt Trigger Inverter** IC
- **4x 220Ω Resistors** (current limiting for MIDI outputs)
- **4x MIDI DIN 5-pin Connectors** (for MIDI output)
- **1x 1kΩ Resistor** (current limiting for optocoupler input)
- **1x 10kΩ Resistor** (pull-up for optocoupler output)
- **1x 5V Power Supply** (can be sourced from the microcontroller or an external supply)

### **Circuit Design Overview**:

1. **MIDI Input Section**:
   - The signal from the `MIDIOUT` pin of your microcontroller is connected to the **6N138 optocoupler**.
   - The **anode** (pin 2) of the optocoupler is connected to **+5V** through a **1kΩ resistor**.
   - The **cathode** (pin 3) is connected to the `MIDIOUT` line and **GND**.
   - The output of the **6N138** is taken from **pin 6** and connected to the **input of the 74HC14** (through a **10kΩ pull-up resistor** to 5V).

2. **Buffer Stage (74HC14 IC)**:
   - The **74HC14** is a hex inverter with Schmitt Trigger inputs, which ensures clean signal transitions.
   - The **output** from the **6N138 optocoupler** (pin 6) is connected to the **input of one inverter** on the **74HC14** (e.g., **pin 1**).
   - The **output of that inverter** (e.g., **pin 2**) is then used as the main MIDI output signal.
   - Use the remaining **four inverters** on the **74HC14** to create **four identical MIDI output signals**.

3. **Output Stage**:
   - Each output from the **74HC14** is connected to a **MIDI DIN connector** through a **220Ω resistor**.
   - The resistor limits the current to comply with MIDI standards.
   - Connect **pin 5** of each **MIDI DIN connector** to **+5V** through another **220Ω resistor**.
   - **Pin 2** of each **MIDI DIN connector** should be connected to **GND**.

### **Detailed Wiring**:
1. **Input Section with Optocoupler**:
   - **Pin 2 (Anode) of 6N138**: Connected to **+5V** via a **1kΩ resistor**.
   - **Pin 3 (Cathode) of 6N138**: Connected to `MIDIOUT` line from your microcontroller.
   - **Pin 5** (Collector output) is grounded.
   - **Pin 6 (Emitter output)**: Connected to the input of the **74HC14** and a **10kΩ pull-up resistor** to **+5V**.

2. **Buffering with 74HC14 Hex Schmitt Trigger**:
   - **Pin 1 (Input)**: Connected to the output of the **6N138**.
   - **Pin 2 (Output)**: Buffered **MIDI signal**.
   - The output of **pin 2** is connected to the **inputs** of four other inverters (**pins 3, 5, 9, 11**) on the **74HC14**.
   - Each output from these inverters (**pins 4, 6, 8, 10**) is then used to drive separate MIDI outputs.

3. **MIDI Output Stage**:
   - Each **output pin** from the **74HC14** goes through a **220Ω resistor** and is connected to **pin 4** of the **MIDI DIN connector**.
   - **Pin 5** of each **MIDI DIN connector** should also have a **220Ω resistor** connected to **+5V**.
   - **Pin 2** of each **MIDI DIN connector** is connected to **GND**.

### **MIDI Connector Pinout**:
- **Pin 2**: **GND**
- **Pin 4**: **MIDI Signal Out** (comes from **74HC14** via a **220Ω resistor**)
- **Pin 5**: Connected to **+5V** through a **220Ω resistor**.

### **Explanation**:
- The **6N138 optocoupler** is used to isolate the original MIDI signal.
- The **74HC14** is used to buffer and split the MIDI signal into multiple outputs.
- Each output has its own **220Ω resistor** to comply with the MIDI electrical specifications.
- The result is **four identical MIDI outputs** capable of driving multiple devices while preserving signal integrity.

### **Diagram Summary**:
1. **Input Stage**:
   - `MIDIOUT` -> 6N138 -> **74HC14 Pin 1** (Buffered).
2. **Output Stage** (using 74HC14):
   - **Pin 2** of the 74HC14 drives **MIDI OUT1**.
   - **Pins 4, 6, 8, 10** drive **MIDI OUT2, OUT3, OUT4** respectively.

This design gives you **four identical MIDI outputs** that you can connect to different devices, ensuring that all receive the same MIDI data without compromising signal quality. This approach is widely used in **MIDI Thru** boxes.