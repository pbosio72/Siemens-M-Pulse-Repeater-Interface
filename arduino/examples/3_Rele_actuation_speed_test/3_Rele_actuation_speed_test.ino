#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Program to test memory actuation speed
The critic parameters are the sleep_time and sleep_disk that define the
slipping time between two actuations.
- Button 1: moves the disk and head to the CCW block position
- Button 2: writes a 1
- Button 3: writes a 0
*/

// Input pins
// Button 1: turn the disk and the head CCW till the first available position
// Button 2: write a bit value of 1
// Button 3: write a bit value of 0
const int button_1 = 3; // Button 1
const int button_2 = 4; // Button 2
const int button_3 = 5; // Button 3

// Input switches
const int switch_A = 6; // Switch A
const int switch_B = 7; // Switch B
const int switch_C = 8; // Switch C

// Output relay pins
const int rele_1 = 10; // Relay 1
const int rele_2 = 11; // Relay 2
const int rele_3 = 12; // Relay 3

// Timing constants
const int sleep_time = 100; // Sleeping time for relay actuation (ms)

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ************************* FUNCTIONS SECTION *************************

// Function to activate/deactivate a relay
void activateRele(int pin_rele, int sleep_time) {
  digitalWrite(pin_rele, HIGH);
  delay(sleep_time);
  digitalWrite(pin_rele, LOW);
  delay(sleep_time);
}

// Function to display parameters on the mini-display
void display_option(int option) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  if (option == 1) { 
    display.println("Initial SetUp..."); 
  }
  if (option == 2) { 
    display.println("Write bit -> 1"); 
  }
  if (option == 3) { 
    display.println("Write bit -> 0"); 
  }
  display.display();
}

// ************************* SETUP SECTION *************************
void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  
  // Configure INPUT pins
  pinMode(button_1, INPUT); // Button 1
  pinMode(button_2, INPUT); // Button 2
  pinMode(button_3, INPUT); // Button 3
  pinMode(switch_A, INPUT); // Switch A
  pinMode(switch_B, INPUT); // Switch B
  pinMode(switch_C, INPUT); // Switch C
  
  // Configure OUTPUT pins
  pinMode(rele_1, OUTPUT); // Relay 1
  pinMode(rele_2, OUTPUT); // Relay 2
  pinMode(rele_3, OUTPUT); // Relay 3
  
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
}

// ************************* LOOP SECTION *************************
void loop() {
  // Variable declarations
  int buttonState_1, buttonState_2, buttonState_3; 
  int switchState_C;
  
  // ---- Initial setup: move to CCW position
  buttonState_1 = digitalRead(button_1);
  delay(1); // Delay for reading stability
  
  if (buttonState_1 == HIGH) { // Set position to CCW
    display_option(1);
    switchState_C = digitalRead(switch_C);
    delay(1); // Delay for reading stability
    
    if (switchState_C == LOW) {
      // If the disk is already at the end, move forward 2 times and return back
      // to ensure the disk will be in the block+1 position
      Serial.println("Relay 3");
      activateRele(rele_3, sleep_time);
      activateRele(rele_3, sleep_time);
      switchState_C = digitalRead(switch_C);
    }
    
    // Head rotation CCW until switch C is triggered
    while (switchState_C == HIGH) {
      delay(1);
      Serial.println("Relay 2");
      activateRele(rele_2, sleep_time);
      delay(sleep_time);
      switchState_C = digitalRead(switch_C);
    }
  }

  // ---- Write bit -> 1
  buttonState_2 = digitalRead(button_2);
  delay(1); // Delay for reading stability
  
  if (buttonState_2 == HIGH) { // Write the sequence
    display_option(2);
    digitalWrite(rele_1, HIGH);
    delay(sleep_time);
    activateRele(rele_3, sleep_time);
    digitalWrite(rele_1, LOW);
    delay(sleep_time);
  }

  // ---- Write bit -> 0
  buttonState_3 = digitalRead(button_3);
  delay(1); // Delay for reading stability
  
  if (buttonState_3 == HIGH) { // Write bit 0
    display_option(3);
    activateRele(rele_3, sleep_time);
  }
}
