#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>  // Include the MIDI library

#define VERSION "v0.0.1"

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_I2C_ADDRESS 0x3C
#define CSW_PIN 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MIDI instance for using Serial1, Serial2, and Serial3 as MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIINPUT1); // Use Hardware UART1 for INPUT 2
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDIINPUT2); // Use Hardware UART2 for INPUT 1
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDIOUTPUT); // Use Hardware UART3 for MERGE OUT

// Variables for BPM
volatile unsigned long clockCount = 0;   // Count of MIDI Clock messages
unsigned long lastBPMTime = 0;            // Last time BPM was calculated
float bpm = 0;                             // Calculated BPM
const int numerator = 4;                   // Fixed numerator for 4/4
const int denominator = 4;                 // Fixed denominator for 4/4 (quarter note)
bool masterClock1 = true;

void printDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("MEGA MIDI MERGE");
    display.setCursor(0, 10);
    display.print(masterClock1 ? "MIDIINPUT1 : " : "MIDIINPUT2 : ");
    display.print(bpm);
    display.setCursor(0, 20);
    display.print("rusaaKKMODS @ 2024");
    display.display();
}

void calculateBPM() {
  // Calculate BPM based on the number of clock messages received
  if (numerator > 0) {
    bpm = (clockCount / 24.0) * 60.0 * (4.0 / numerator); // Adjust BPM based on fixed numerator
  } else {
    bpm = 0; // Prevent division by zero
  }
  clockCount = 0; // Reset the clock count after calculation
}

// Function to forward a MIDI message to a given MIDI output
template <typename T>
void forwardMessage(midi::MidiInterface<T> &midiInput, const midi::Message<128>& message) {
    byte type = message.type;

    // Filter out clock and timing-related messages if not from the master
    if ((type == midi::Clock || type == midi::Start || type == midi::Continue || type == midi::Stop || type == midi::ActiveSensing) &&
        ((masterClock1 && &midiInput != &MIDIINPUT1) || (!masterClock1 && &midiInput != &MIDIINPUT2))) {
        return; // Do not forward these messages if not from the master clock
    }

    // BPM Counter
    if (type == midi::Clock) clockCount++;
  
    byte channel = message.channel;
    byte data1 = message.data1;
    byte data2 = message.data2;
    
    switch (type) {
        case midi::NoteOn:
            MIDIOUTPUT.sendNoteOn(data1, data2, channel);
            break;
        case midi::NoteOff:
            MIDIOUTPUT.sendNoteOff(data1, data2, channel);
            break;
        case midi::ProgramChange:
            MIDIOUTPUT.sendProgramChange(data1, channel);
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
      
        // Handle MIDI Clock and timing-related messages with precise forwarding
        case midi::Clock:
            MIDIOUTPUT.sendRealTime(midi::Clock);
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

// Generic MIDI Message handler to forward all MIDI1 messages to MIDI2 and MIDI3
void forwardMessageInput1(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT1, message);
}

// Generic MIDI Message handler to forward all MIDI2 messages to MIDI1 and MIDI3
void forwardMessageInput2(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT2, message);
}

void setup() {
    pinMode(CSW_PIN, INPUT_PULLUP); // Set CSW_PIN as input for the SPDT master clock switch (non-serial, non-I2C)
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED as an output

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
    display.print("MEGA MIDI MERGE");
    display.setCursor(0, 10);
    display.print(VERSION);
    display.setCursor(0, 20);
    display.print("rusaaKKMODS @ 2024");
    display.display();
  
    // Set a generic handler for MIDIINPUT1 and MIDIINPUT2 to forward all messages
    MIDIINPUT1.setHandleMessage(forwardMessageInput1);
    MIDIINPUT2.setHandleMessage(forwardMessageInput2);
}

/**
 * @brief Main loop for the MEGA MIDI MERGE
 *
 * This function continuously runs in the background and does the following:
 * 1. Checks if the master clock source has changed (using the SPDT switch)
 * 2. If the master clock source has changed, updates the OLED display with the current master clock status
 * 3. Continuously reads from all MIDI inputs (MIDIINPUT1 and MIDIINPUT2) to forward messages to the merge output
 */
void loop() {
    masterClock1 = digitalRead(CSW_PIN) == HIGH;

    // Update BPM every second
    unsigned long currentTime = millis();
    if (currentTime - lastBPMTime >= 1000) { // Calculate BPM every second
        calculateBPM();
        lastBPMTime = currentTime;
        printDisplay();
    }

    // Continuously read from all MIDI inputs
    MIDIINPUT1.read();
    MIDIINPUT2.read();
}
