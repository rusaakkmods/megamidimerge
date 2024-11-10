#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_I2C_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Initialize the OLED display using the Adafruit_SSD1306 library
 *
 * This function begins the I2C bus using the Wire library and then
 * initializes the display at the specified I2C address. The display is
 * configured to use a text size of 1 and a white color for the text.
 */
void initDisplay() {
    // Initialize I2C for OLED
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(PRODUCT_NAME);
    display.print(" ");
    display.print(VERSION);
    display.setCursor(0, 10);
    display.print(RUSAAKKMODS);
    display.setCursor(0, 20);
    display.print("Starting...");

    display.display();
}


/**
 * @brief Update the OLED display with the current master clock input, BPM, and clock pulse resolution.
 *
 * This function clears the display, sets the cursor to the top left, and prints the product name.
 * It then sets the cursor to the second row and prints the name of the master clock input.
 * The BPM is printed below the master clock input name.
 * The cursor is then set to the third row and the clock pulse resolution is printed.
 * Finally, the display is updated with the new data.
 *
 * @param[in] master Name of the master clock input (either "MIDI-1" or "MIDI-2").
 * @param[in] bpm The current Beats Per Minute (BPM) value calculated from the master clock input.
 * @param[in] clockResolution The current clock pulse resolution (1, 2, 4, 8, 16, or 32 pulses per beat).
 */
void printDisplay(const char* master, float bpm, int clockResolution) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(PRODUCT_NAME);
    display.setCursor(0, 10);
    display.print(master);
    display.print(" : ");
    display.print(bpm, 0);
    display.setCursor(0, 20);
    display.print("Clock Rate: ");
    display.print(clockResolution);
    display.display();
}
