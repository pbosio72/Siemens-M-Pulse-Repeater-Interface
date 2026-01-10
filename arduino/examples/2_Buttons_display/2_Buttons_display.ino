#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
Program to activate mechanical memory relays via buttons
Input on pins 3, 4, and 5
Output on pins 10, 11, and 12
*/

// Input button pins
const int button_1 = 3; // Button 1
const int button_2 = 4; // Button 2
const int button_3 = 5; // Button 3

// Input switch pins
const int switch_A = 6; // Switch A
const int switch_B = 7; // Switch B
const int switch_C = 8; // Switch C

// Output relay pins
const int rele_1 = 10; // Relay 1
const int rele_2 = 11; // Relay 2
const int rele_3 = 12; // Relay 3

// Timing constants
const int sleep_time = 10; // Delay time in milliseconds

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Setup routine runs once when you press reset
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

// Loop routine runs over and over again forever
void loop() {
  // Read button states
  int buttonState_1 = digitalRead(button_1);
  delay(1); // Delay for reading stability
  int buttonState_2 = digitalRead(button_2);
  delay(1); // Delay for reading stability
  int buttonState_3 = digitalRead(button_3);
  delay(1); // Delay for reading stability
  
  // Activate relays based on button states
  rele_write(buttonState_1, buttonState_2, buttonState_3, sleep_time);

  // Read switch states
  int switchState_A = digitalRead(switch_A);
  delay(1); // Delay for reading stability
  int switchState_B = digitalRead(switch_B);
  delay(1); // Delay for reading stability
  int switchState_C = digitalRead(switch_C);
  delay(1); // Delay for reading stability
  
  // Display the state of switches A, B, C
  display_data(switchState_A, switchState_B, switchState_C);
}

// Function to control relays
void rele_write(int buttonState_1, int buttonState_2, int buttonState_3, int sleep_time) {
  digitalWrite(rele_1, buttonState_1);
  digitalWrite(rele_2, buttonState_2);
  digitalWrite(rele_3, buttonState_3);
  delay(sleep_time);
}

// Function to display switch states on OLED
void display_data(int a, int b, int c) {
  String A = "OFF"; // String version of boolean data
  String B = "OFF"; // String version of boolean data
  String C = "OFF"; // String version of boolean data
  
  if (a == HIGH) { A = "ON"; }
  if (b == HIGH) { B = "ON"; }
  if (c == HIGH) { C = "ON"; }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Switch A: " + A);
  display.println("Switch B: " + B);
  display.println("Switch C: " + C);
  display.display();
}