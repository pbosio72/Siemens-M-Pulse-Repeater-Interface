#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Program to test the write/read process
- The memory is automatically initialized to the starting position (CCW block)
- Button 1 press:
   The program reads a number sequence stored in an internal variable (myNumber)
    - Numbers from 0 to 9
    - Maximum 15 digits
   Each number is converted into a 4-bit sequence (from 0000 to 1001) that will be
   sequentially stored in memory
- After the write process, the user can read the sequence by pressing button 3
- Button 2 is not used
- A flag_write is implemented to prevent consecutive writes. If the flag is set to 0,
  the write operation is allowed, otherwise it's blocked
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

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ************************* FUNCTIONS SECTION *************************

// FUNCTION -> Activate/deactivate a relay
void activateRele(int pin_rele, int sleepping) {
  digitalWrite(pin_rele, HIGH);
  delay(sleepping);
  digitalWrite(pin_rele, LOW);
  delay(sleepping);
}

// FUNCTION -> Display status messages on the OLED display
void display_option(int option) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if (option == 1) { display.println("Initial SetUp..."); }
  if (option == 2) { display.println("Writing sequence..."); }
  if (option == 3) { display.println("Write complete!"); }
  if (option == 4) { display.println("Reading data..."); }
  if (option == 5) { display.println("End of disk!"); }
  if (option == 6) { display.println("No data available!"); }
  if (option == 7) { 
    display.println("Write blocked!");
    display.println("Read first!");
  }
  
  display.display();
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
  
  display_option(2); // Show "Writing sequence..."
  
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
  display_option(3); // Show "Write complete!"
  delay(1000); // Display message for 1 second
}

// ************************* SETUP SECTION *************************
void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  
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

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found");
    while (1); // Halt if display initialization fails
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Ready!");
  display.display();
  delay(1000);
  
  // ACTION -> Set the disk head to the initial position (CCW block)
  int switchState_C;
  display_option(1);
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
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Ready to write!");
  display.println("Btn1: Write");
  display.println("Btn3: Read");
  display.display();
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
    writeNumberSequence(myNumber);
    
    // Wait for button release
    while (digitalRead(button_1) == HIGH) {
      delay(10);
    }
  } else if (buttonState_1 == HIGH && flag_write == 1) {
    // Show message that write is blocked
    display_option(7);
    delay(1000);
  }

  // ---- Read bits
  buttonState_3 = digitalRead(button_3);
  delay(1); // Delay for reading stability
  
  if (buttonState_3 == HIGH) { // Read stored data
    switchState_C = digitalRead(switch_C);
    delay(1); // Delay for reading stability
    
    if (switchState_C == LOW) {
      display_option(6); // If the disk is already at the end
      delay(1000);
    } else {
      // Head rotation CCW until switch C is triggered
      display_option(4); // Show "Reading data..."
      
      while (switchState_C == HIGH) {
        switchState_read = digitalRead(switch_read); // Read the head value (1 or 0)
        delay(1);
        Serial.print(!switchState_read); // Print bit value without newline
        
        activateRele(rele_2, sleep_time_read); // Rotate the disk
        switchState_C = digitalRead(switch_C); // Check the CCW block position
        delay(1);
      }
      
      Serial.println(); // New line after reading complete
      flag_write = 0; // Reset flag to allow new writes
      
      // Return to ready state
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Read complete!");
      display.println("Ready to write!");
      display.display();
      delay(2000);
    }
    
    // Wait for button release
    while (digitalRead(button_3) == HIGH) {
      delay(10);
    }
  }
}