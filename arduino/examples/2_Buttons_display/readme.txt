Button actuation and mini-oled display visual

**File:** `2_Buttons_display.ino`

**Purpose:** This sketch provides direct button-to-relay control while continuously monitoring and displaying the state of three input switches on an OLED display.

**Hardware Requirements:**
- 3 push buttons (pins 3, 4, 5)
- 3 switches for monitoring (pins 6, 7, 8)
- 3 relays (pins 10, 11, 12)
- OLED display (128x64, I2C address 0x3C)

**Functionality:**
- **Button 1:** Directly controls Relay 1 (ON when pressed, OFF when released)
- **Button 2:** Directly controls Relay 2 (ON when pressed, OFF when released)
- **Button 3:** Directly controls Relay 3 (ON when pressed, OFF when released)
- **Continuous monitoring:** Reads the state of switches A, B, and C
- **Display:** Shows real-time status of all three switches (ON/OFF)

**Display Output:**
```
Switch A: ON/OFF
Switch B: ON/OFF
Switch C: ON/OFF
```

**Timing Parameters:**
- Relay actuation time: 10ms
- Reading stability delay: 1ms

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
