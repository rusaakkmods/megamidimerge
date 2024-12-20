#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>  // Include the MIDI library

#define VERSION "v0.0.1"

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_I2C_ADDRESS 0x3C
//OLED PIN IN MEGA ARE:
//  SDA PIN 20
//  SCL PIN 21

#define MASTER_SWITCH_PIN 7
#define CLOCK_OUT_PIN 8  // Clock Output using Digital Pin
#define RESOLUTION_KNOB_PIN A0   //PIN 54       // Analog input pin for Clock Pulse Rate knob
#define CLOCK_IN_PIN 2                   // Clock Input using Digital Pin
#define CLOCK_IN_PLUG_DETECT_PIN A1  //PIN 55     // Pin to detect if Clock In is plugged in

// Bonus: Gameboy sync
#define GB_CLOCK_PIN PIN_PA0    // pin 22 PORTA0
#define GB_SI_PIN PIN_PA1       // pin 23 PORTA1 // to Gameboy Serial Out
#define GB_SO_PIN PIN_PA2       // pin 24 PORTA2 // to Gameboy Serial In


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MIDI instance for using Serial1, Serial2, and Serial3 as MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIINPUT1); // Use Hardware UART1 - OUT TX PIN 18, IN RX PIN 19
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDIINPUT2); // Use Hardware UART2 - OUT TX PIN 16, IN RX PIN 17
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDIOUTPUT); // Use Hardware UART3 - OUT TX PIN 14, NC RX PIN 15 (NC = Not Connected to Ground)

// Variables for BPM
unsigned long clockCount = 0;   // Count of MIDI Clock messages
unsigned long lastBPMTime = 0;            // Last time BPM was calculated
unsigned int bpm = 0;                     // Calculated BPM as an integer
const int numerator = 4;                   // Fixed numerator for 4/4 (quarter note)
int clockPulseCount = 0;          // Count of MIDI Clock pulses for analog clock
const int ppqn = 24;                    // Pulses Per Quarter Note
unsigned int resolution = 6;                     // Clock Resolution
unsigned long lastPulseTime = 0;            // Last time Pulse was calculated (HIGH)
volatile bool clockInputPulse = false; // Modified within ISR, so 'volatile' is necessary       // Flag for detecting clock input pulses
const int clockPulseInterval = 5;

bool isMasterInput1() {
    return digitalRead(MASTER_SWITCH_PIN) == HIGH;
}

bool isClockInPlugged() {
    return analogRead(CLOCK_IN_PLUG_DETECT_PIN) < 100;  // Plug is inserted threshbold input less than 100
}

void printDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("MEGAMIDI MERGE");
    display.setCursor(0, 10);
    display.print(isClockInPlugged() ? "Clock In BPM: " : (isMasterInput1() ? "MIDI [1] BPM: " : "MIDI [2] BPM: "));
    display.print(bpm);
    display.setCursor(0, 20);
    display.print("Clock Rate: ");
    display.print(resolution);
    String measure;
    switch (resolution) {
        case 24: measure = " [1/4]"; break;
        case 16: measure = " [1/4T]"; break;
        case 12: measure = " [1/8]"; break;
        case 8: measure = " [1/8T]"; break;
        case 6: measure = " [1/16]"; break;
        case 4: measure = " [1/16T]"; break;
        case 3: measure = " [1/32]"; break;
        case 2: measure = " [1/32T]"; break;
        case 1: measure = " [1/64]"; break;
        default: measure = " N/A"; break;
    }
    display.print(measure);
    display.display();
}

void calculateBPM() {
  // Calculate BPM based on the number of clock messages received
  bpm = (clockCount * 60 * 4) / (ppqn * numerator); // Adjust BPM based on fixed numerator using integer arithmetic
  clockCount = 0; // Reset the clock count after calculation
}

// Update BPM every second
void handleDisplay() {
  unsigned long currentTime = millis();
  if (currentTime - lastBPMTime >= 1000) { // Calculate BPM every second
      calculateBPM();
      lastBPMTime = currentTime;
      printDisplay();
  }
}

// Handle MIDI Clock and timing-related messages with precise forwarding
void sendRealTimeMessage(byte messageType) {
  switch (messageType) {
        case midi::Clock:
            clockCount++; // BPM Counting clock
            MIDIOUTPUT.sendRealTime(midi::Clock);
            generateDigitalClockPulse(); // Generate digital clock pulse
            sendClockPulseToLSDJ(); // send to GameBoy slave sync mode: MIDI
            break;
        case midi::Start:
            MIDIOUTPUT.sendRealTime(midi::Start);
            break;
        case midi::Continue:
            MIDIOUTPUT.sendRealTime(midi::Continue);
            break;
        case midi::Stop:
            MIDIOUTPUT.sendRealTime(midi::Stop);
            break;
        case midi::ActiveSensing:
            MIDIOUTPUT.sendRealTime(midi::ActiveSensing);
            break;
        case midi::SystemReset:
            MIDIOUTPUT.sendRealTime(midi::SystemReset);
            break;
        default:
            // Handle other message types if necessary
            break;
  }
}

// Function to forward a MIDI message to a given MIDI output
template <typename T>
void forwardMessage(midi::MidiInterface<T> &midiInput, const midi::Message<128>& message) {
    byte messageType = message.type;

    // Filter out clock and timing-related messages if not from the master
    if (messageType == midi::Clock || messageType == midi::Start || messageType == midi::Continue || messageType == midi::Stop || messageType == midi::ActiveSensing) {
        if (!isClockInPlugged()) {
          bool isMasterInput1Active = isMasterInput1();
          bool isMidiInput1 = (&midiInput == &MIDIINPUT1);
          bool isMidiInput2 = (&midiInput == &MIDIINPUT2);
          if ((isMasterInput1Active && isMidiInput1) || (!isMasterInput1Active && isMidiInput2)) {
            sendRealTimeMessage(messageType);
          }
        }
        return; // Do not forward these messages
    }

    byte channel = message.channel;
    byte data1 = message.data1;
    byte data2 = message.data2;
    
    switch (messageType) {
        case midi::NoteOn:
            MIDIOUTPUT.sendNoteOn(data1, data2, channel);
            break;
        case midi::NoteOff:
            MIDIOUTPUT.sendNoteOff(data1, data2, channel);
            break;
        case midi::ProgramChange:
            MIDIOUTPUT.sendProgramChange(data1, channel);
            break;
        case midi::ControlChange:
            MIDIOUTPUT.sendControlChange(data1, data2, channel);
            break;
        case midi::PitchBend:
            MIDIOUTPUT.sendPitchBend((data2 << 7) | data1, channel);
            break;
        case midi::AfterTouchPoly:
            MIDIOUTPUT.sendAfterTouch(data1, data2, channel);
            break;
        case midi::AfterTouchChannel:
            MIDIOUTPUT.sendAfterTouch(data1, channel);
            break;
        case midi::TimeCodeQuarterFrame:
            MIDIOUTPUT.sendTimeCodeQuarterFrame(data1);
            break;
        case midi::SongPosition:
            MIDIOUTPUT.sendSongPosition((data2 << 7) | data1);
            break;
        case midi::SongSelect:
            MIDIOUTPUT.sendSongSelect(data1);
            break;
        case midi::TuneRequest:
            MIDIOUTPUT.sendTuneRequest();
            break;
        default:
            // Handle other message types if necessary
            break;
    }
}

// Generate digital clock pulse on additional digital pin
void generateDigitalClockPulse() {
    clockPulseCount++;
    unsigned long currentTime = millis();
    if (clockPulseCount >= resolution) { // Generate a pulse based on resolution
        clockPulseCount = 0;
        digitalWrite(CLOCK_OUT_PIN, HIGH);
        lastPulseTime = currentTime;
    }
}

// Generate digital clock pulse to gameboy
// Using macro to PORTS
// based Arduinoboy https://github.com/trash80/Arduinoboy/blob/master/Arduinoboy/Mode_LSDJ_SlaveSync.ino
// Thanks to @trash80
void sendClockPulseToLSDJ() {
  for(int ticks = 0; ticks < 8; ticks++) {
    digitalWrite(GB_CLOCK_PIN, LOW);
    digitalWrite(GB_CLOCK_PIN, HIGH);
  }
}

void handleClockOut() {
  unsigned long currentTime = millis();
  if (currentTime - lastPulseTime >= clockPulseInterval) { //5 millisecond non delay pulse out
    digitalWrite(CLOCK_OUT_PIN, LOW);
  }

  // Read RESOLUTION knob value and map it to predefined RESOLUTION values
  int knobValue = analogRead(RESOLUTION_KNOB_PIN);
  const int snappedValues[] = {24, 16, 12, 8, 6, 4, 3, 2, 1, 0};
  int index = map(knobValue, 0, 1023, 0, 9); // Map knob value to index 9, ensures that when knobValue is close to the maximum (1023), the index can reach 8.
  index = constrain(index, 0, 8);   // Ensure index stays within the range 0-8
  resolution = snappedValues[index];
}

// Clock Input Interrupt Service Routine (ISR)
void clockInISR() {
    clockInputPulse = true; // Set clock input flag when a pulse is detected
}

void handleClockIn() {
  if (isClockInPlugged() && clockInputPulse) {
    sendRealTimeMessage(midi::Clock); //override MIDI Clock
    clockInputPulse = false;
  }
}

// Generic MIDI Message handler to forward all MIDI1 messages to MIDI2 and MIDI3
void forwardMessageInput1(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT1, message);
}

// Generic MIDI Message handler to forward all MIDI2 messages to MIDI1 and MIDI3
void forwardMessageInput2(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT2, message);
}

void setup() {
    pinMode(MASTER_SWITCH_PIN, INPUT_PULLUP); // Set CSW_PIN as input for the SPDT master clock switch (non-serial, non-I2C)
    pinMode(CLOCK_OUT_PIN, OUTPUT); // Set CLOCK_OUT_DIGITAL_PIN as output for digital clock
    pinMode(RESOLUTION_KNOB_PIN, INPUT); // Set RESOLUTION_KNOB_PIN as input for clock pulse rate knob
    pinMode(CLOCK_IN_PIN, INPUT_PULLUP); // Set CLOCK_IN_PIN as input with pullup for clock input pulses
    pinMode(CLOCK_IN_PLUG_DETECT_PIN, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(CLOCK_IN_PIN), clockInISR, RISING); // Attach interrupt for clock input pulses

    // GAMEBOY LINK
    pinMode(GB_CLOCK_PIN, OUTPUT);
    pinMode(GB_SI_PIN, INPUT);
    pinMode(GB_SO_PIN, OUTPUT);
  
    // Initialize Serial1, Serial2, and Serial3 for MIDI communication (31250 baud rate)
    Serial1.begin(31250); // Initialize UART1 for MIDIINPUT1
    Serial2.begin(31250); // Initialize UART2 for MIDIINPUT2
    Serial3.begin(31250); // Initialize UART3 for MIDIOUTPUT
  
    MIDIINPUT1.begin(MIDI_CHANNEL_OMNI);  // Listen to all MIDI channels on MIDIINPUT1
    MIDIINPUT2.begin(MIDI_CHANNEL_OMNI);  // Listen to all MIDI channels on MIDIINPUT2

    // Initialize I2C for OLED
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("MEGAMIDI MERGE ");
    display.print(VERSION);
    display.setCursor(0, 10);
    display.print("Initializing..");
    display.setCursor(0, 20);
    display.print("rusaaKKMODS @ 2024");
    display.display();
  
    // Set a generic handler for MIDIINPUT1 and MIDIINPUT2 to forward all messages
    MIDIINPUT1.setHandleMessage(forwardMessageInput1);
    MIDIINPUT2.setHandleMessage(forwardMessageInput2);
}

void loop() {
    handleClockOut();
    handleClockIn();
    handleDisplay();

    // Continuously read from all MIDI inputs
    MIDIINPUT1.read();
    MIDIINPUT2.read();
}
