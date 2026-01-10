Buttons activation

**File:** 1_Buttons_activation.ino`

**Purpose:** This is the simplest sketch, providing basic button-to-relay mapping without any display or additional feedback mechanisms.

**Hardware Requirements:**
- 3 push buttons (pins 3, 4, 5)
- 3 relays (pins 10, 11, 12)

**Functionality:**
- **Button 1:** Directly controls Relay 1 (ON when pressed, OFF when released)
- **Button 2:** Directly controls Relay 2 (ON when pressed, OFF when released)
- **Button 3:** Directly controls Relay 3 (ON when pressed, OFF when released)

**Timing Parameters:**
- Relay actuation time: 10ms

**Use Case:** Ideal for basic testing or when no visual feedback is required. Suitable for initial hardware verification.


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
- No external libraries required

## Notes

- All sketches initialize relays to the OFF state (LOW) on startup
- Serial communication is initialized at 9600 baud for debugging purposes
- Small delays (1ms) are implemented between pin reads for stability
