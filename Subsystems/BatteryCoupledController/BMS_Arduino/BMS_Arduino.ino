#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- Define your BMS communication pins ---
#define BMS_RX_PIN 10 // Connects to BMS TX
#define BMS_TX_PIN 11 // Connects to BMS RX

SoftwareSerial bmsSerial(BMS_RX_PIN, BMS_TX_PIN);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// JBD "Read Basic Info" command
byte jbdRequest[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
byte bmsResponse[34]; 

void setup() {
  // Start Hardware Serial for DSP/Simulink
  // Increased to 115200 to match your ESP example and typical DSP speeds
  Serial.begin(115200);
  
  // Start Software Serial for BMS
  bmsSerial.begin(9600); 

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0); 
  lcd.print("BMS Reader");
  lcd.setCursor(0, 1);
  lcd.print("DSP Link Active");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Flush the receiver buffer BEFORE requesting new data
  while (bmsSerial.available() > 0) {
    bmsSerial.read();
  }

  // 1. Send the request packet to the BMS
  bmsSerial.write(jbdRequest, sizeof(jbdRequest));

  // Wait a short moment for the BMS to process and respond
  // Note: This 100ms + the delay at the end dictates your Simulink sample time
  delay(100); 

  // 2. Read the response
  if (bmsSerial.available() > 0) {
    
    int bytesRead = bmsSerial.readBytes(bmsResponse, 34);

    // Check if we got a valid-looking packet
    if (bytesRead >= 33 && bmsResponse[0] == 0xDD && bmsResponse[1] == 0x03) {
      
      // --- Parse Raw BMS Data ---
      int voltage_raw = (bmsResponse[4] << 8) | bmsResponse[5];
      float total_voltage = (float)voltage_raw * 0.01f;

      int16_t current_raw = (bmsResponse[6] << 8) | bmsResponse[7];
      float current = (float)(current_raw - 0) * 0.01f * -0.2438f; 

      int soc_percent = bmsResponse[23];


// --- 1. SEND DATA TO DSP (SIMULINK) ---
      // Convert floats to integers by multiplying by 100
      int16_t v_out = (int16_t)(total_voltage * 100.0);
      int16_t i_out = (int16_t)(current * 100.0); 
      uint8_t soc_out = (uint8_t)soc_percent;

      Serial.write('S'); // Header

      // --- Send 5 bytes of REAL data ---
      Serial.write(v_out >> 8);
      Serial.write(v_out & 0xFF);
      Serial.write(i_out >> 8);
      Serial.write(i_out & 0xFF);
      Serial.write(soc_out);

      // --- Send 3 bytes of PADDING to hit the 8-byte requirement ---
      Serial.write((byte)0);
      Serial.write((byte)0);
      Serial.write((byte)0);

      Serial.write('E'); // Terminator


      // --- 2. Update the LCD ---
      lcd.setCursor(0, 0); 
      lcd.print(total_voltage, 2); 
      lcd.print("V SOC:");
      lcd.print(soc_percent);
      lcd.print("%  "); 

      lcd.setCursor(0, 1);
      lcd.print("Cur: ");
      lcd.print(current, 2);
      lcd.print("A      "); 

    } else {
      // If error, send empty or zeroed data to keep Simulink in sync
      Serial.write('S');
      Serial.write((byte)0); Serial.write((byte)0); // V = 0
      Serial.write((byte)0); Serial.write((byte)0); // I = 0
      Serial.write((byte)0);                        // SOC = 0
      Serial.write('E');
      
      lcd.setCursor(0, 0);
      lcd.print("Read Error!     ");
      lcd.setCursor(0, 1);
      lcd.print("Check BMS wires ");
    }
  }

  // Match Simulink Sample Time
  // Adjust this delay so your total loop time matches your DSP fixed step size
  delay(100); 
}