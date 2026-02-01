#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*
48V POWER SUPPLY

IMPORTANT: for safety reason read the related readme.txt file

DISCLAIMER:
The author(s) of this project DISCLAIM ALL RESPONSIBILITY and LIABILITY
for any injury, death, property damage, or other consequences resulting
from the construction, modification, or use of this system.

By building or using this project, you acknowledge that:
- You understand the risks involved with high voltage systems
- You have the necessary skills and knowledge to work safely
- You accept full responsibility for your safety and others
- You will comply with all local electrical codes and regulations
- You use this information entirely at your own risk

IF YOU ARE NOT QUALIFIED TO WORK WITH HIGH VOLTAGE SYSTEMS,
DO NOT ATTEMPT TO BUILD THIS PROJECT.
*/

/*
Program to test the write/read process with LCD2004 display
- The memory is automatically initialized to the starting position (CCW block)
- Button 1 press:
   The program reads a number sequence stored in an internal variable (myNumber)
    - Numbers from 0 to 9
    - Maximum 15 digits
   Each number is converted into a 4-bit sequence (from 0000 to 1001) that will be
   sequentially stored in memory
- After the write process, the user can read the sequence by pressing button 3
- The read bit sequence is displayed on the LCD2004
- Button 2 is not used
- A flag_write is implemented to prevent consecutive writes. If the flag is set to 0,
  the write operation is allowed, otherwise it's blocked

- Requested library : LiquidCrystal I2C
*/

// Input pins
const int button_1 = 3; // Button 1: write number sequence
const int button_2 = 4; // Button 2: not used
const int button_3 = 5; // Button 3: read stored data

// Input switches
const int switch_A = 6; // Switch A
const int switch_B = 7; // Switch B
const int switch_C = 8; // Switch C
const int switch_read = 9; // Switch reading head

// Output relay pins
const int rele_1 = 10; // Relay 1
const int rele_2 = 11; // Relay 2
const int rele_3 = 12; // Relay 3

// Timing constants
const int sleep_time_rotate = 100; // Relay actuation time during rotation (ms)
const int sleep_time_write = 100; // Relay actuation time during write operation (ms)
const int sleep_time_read = 100; // Relay actuation time during read operation (ms)

const int max_numbers = 15; // Maximum number of single digits to be stored

// Global flag to prevent multiple consecutive writes
int flag_write = 0; // 0 = write allowed, 1 = write blocked

// Number sequence to write
String myNumber = "0123456789012";

// LCD2004 display configuration (20 chars x 4 lines, I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Global variable to store read bit sequence
String bitSequence = "";

// ************************* FUNCTIONS SECTION *************************

// FUNCTION -> Activate/deactivate a relay
void activateRele(int pin_rele, int sleepping) {
  digitalWrite(pin_rele, HIGH);
  delay(sleepping);
  digitalWrite(pin_rele, LOW);
  delay(sleepping);
}

// FUNCTION -> Convert a digit (0-9) to a 4-bit array
void digitToBits(char digit, int bits[4]) {
  int num = digit - '0'; // Convert char to int (e.g., '5' -> 5)
  
  // Extract the 4 bits (from most significant to least significant)
  bits[0] = (num >> 3) & 1; // Bit 3 (MSB)
  bits[1] = (num >> 2) & 1; // Bit 2
  bits[2] = (num >> 1) & 1; // Bit 1
  bits[3] = num & 1;        // Bit 0 (LSB)
}

// FUNCTION -> Write a single bit to memory
void writeSingleBit(int bit_value) {
  if (bit_value == 1) {
    // Write bit 1
    digitalWrite(rele_1, HIGH);
    delay(sleep_time_write);
    activateRele(rele_3, sleep_time_write);
    digitalWrite(rele_1, LOW);
  } else {
    // Write bit 0
    activateRele(rele_3, sleep_time_rotate);
  }
}

// FUNCTION -> Write the entire number sequence to memory
void writeNumberSequence(String number) {
  int bits[4];
  
  for (int i = 0; i < number.length() && i < max_numbers; i++) {
    digitToBits(number[i], bits);
    // Write the 4 bits to memory
    for (int j = 0; j < 4; j++) {
      writeSingleBit(bits[j]);
      delay(10); // Small delay between bits
    } 
  }
  activateRele(rele_3, sleep_time_rotate); // Rotate the disk otherwise the last bit will be ignored
  activateRele(rele_2, sleep_time_read); // Move the head one step back otherwise there will be one digit more
}

// FUNCTION -> Display bit sequence on LCD
void displayBitSequence(String bits) {
  lcd.clear();
  
  // Calculate how many lines we need (20 chars per line)
  int len = bits.length();
  int line = 0;
  int pos = 0;
  
  // Display bits across multiple lines if needed
  for (int i = 0; i < len && line < 4; i++) {
    if (pos >= 20) {
      line++;
      pos = 0;
    }
    if (line < 4) {
      lcd.setCursor(pos, line);
      lcd.print(bits.charAt(i));
      pos++;
    }
  }
}

// ************************* SETUP SECTION *************************
void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  Serial.println("Starting...");
  
  // Configure INPUT pins
  pinMode(button_1, INPUT);    // Button 1
  pinMode(button_2, INPUT);    // Button 2
  pinMode(button_3, INPUT);    // Button 3
  pinMode(switch_A, INPUT);    // Switch A
  pinMode(switch_B, INPUT);    // Switch B
  pinMode(switch_C, INPUT);    // Switch C
  pinMode(switch_read, INPUT); // Switch reading head
  
  // Configure OUTPUT pins
  pinMode(rele_1, OUTPUT);     // Relay 1
  pinMode(rele_2, OUTPUT);     // Relay 2
  pinMode(rele_3, OUTPUT);     // Relay 3
  
  // Initialize relays to OFF state
  digitalWrite(rele_1, LOW);
  digitalWrite(rele_2, LOW);
  digitalWrite(rele_3, LOW);

  // Initialize LCD2004 display
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System starting...");
  Serial.println("LCD initialized");
  delay(2000);
  
  // ACTION -> Set the disk head to the initial position (CCW block)
  int switchState_C;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Positioning head...");
  Serial.println("Positioning head...");
  
  switchState_C = digitalRead(switch_C);
  delay(1); // Delay for reading stability
  
  if (switchState_C == LOW) {
    // If the disk is already at the end, move forward 2 steps and return back
    // to ensure the disk will be in the block+1 position
    activateRele(rele_3, sleep_time_rotate);
    activateRele(rele_3, sleep_time_rotate);
    switchState_C = digitalRead(switch_C);
  }
  
  // Head rotation CCW until switch C is triggered
  while (switchState_C == HIGH) {
    activateRele(rele_2, sleep_time_rotate);
    switchState_C = digitalRead(switch_C);
    delay(1);
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready!");
  lcd.setCursor(0, 1);
  lcd.print("Btn1: Write");
  lcd.setCursor(0, 2);
  lcd.print("Btn3: Read & Display");
  Serial.println("Ready!");
}

// ************************* LOOP SECTION *************************
void loop() {
  // Variable declarations
  int buttonState_1, buttonState_3; 
  int switchState_C;
  int switchState_read;

  // ---- Write number sequence
  buttonState_1 = digitalRead(button_1);
  delay(1); // Delay for reading stability
  
  if (buttonState_1 == HIGH && flag_write == 0) { // Check flag to prevent multiple writes
    flag_write = 1; // Set flag to block further writes
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Writing data...");
    Serial.println("Writing sequence...");
    
    writeNumberSequence(myNumber);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Write complete!");
    Serial.println("Write complete!");
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press Btn3");
    lcd.setCursor(0, 1);
    lcd.print("to read data");
    
    // Wait for button release
    while (digitalRead(button_1) == HIGH) {
      delay(10);
    }
  } else if (buttonState_1 == HIGH && flag_write == 1) {
    // Show message that write is blocked
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Write blocked!");
    lcd.setCursor(0, 1);
    lcd.print("Read data first!");
    Serial.println("Write blocked - read first!");
    delay(2000);
  }

  // ---- Read bits and display on LCD
  buttonState_3 = digitalRead(button_3);
  delay(1); // Delay for reading stability
  
  if (buttonState_3 == HIGH) { // Read stored data
    switchState_C = digitalRead(switch_C);
    delay(1); // Delay for reading stability
    
    if (switchState_C == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No data available!");
      Serial.println("No data - disk at end");
      delay(2000);
    } else {
      // Clear bit sequence string
      bitSequence = "";
      
      lcd.clear();
      Serial.println("Reading data...");
      
      int bitCount = 0;
      int currentLine = 0;
      int currentPos = 0;
      
      // Head rotation CCW until switch C is triggered
      while (switchState_C == HIGH) {
        switchState_read = digitalRead(switch_read); // Read the head value (1 or 0)
        delay(1);
        
        // Get bit value
        int bitValue = !switchState_read;
        
        // Append bit to sequence string
        bitSequence += bitValue ? "1" : "0";
        Serial.print(bitValue); // Print to Serial Monitor
        
        // Display bit on LCD in real-time
        lcd.setCursor(currentPos, currentLine);
        lcd.print(bitValue);
        
        currentPos++;
        if (currentPos >= 20) {
          currentPos = 0;
          currentLine++;
        }
        
        activateRele(rele_2, sleep_time_read); // Rotate the disk
        switchState_C = digitalRead(switch_C); // Check the CCW block position
        delay(1);
      }
      
      Serial.println(); // New line after reading complete
      Serial.print("Bit sequence: ");
      Serial.println(bitSequence);
      flag_write = 0; // Reset flag to allow new writes
      
      // Keep displaying the result
      delay(5000); // Show result for 5 seconds
      
      // Return to ready state
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ready!");
      lcd.setCursor(0, 1);
      lcd.print("Btn1: Write");
      lcd.setCursor(0, 2);
      lcd.print("Btn3: Read & Display");
    }
    
    // Wait for button release
    while (digitalRead(button_3) == HIGH) {
      delay(10);
    }
  }
}
