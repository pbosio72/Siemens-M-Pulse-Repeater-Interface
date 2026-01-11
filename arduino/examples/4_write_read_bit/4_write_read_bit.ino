#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Program to test the write/read process
- The memory is automatically initialized to the starting position (CCW block)
- The user can write bit sequences:
  - Button 1: writes a 1
  - Button 2: writes a 0
- Pressing button 3 allows the head to read the data
- Data will be displayed on the OLED display

- Previous tests showed that the disk actuation speed and reading speed must
  be separate because the disk cannot write Bit 1 at the same speed as 
  disk rotation

Note: It will also be verified that the sleep_time variable allows correct bit reading
*/

// Input pins
// Button 1: writes a bit value of 1
// Button 2: writes a bit value of 0
// Button 3: reads stored data
const int button_1 = 3; // Button 1
const int button_2 = 4; // Button 2
const int button_3 = 5; // Button 3

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
const int sleep_time_rotate = 60; // Relay actuation time during rotation (ms)
const int sleep_time_write = 60; // Relay actuation time during write operation (ms)
const int sleep_time_read = 60; // Relay actuation time during read operation (ms)

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ************************* FUNCTIONS SECTION *************************

// Function to activate/deactivate a relay
void activateRele(int pin_rele, int sleepping) {
  digitalWrite(pin_rele, HIGH);
  delay(sleepping);
  digitalWrite(pin_rele, LOW);
  delay(sleepping);
}

// Function to display status messages on the OLED display
void display_option(int option) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (option == 1) { display.println("Initial SetUp..."); }
  if (option == 2) { display.println("Write bit -> 1"); }
  if (option == 3) { display.println("Write bit -> 0"); }
  if (option == 4) { display.println("Read data..."); }
  if (option == 5) { display.println("End of disk!"); }
  if (option == 6) { display.println("No data available!!"); }
  display.display();
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
  
  // Set the disk head to the initial position (CCW block)
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

}

// ************************* LOOP SECTION *************************
void loop() {
  // Variable declarations
  int buttonState_1, buttonState_2, buttonState_3; 
  int switchState_A, switchState_B, switchState_C ;
  int switchState_read;
  int eod_state; // End of Disk state (1 if the disk is in CW block position; 0 otherwise)

  // Check End of Disk position
  switchState_A = digitalRead(switch_A);
  delay(1); // Delay for reading stability
  switchState_B = digitalRead(switch_B);
  delay(1); // Delay for reading stability
  switchState_C = digitalRead(switch_C);
  delay(1); // Delay for reading stability
  eod_state=0;
  if (switchState_A == LOW) { // && switchState_B == LOW && switchState_C == HIGH) {
    display_option(5);
    eod_state=1;
  }

  if (eod_state==0) {
    // ---- Write bit -> 1
    buttonState_1 = digitalRead(button_1);
    delay(1); // Delay for reading stability
    if (buttonState_1 == HIGH) { // Write bit 1
      display_option(2);
      digitalWrite(rele_1, HIGH);
      delay(sleep_time_write);
      activateRele(rele_3, sleep_time_write);
      digitalWrite(rele_1, LOW);
    }

    // ---- Write bit -> 0
    buttonState_2 = digitalRead(button_2);
    delay(1); // Delay for reading stability
    if (buttonState_2 == HIGH) { // Write bit 0
      display_option(3);
      activateRele(rele_3, sleep_time_rotate); // bit=0 is a simple rotation
    }
  }
    // ---- Read bits
    buttonState_3 = digitalRead(button_3);
    delay(1); // Delay for reading stability
    if (buttonState_3 == HIGH) { // Read stored data
      switchState_C = digitalRead(switch_C);
      delay(1); // Delay for reading stability
      if (switchState_C == LOW) {
        display_option(6); // If the disk is already at the end
      }
      // Head rotation CCW until switch C is triggered
      while (switchState_C == HIGH) {
        display_option(4);
        switchState_read = digitalRead(switch_read); // Read the head value (1 or 0)
        delay(1);
        Serial.println(switchState_read);
        activateRele(rele_2, sleep_time_read); // Rotate the disk
        switchState_C = digitalRead(switch_C); // Check the CCW block position
        delay(1);
      }
    }
}