Test for the rele speed actuation

**File:** `Rele_actuation_speed_test.ino`

**Purpose:** This sketch is designed to test the actuation speed of the mechanical memory system. It provides functionality to position the disk and head, and write individual bits to the memory.

**Hardware Requirements:**
- 3 push buttons (pins 3, 4, 5)
- 3 switches for position feedback (pins 6, 7, 8)
- 3 relays for mechanical actuation (pins 10, 11, 12)
- OLED display (128x64, I2C address 0x3C)

**Functionality:**
- **Button 1:** Initiates setup procedure - moves the disk and head to the CCW (counter-clockwise) block position. The system uses Switch C to detect when the initial position is reached. If already at the end position, it moves forward twice then returns to ensure proper positioning.
- **Button 2:** Writes a bit value of 1 to the memory. Activates Relay 1, then pulses Relay 3 to write the bit, then deactivates Relay 1.
- **Button 3:** Writes a bit value of 0 to the memory. Pulses Relay 3 to write the bit.

**Display Feedback:**
- Shows "Initial SetUp..." when Button 1 is pressed
- Shows "Write bit -> 1" when Button 2 is pressed
- Shows "Write bit -> 0" when Button 3 is pressed

**Timing Parameters:**
- Relay actuation time: 100ms
- Disk movement delay: 100ms

## Pin Mapping

| Component | Pin Number |
|-----------|------------|
| Button 1  | 3          |
| Button 2  | 4          |
| Button 3  | 5          |
| Switch A  | 6          |
| Switch B  | 7          |
| Switch C  | 8          |
| Relay 1   | 10         |
| Relay 2   | 11         |
| Relay 3   | 12         |

## Libraries Required
- Wire.h (built-in)
- Adafruit_GFX.h
- Adafruit_SSD1306.h

## Notes

- All sketches initialize relays to the OFF state (LOW) on startup
- Serial communication is initialized at 9600 baud for debugging purposes
- Small delays (1ms) are implemented between pin reads for stability
- OLED display address is set to 0x3C (default for most SSD1306 displays)
