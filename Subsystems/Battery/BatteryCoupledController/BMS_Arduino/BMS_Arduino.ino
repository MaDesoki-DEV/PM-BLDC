/**
 * @file BMS_DSP_Interface.ino
 * @brief Interfaces a JBD BMS with a DSP (Simulink) and an I2C LCD display.
 * 
 * Display UI Map (16x2):
 * [V : x x . x x   I : - x x . x x]
 * [S : x x x %   B : O K   D : T X]
 */

#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// ==============================================================================
// Constants & Configuration
// ==============================================================================

// Pin Definitions
constexpr uint8_t BMS_RX_PIN = 10;   
constexpr uint8_t BMS_TX_PIN = 11;   

// Serial Baud Rates
constexpr uint32_t DSP_BAUD_RATE = 115200;
constexpr uint32_t BMS_BAUD_RATE = 9600;

// LCD Configuration
constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS    = 2;

// BMS Communication Constants
constexpr uint8_t BMS_RESPONSE_SIZE = 34;
constexpr uint8_t JBD_HEADER_BYTE   = 0xDD;
constexpr uint8_t JBD_SUCCESS_BYTE  = 0x03;

// Loop Timing
constexpr uint32_t BMS_WAIT_TIME_MS = 100;
constexpr uint32_t DSP_SAMPLE_DELAY_MS = 100; 

// ==============================================================================
// Global Objects & Data Structures
// ==============================================================================

SoftwareSerial bmsSerial(BMS_RX_PIN, BMS_TX_PIN);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

const uint8_t jbdRequestCmd[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};

struct BmsData {
  float voltage;
  float current;
  uint8_t soc;
};

// ==============================================================================
// Function Prototypes
// ==============================================================================
void clearBmsBuffer();
bool readBmsData(BmsData& data);
void sendDataToDsp(const BmsData& data, bool isValid);
void updateDisplay(const BmsData& data, bool isBmsConnected);

// ==============================================================================
// Setup & Main Loop
// ==============================================================================

void setup() {
  Serial.begin(DSP_BAUD_RATE);
  bmsSerial.begin(BMS_BAUD_RATE); 

  lcd.init();
  lcd.backlight();
  
  // Elegant Startup Screen
  lcd.setCursor(0, 0); 
  lcd.print("   BMS Reader   ");
  lcd.setCursor(0, 1);
  lcd.print("  Initializing  ");
  delay(2000);
  lcd.clear();
}

void loop() {
  BmsData currentData = {0.0f, 0.0f, 0}; 
  
  // 1. Fetch data
  bool isBmsConnected = readBmsData(currentData);

  // 2. Transmit to DSP
  sendDataToDsp(currentData, isBmsConnected);

  // 3. Render clean UI
  updateDisplay(currentData, isBmsConnected);

  // 4. Synchronize loop step
  delay(DSP_SAMPLE_DELAY_MS); 
}

// ==============================================================================
// Helper Functions
// ==============================================================================

void clearBmsBuffer() {
  while (bmsSerial.available() > 0) {
    bmsSerial.read();
  }
}

bool readBmsData(BmsData& data) {
  uint8_t bmsResponse[BMS_RESPONSE_SIZE];

  clearBmsBuffer();
  bmsSerial.write(jbdRequestCmd, sizeof(jbdRequestCmd));
  delay(BMS_WAIT_TIME_MS); 

  if (bmsSerial.available() > 0) {
    int bytesRead = bmsSerial.readBytes(bmsResponse, BMS_RESPONSE_SIZE);

    if (bytesRead >= (BMS_RESPONSE_SIZE - 1) && 
        bmsResponse[0] == JBD_HEADER_BYTE && 
        bmsResponse[1] == JBD_SUCCESS_BYTE) {
      
      uint16_t voltageRaw = (bmsResponse[4] << 8) | bmsResponse[5];
      data.voltage = (float)voltageRaw * 0.01f;

      int16_t currentRaw = (bmsResponse[6] << 8) | bmsResponse[7];
      data.current = (float)currentRaw * 0.01f * 0.25f; 

      data.soc = bmsResponse[23];

      return true; 
    }
  }
  return false; 
}

void sendDataToDsp(const BmsData& data, bool isValid) {
  Serial.write('S'); 

  if (isValid) {
    int16_t vOut = (int16_t)(data.voltage * 100.0f);
    int16_t iOut = (int16_t)(data.current * 100.0f); 

    Serial.write(vOut >> 8);
    Serial.write(vOut & 0xFF);
    Serial.write(iOut >> 8);
    Serial.write(iOut & 0xFF);
    Serial.write(data.soc);
  } else {
    for (uint8_t i = 0; i < 5; i++) {
      Serial.write((uint8_t)0);
    }
  }

  // Simulink padding
  Serial.write((uint8_t)0);
  Serial.write((uint8_t)0);
  Serial.write((uint8_t)0);

  Serial.write('E'); 
}

/**
 * @brief Formats strings to fixed widths to prevent UI jitter on the LCD.
 */
void updateDisplay(const BmsData& data, bool isBmsConnected) {
  char row0[17];
  char row1[17];

  if (isBmsConnected) {
    char vStr[8];
    char iStr[8];
    
    // dtostrf standardizes float conversion on Arduino. 
    // Format: (variable, minimum_width, decimal_places, target_buffer)
    dtostrf(data.voltage, 5, 2, vStr);
    dtostrf(data.current, 6, 2, iStr);

    // %5.5s and %6.6s force the string to be EXACTLY 5 and 6 characters long.
    // This strictly prevents text overflow if the voltage goes over 100V.
    // Total string length is strictly 16 chars: "V:"(2) + 5 + " I:"(3) + 6 = 16
    snprintf(row0, sizeof(row0), "V:%5.5s I:%6.6s", vStr, iStr);

    // %3d forces SOC to take up 3 characters, aligning single/double digits to the right.
    // Total string length is strictly 16 chars: "S:"(2) + 3 + "% B:OK D:TX"(11) = 16
    snprintf(row1, sizeof(row1), "S:%3d%% B:OK D:TX", data.soc);

  } else {
    // Elegant error state that maintains the exact same visual structure
    snprintf(row0, sizeof(row0), "V:--.-- I:--.-- ");
    snprintf(row1, sizeof(row1), "S:--%%  B:ER D:TX"); 
  }

  // Print constructed rows (no clearing needed, overwrites perfectly)
  lcd.setCursor(0, 0);
  lcd.print(row0);
  lcd.setCursor(0, 1);
  lcd.print(row1);
}