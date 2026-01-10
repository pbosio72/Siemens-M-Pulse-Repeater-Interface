/*
Program to activate mechanical memory relays via buttons
Input on pins 3, 4, and 5
Output on pins 10, 11, and 12
*/

// Input button pins
const int button_1 = 3; // Button 1
const int button_2 = 4; // Button 2
const int button_3 = 5; // Button 3

// Output relay pins
const int rele_1 = 10; // Relay 1
const int rele_2 = 11; // Relay 2
const int rele_3 = 12; // Relay 3

// Timing constant
const int sleep_time = 10; // Delay time in milliseconds

// Setup routine runs once when you press reset
void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  
  // Configure button pins as inputs
  pinMode(button_1, INPUT);
  pinMode(button_2, INPUT);
  pinMode(button_3, INPUT);
  
  // Configure relay pins as outputs
  pinMode(rele_1, OUTPUT);
  pinMode(rele_2, OUTPUT);
  pinMode(rele_3, OUTPUT);
  
  // Initialize relays to OFF state
  digitalWrite(rele_1, LOW);
  digitalWrite(rele_2, LOW);
  digitalWrite(rele_3, LOW);
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

  // Control relay 1 based on button 1 state
  if (buttonState_1 == HIGH) {
    digitalWrite(rele_1, HIGH);
    delay(sleep_time);
  } else {
    digitalWrite(rele_1, LOW);
    delay(sleep_time);
  }

  // Control relay 2 based on button 2 state
  if (buttonState_2 == HIGH) {
    digitalWrite(rele_2, HIGH);
    delay(sleep_time);
  } else {
    digitalWrite(rele_2, LOW);
    delay(sleep_time);
  }

  // Control relay 3 based on button 3 state
  if (buttonState_3 == HIGH) {
    digitalWrite(rele_3, HIGH);
    delay(sleep_time);
  } else {
    digitalWrite(rele_3, LOW);
    delay(sleep_time);
  }
}