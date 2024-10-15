// Part 3

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Adafruit_CAP1188.h>

// Initialize the TFT display
TFT_eSPI tft = TFT_eSPI();  // Create an instance of the TFT display

// Define software SPI pins for CAP1188
#define CAP1188_SCK   25    // Clock pin
#define CAP1188_MISO  33   // MISO pin
#define CAP1188_MOSI  32   // MOSI pin
#define CAP1188_CS    26   // Chip Select pin

// Adafruit_CAP1188::Adafruit_CAP1188(uint8_t clkpin, uint8_t misopin,
//                                    uint8_t mosipin, uint8_t cspin,
//                                    int8_t resetpin) 

// Initialize CAP1188 with software SPI
Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_SCK, CAP1188_MISO, CAP1188_MOSI, CAP1188_CS, -1);

void setup() {
  Serial.begin(9600);
  
  // Initialize the CAP1188 with software SPI
  if (!cap.begin()) {
    Serial.println("CAP1188 not found. Check wiring!");
    while (1);
  }
  Serial.println("CAP1188 found!");

  // Initialize the TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  
}

void loop() {
  uint8_t touched = cap.touched();
  int touchCount = 0;

  // Count how many sensors are touched
  for (uint8_t i = 0; i < 8; i++) {
    if (touched & (1 << i)) {
      touchCount++;
    }
  }

  // Clear previous display and update the touch count on TFT
  tft.fillScreen(TFT_BLACK); // Clear the screen
  tft.setCursor(0, 20);
  tft.setTextSize(2);

  String text = String(touchCount);
  tft.print(text);
  delay(20); // Adjust the delay as needed
}

// Part 2

// #include <Arduino.h>
// #include <TFT_eSPI.h>
// #include <Adafruit_CAP1188.h>

// // Initialize the TFT display
// TFT_eSPI tft = TFT_eSPI(); 
// // I2C address for CAP1188 (default is 0x29)
// Adafruit_CAP1188 cap = Adafruit_CAP1188();

// void setup() {
//   Serial.begin(9600);
//   if (!cap.begin()) {
//     Serial.println("CAP1188 not found. Check wiring!");
//     while (1);
//   }
//   Serial.println("CAP1188 found!");

//   // Initialize the TFT display
//   tft.init();
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLACK);
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.setTextSize(2);
// }

// void loop() {
//   uint8_t touched = cap.touched();
//   int touchCount = 0;

//   // Count how many sensors are touched
//   for (uint8_t i = 0; i < 8; i++) {
//     if (touched & (1 << i)) {
//       touchCount++;
//     }
//   }

//   // Clear previous display and update the touch count on TFT
//   tft.fillScreen(TFT_BLACK); // Clear the screen
//   tft.setCursor(0, 20);
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Set text color to white with black background
//   tft.setTextSize(2);

//   String text = String(touchCount);
//   tft.print(text);

//   delay(20); // Adjust the delay as needed
// }


// Part 1

// #include <Adafruit_I2CDevice.h>
// #include <Arduino.h>
// #include <TFT_eSPI.h>

// TFT_eSPI tft = TFT_eSPI();  // Create display object

// unsigned long previousMillis = 0;  // Store last time counter was updated
// int counter = 0;  // Seconds counter

// void setup() {
//   tft.init();
//   tft.setRotation(1);  // Set rotation to landscape mode

//   tft.fillScreen(TFT_BLACK);  // Clear the screen
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Set text color to white with black background
//   tft.setTextSize(10);  // Set text size
// }

// void loop() {
//   unsigned long currentMillis = millis();  // Get current time

//   // Update the counter every 1000 milliseconds (1 second)
//   if (currentMillis - previousMillis >= 1000) {
//     previousMillis = currentMillis;  // Update last time

//     // Clear the previous counter value
//     tft.fillScreen(TFT_BLACK);  // Clear the screen

//     // Increment and display the counter
//     counter++;
//     String counterText = String(counter);
//     tft.setCursor(0, 40);
//     tft.print(counterText);
//   }
// }
