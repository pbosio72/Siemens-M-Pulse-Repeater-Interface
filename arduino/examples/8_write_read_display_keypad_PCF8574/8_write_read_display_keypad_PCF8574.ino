#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PCF8574.h>

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
MECHANICAL MEMORY SYSTEM WITH KEYPAD INPUT
===========================================

This sketch implements a complete mechanical memory system with 3x4 matrix keypad input.
Numbers (0-9) are converted to 4-bit BCD sequences and stored mechanically on a rotating disk.
The system uses a PCF8574 I2C expander to handle the keypad matrix.

REQUIRED LIBRARIES (install via Library Manager):
- Wire.h (built-in)
- LiquidCrystal_I2C by Frank de Brabander
- PCF8574 library by Rob Tillaart

HARDWARE:
- Arduino Uno
- LCD2004 (20x4 with I2C module, address 0x27)
- PCF8574 I2C expander (address 0x20)
- 3x4 Matrix Keypad connected to PCF8574 (P0-P6)
- 3 Relays on Arduino pins 10, 11, 12
- Position switch on Arduino pin 8 (CCW limit sensor)
- Read head switch on Arduino pin 9
- Mechanical memory disk and head assembly

KEYPAD LAYOUT:
  1  2  3
  4  5  6
  7  8  9
  *  0  #

KEYPAD PIN MAPPING (tested configuration):
// Pin out inverted for montability reasons
Physical Pin → PCF8574 Pin → Function
Pin 1        → P6          → Column 2 (2, 5, 8, 0)
Pin 2        → P5          → Row 1 (1, 2, 3)
Pin 3        → P4          → Column 1 (1, 4, 7, *)
Pin 4        → P3          → Row 4 (*, 0, #)
Pin 5        → P2          → Column 3 (3, 6, 9, #)
Pin 6        → P1          → Row 3 (7, 8, 9)
Pin 7        → P0          → Row 2 (4, 5, 6)

FUNCTIONS:
- Keys 0-9: Input numbers (max 15 digits), each digit is immediately written to memory
- Key *: Reset system (reposition head to CCW block, clear sequence)
- Key #: Read data from memory and display decoded decimal numbers in real-time

OPERATION:
1. System initializes and auto-resets head to CCW position
2. User enters digits 0-9, each is immediately converted to 4 bits and written
3. LCD shows "Input Sequence" with entered digits
4. Press * to reset system and clear sequence
5. Press # to read memory and display decoded digits in real-time
*/

// Output relay pins (on Arduino)
const int rele_1 = 10; // Relay 1: Write bit 1 mechanism
const int rele_2 = 11; // Relay 2: Head rotation control
const int rele_3 = 12; // Relay 3: Disk rotation control

// Input switches (on Arduino)
const int switch_A = 6; // Switch A (reserved for future use)
const int switch_B = 7; // Switch B (reserved for future use)
const int switch_C = 8; // Switch C: CCW (counter-clockwise) limit sensor
const int switch_read = 9; // Switch: Reading head position sensor

// Timing constants (milliseconds)
const int sleep_time_rotate = 100; // Relay actuation time during rotation
const int sleep_time_write = 100;  // Relay actuation time during write operation
const int sleep_time_read = 100;   // Relay actuation time during read operation
const int delay_small = 200;       // Small delay for text display (sliding effect)
const int delay_medium = 2000;     // Medium delay for status messages
const int delay_long = 3000;       // Long delay for important messages
const int delay_huge = 5000;       // Extended delay for final results

// Memory configuration
const int max_numbers = 15; // Maximum number of digits that can be stored

// Global variables
String inputSequence = ""; // Stores user input digits (decimal format)
String bitSequence = "";   // Stores read bit sequence (binary format)
int digitCount = 0;        // Counter for entered digits

// LCD2004 display configuration (20 characters x 4 lines, I2C address 0x27)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// PCF8574 I2C expander (I2C address 0x20)
PCF8574 pcf(0x20);

// Keypad matrix configuration
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

// Keypad key mapping
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Pin out inverted for montability reasons
// PCF8574 pin assignments (based on physical testing)
// Rows control which horizontal line is active
byte rowPins[ROWS] = {5, 0, 1, 3}; // P5, P0, P1, P3
// Columns detect which vertical line is pressed
byte colPins[COLS] = {4, 6, 2};    // P4, P6, P2

// ************************* KEYPAD FUNCTIONS *************************

// FUNCTION -> Scan keypad matrix and return pressed key (or 0 if none)
char scanKeypad() {
  // Scan each row
  for (int r = 0; r < ROWS; r++) {
    // Prepare byte with current row LOW, others HIGH
    byte rowPattern = 0xFF; // All HIGH initially
    bitClear(rowPattern, rowPins[r]); // Set current row LOW
    
    // Write pattern to PCF8574
    pcf.write8(rowPattern);
    delay(5); // Small delay for signal stability
    
    // Read all pins from PCF8574
    byte readValue = pcf.read8();
    
    // Check each column
    for (int c = 0; c < COLS; c++) {
      if (bitRead(readValue, colPins[c]) == LOW) {
        // Key pressed! Wait for release to avoid multiple triggers
        while (bitRead(pcf.read8(), colPins[c]) == LOW) {
          delay(10);
        }
        delay(50); // Debounce delay
        return keys[r][c];
      }
    }
  }
  
  return 0; // No key pressed
}

// ************************* MEMORY FUNCTIONS *************************

// FUNCTION -> Activate/deactivate a relay with specified timing
void activateRele(int pin_rele, int sleepping) {
  digitalWrite(pin_rele, HIGH);
  delay(sleepping);
  digitalWrite(pin_rele, LOW);
  delay(sleepping);
}

// FUNCTION -> Convert a decimal digit (0-9) to a 4-bit binary array
void digitToBits(char digit, int bits[4]) {
  int num = digit - '0'; // Convert char to int (e.g., '5' -> 5)
  
  // Extract the 4 bits using bitwise operations (MSB to LSB)
  bits[0] = (num >> 3) & 1; // Bit 3 (Most Significant Bit)
  bits[1] = (num >> 2) & 1; // Bit 2
  bits[2] = (num >> 1) & 1; // Bit 1
  bits[3] = num & 1;        // Bit 0 (Least Significant Bit)
}

// FUNCTION -> Write a single bit to mechanical memory
void writeSingleBit(int bit_value) {
  if (bit_value == 1) {
    // Write bit 1: activate write mechanism, then rotate disk
    digitalWrite(rele_1, HIGH);
    delay(sleep_time_write);
    activateRele(rele_3, sleep_time_write);
    digitalWrite(rele_1, LOW);
  } else {
    // Write bit 0: only rotate disk (no write mechanism)
    activateRele(rele_3, sleep_time_rotate);
  }
}

// FUNCTION -> Write a single digit to memory (converts to 4 bits and writes sequentially)
void writeDigitToMemory(char digit) {
  int bits[4];
  digitToBits(digit, bits);
  
  // Write each of the 4 bits to memory
  for (int j = 0; j < 4; j++) {
    writeSingleBit(bits[j]);
    delay(10); // Small delay between bits for mechanical stability
  }
}

// ************************* DISPLAY FUNCTIONS *************************

// FUNCTION -> Display text character by character with delay (sliding effect)
void printSlow(const char* text, unsigned long delayMs) {
  while (*text) {
    lcd.print(*text);
    delay(delayMs);
    text++;
  }
}

// ************************* SYSTEM FUNCTIONS *************************

// FUNCTION -> Reset system: clear sequence and reposition head to CCW block
void resetSystem() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Reset System");
  delay(delay_medium);
  
  // Check current position of CCW limit switch
  int switchState_C = digitalRead(switch_C);
  delay(1); // Delay for reading stability
  
  if (switchState_C == LOW) {
    // If disk is already at the end position, move forward 2 steps then return
    // This ensures proper positioning at block+1
    activateRele(rele_3, sleep_time_rotate);
    activateRele(rele_3, sleep_time_rotate);
    switchState_C = digitalRead(switch_C);
  }
  
  // Rotate head counter-clockwise until CCW limit switch is triggered
  while (switchState_C == HIGH) {
    activateRele(rele_2, sleep_time_rotate);
    switchState_C = digitalRead(switch_C);
    delay(1); // Delay for reading stability
  }
  
  // Clear all sequence data
  inputSequence = "";
  digitCount = 0;
  
  // Display ready message
  lcd.clear();
  lcd.setCursor(0, 0);
  printSlow("Siemens Memory Ready", delay_small);
  lcd.setCursor(0, 2);
  lcd.print("Enter digits 0-9");
  lcd.setCursor(0, 3);
  lcd.print(" *->Reset    #->Read");
  delay(delay_long);
  
  // Display input prompt
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Input Sequence");
}

// FUNCTION -> Read memory and decode to decimal digits in real-time
void readAndDecodeMemory() {
  // Check if disk is at valid starting position
  int switchState_C = digitalRead(switch_C);
  delay(1); // Delay for reading stability

  if (switchState_C == LOW) {
    // Disk is at end position, cannot read
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No data available!");
    delay(delay_long);
    return;
  }

  // Display read header
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     Read Data ");
  lcd.setCursor(0, 2);
  lcd.print("-> ");
  
  String fourBits = "";  // Buffer to accumulate 4 bits
  int cursorPos = 3;     // Current LCD cursor position (starting after "-> ")
  int lineNumber = 2;    // Current line (starting at line 2)

  // Read bits from memory until CCW limit is reached
  while (switchState_C == HIGH) {
    // Read current bit from head switch
    int switchState_read = digitalRead(switch_read);
    delay(1); // Delay for reading stability

    // Convert to bit character (inverted logic)
    char bit = (!switchState_read) ? '1' : '0';
    fourBits += bit;
    
    // When 4 bits are collected, decode to decimal digit
    if (fourBits.length() == 4) {
      // Convert 4-bit binary to decimal value
      int decimalValue = 0;
      for (int j = 0; j < 4; j++) {
        if (fourBits.charAt(j) == '1') {
          decimalValue += (1 << (3 - j)); // Bit weighting: 8, 4, 2, 1
        }
      }
      
      // Display decoded digit on LCD
      lcd.print(decimalValue);
      cursorPos++;
      delay(delay_small); // Small delay for visual effect
      
      // Move to next line if current line is full (20 chars)
      if (cursorPos >= 20) {
        lineNumber++;
        if (lineNumber < 4) { // Stay within LCD bounds
          lcd.setCursor(0, lineNumber);
          cursorPos = 0;
        }
      }
      
      fourBits = ""; // Clear buffer for next digit
    }
    
    // Rotate disk to next position
    activateRele(rele_2, sleep_time_read);
    switchState_C = digitalRead(switch_C);
    delay(1); // Delay for reading stability
  }
  
  // Display completion message
  lcd.setCursor(0, 3);
  lcd.print("                Done");
  delay(delay_huge);
  
  // Clear input sequence for next operation
  inputSequence = "";
  digitCount = 0;
  
  // Return to input mode
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Input Sequence");
}

// ************************* SETUP SECTION *************************
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Configure Arduino digital pins
  pinMode(switch_A, INPUT);
  pinMode(switch_B, INPUT);
  pinMode(switch_C, INPUT);
  pinMode(switch_read, INPUT);
  pinMode(rele_1, OUTPUT);
  pinMode(rele_2, OUTPUT);
  pinMode(rele_3, OUTPUT);
  
  // Initialize all relays to OFF state
  digitalWrite(rele_1, LOW);
  digitalWrite(rele_2, LOW);
  digitalWrite(rele_3, LOW);
  
  // Initialize LCD2004 display
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System starting...");
  delay(delay_medium);
  
  // Initialize PCF8574 I2C expander
  if (!pcf.begin()) {
    Serial.println("ERROR: PCF8574 not found!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PCF8574 ERROR!");
    while (1); // Halt execution if PCF8574 fails
  }
   
  // Configure PCF8574 pins for keypad matrix
  // All pins start as INPUT with pull-up (default for PCF8574)
  // Rows will be driven LOW one at a time during scanning
  // Columns will be read to detect key presses
  pcf.write8(0xFF); // Set all pins HIGH initially
  
  Serial.println("PCF8574 initialized");
  resetSystem();
}

// ************************* LOOP SECTION *************************
void loop() {
  // Scan keypad for user input
  char key = scanKeypad();
  
  if (key) { 
    // Handle numeric keys (0-9): input and write to memory
    if (key >= '0' && key <= '9') {
      if (digitCount < max_numbers) {
        // Add digit to input sequence
        inputSequence += key;
        digitCount++;
        
        // Update display to show current sequence
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Input Sequence");
        lcd.setCursor(0, 1);
        lcd.print("-> ");
        lcd.print(inputSequence);
        
        // Show writing status
        lcd.setCursor(0, 3);
        lcd.print("Writing Memory... ");
        
        // Write digit to memory immediately (4 bits)
        writeDigitToMemory(key);
        
        // Clear writing message and show progress
        lcd.setCursor(0, 3);
        lcd.print("              "); // Clear previous text
        lcd.print(digitCount);
        lcd.print("/");
        lcd.print(max_numbers);

      } else {
        // Maximum digit limit reached
        lcd.setCursor(0, 3);
        lcd.print("Max digits reached!");
        delay(delay_long);
        lcd.setCursor(0, 3);
        lcd.print("                    "); // Clear message
      }
    }
    
    // Handle * key: reset system
    else if (key == '*') {
      resetSystem();
    }
    
    // Handle # key: read and decode memory
    else if (key == '#') {
      if (digitCount > 0) {
        // Finalize write operation with compensation moves
        activateRele(rele_3, sleep_time_rotate); // Extra disk rotation
        activateRele(rele_2, sleep_time_read);   // Head moves back one position
        
        // Read memory and display decoded digits
        readAndDecodeMemory();
      } else {
        // No data has been entered yet
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No data to read!");
        delay(delay_long);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Input Sequence");
      }
      // Reset digit counter for next write operation
      digitCount = 0;
    }
  }
}
