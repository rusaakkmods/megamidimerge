#include <MIDI.h>  // Include the MIDI library
#include "midi_clock.h"

// Pin Definitions
#define MASTER_SWITCH_PIN 7  // Master Clock switch pin

const int MIDI_BAUD_RATE = 31250;

// MIDI instance for using MEGA 2650 Serial1, Serial2, and Serial3 as MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIINPUT1); // Use Hardware UART1 for INPUT 2
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDIINPUT2); // Use Hardware UART2 for INPUT 1
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDIOUTPUT); // Use Hardware UART3 for MERGE OUT

/**
 * @brief Check if the master clock input is set to Input 1.
 *
 * This function reads the digital state of the MASTER_SWITCH_PIN to determine
 * if the master clock source is set to Input 1. It returns true if the switch
 * is in the HIGH state, indicating that Input 1 is the master clock source.
 *
 * @return True if the master clock input is set to Input 1, false otherwise.
 */

bool isMasterInput1() {
    return digitalRead(MASTER_SWITCH_PIN) == HIGH;
}

/**
 * @brief Forward a MIDI message from an input to the output merge.
 *
 * This function filters out clock and timing-related messages if not from the master
 * clock source and forwards the message to the output merge. It also updates the
 * BPM counter and generates an analog clock pulse if the message is a MIDI Clock
 * message.
 *
 * @param[in] midiInput Reference to the MIDI input interface to forward the message from.
 * @param[in] message The MIDI message to forward.
 */
template <typename T>
void forwardMessage(midi::MidiInterface<T> &midiInput, const midi::Message<128>& message) {
    byte messageType = message.type;
    byte channel = message.channel;
    byte data1 = message.data1;
    byte data2 = message.data2;

    // Filter out clock and timing-related messages if not from the master
    if ((messageType == midi::Clock || messageType == midi::Start || messageType == midi::Continue || messageType == midi::Stop || messageType == midi::ActiveSensing) &&
        ((isMasterInput1() && &midiInput != &MIDIINPUT1) || (!isMasterInput1() && &midiInput != &MIDIINPUT2))) {
        return; // Do not forward these messages if not from the master clock
    }

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
            clockCount++; // BPM Counter
            generateClockPulse(); // Generate analog clock pulse
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

/**
 * @brief Forward a MIDI message received on Input 1 to the MIDI output merge.
 *
 * This function calls forwardMessage(MIDIINPUT1, message) to forward the given
 * message from Input 1 to the output merge.
 *
 * @param message The MIDI message to forward.
 */
void forwardMessageInput1(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT1, message);
}

/**
 * @brief Forward a MIDI message received on Input 2 to the MIDI output merge.
 *
 * This function calls forwardMessage(MIDIINPUT2, message) to forward the given
 * message from Input 2 to the output merge.
 *
 * @param message The MIDI message to forward.
 */
void forwardMessageInput2(const midi::Message<128>& message) {
    forwardMessage(MIDIINPUT2, message);
}

/**
 * @brief Initialize MIDI input interfaces and set message handlers.
 *
 * This function begins listening on all MIDI channels for both MIDI inputs
 * and assigns handler functions to process incoming MIDI messages. It configures
 * MIDI Input 1 and MIDI Input 2 to operate in omni mode, allowing them to 
 * receive messages on all channels.
 */
void initMidi() {
    // Initialize pins
    pinMode(MASTER_SWITCH_PIN, INPUT_PULLUP); // Set CSW_PIN as input for the SPDT master clock switch (non-serial, non-I2C)
    pinMode(CLOCK_OUT1_PIN, OUTPUT); // Set CLOCK_OUT1_PIN as output for analog clock
    pinMode(CLOCK_OUT2_PIN, OUTPUT); // Set CLOCK_OUT2_PIN as output for analog clock
    pinMode(CLOCK_BUTTON_PIN, INPUT_PULLUP); // Set RESOLUTION_BUTTON_PIN as input for resolution button

    // Initialize Serial1, Serial2, and Serial3 for MIDI communication (31250 baud rate)
    Serial1.begin(MIDI_BAUD_RATE); // Initialize UART1 for MIDIINPUT1
    Serial2.begin(MIDI_BAUD_RATE); // Initialize UART2 for MIDIINPUT2
    Serial3.begin(MIDI_BAUD_RATE); // Initialize UART3 for MIDIOUTPUT

    // Start listen all MIDI Input channels
    MIDIINPUT1.begin(MIDI_CHANNEL_OMNI);
    MIDIINPUT2.begin(MIDI_CHANNEL_OMNI);

    // Set handler for all MIDI Input messages
    MIDIINPUT1.setHandleMessage(forwardMessageInput1);
    MIDIINPUT2.setHandleMessage(forwardMessageInput2);
}

/**
 * @brief Read and process incoming MIDI messages from both inputs.
 *
 * This function calls `read()` on both MIDI Input 1 and MIDI Input 2 to read
 * any incoming MIDI messages and process them using the assigned message
 * handlers. It is called from the `loop()` function.
 */
void readMidiInput() {
    MIDIINPUT1.read();
    MIDIINPUT2.read();
}