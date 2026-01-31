=============================================================================
WRITE/READ SEQUENCE WITH LCD2004 - README
=============================================================================

Sketch Name: 6_write_read_with_display.ino
Version: 2.0
Date: 2026-01-10
Description: Complete number encoding and storage system with LCD2004 display

=============================================================================
OVERVIEW
=============================================================================

This sketch implements a complete write/read system for a mechanical memory
device. It converts decimal numbers (0-9) into 4-bit BCD sequences and stores
them mechanically on a rotating disk. The stored data can then be read back
and displayed in real-time on the LCD2004 display.

Key Features:
- Automatic initialization and head positioning
- Number to binary (BCD) conversion
- Sequential bit writing to mechanical memory
- Real-time bit display on LCD2004 during read operation
- Bit sequence reading with inverted logic
- Write protection flag to prevent accidental overwrites
- LCD2004 feedback (20 characters x 4 lines)

Key Difference from Previous Version:
- Uses LCD2004 (20x4) instead of OLED SSD1306
- Displays read bits in real-time on LCD as they are read
- Simplified display messages
- Bits appear character by character across 4 lines (80 bits max visible)

=============================================================================
HARDWARE REQUIREMENTS
=============================================================================

Components:
- Arduino Uno (or compatible)
- 3 Push Buttons (pins 3, 4, 5)
- 4 Position Switches (pins 6, 7, 8, 9)
- 3 Relays (pins 10, 11, 12)
- LCD2004 Display (20x4 with I2C module, address 0x27)
- Mechanical memory disk and head assembly

Pin Configuration:
  Button 1 (Write)     → Pin 3
  Button 2 (Unused)    → Pin 4
  Button 3 (Read)      → Pin 5
  Switch A             → Pin 6
  Switch B             → Pin 7
  Switch C (CCW limit) → Pin 8
  Read Head Switch     → Pin 9
  Relay 1 (Write bit1) → Pin 10
  Relay 2 (Head move)  → Pin 11
  Relay 3 (Disk move)  → Pin 12
  I2C SDA              → A4
  I2C SCL              → A5

LCD2004 I2C Module:
  VCC → 5V
  GND → GND
  SDA → A4
  SCL → A5

Note: LCD shares I2C bus with future MCP23017 expander (different addresses)

=============================================================================
SOFTWARE REQUIREMENTS
=============================================================================

Arduino Libraries (install via Library Manager):
- Wire.h (built-in)
- LiquidCrystal_I2C.h (by Frank de Brabander)

Arduino IDE: 1.8.x or 2.x

Installation:
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "LiquidCrystal I2C"
4. Install "LiquidCrystal I2C" by Frank de Brabander

=============================================================================
HOW IT WORKS
=============================================================================

NUMBER ENCODING (BCD - Binary-Coded Decimal)
Each decimal digit is converted to 4 bits:
  0 → 0000    5 → 0101
  1 → 0001    6 → 0110
  2 → 0010    7 → 0111
  3 → 0011    8 → 1000
  4 → 0100    9 → 1001

Example: "102" becomes:
  1 → 0001
  0 → 0000
  2 → 0010
  Result: 000100000010 (12 bits total)

WRITE PROCESS:
1. System auto-initializes and positions head at CCW block
2. User presses Button 1
3. LCD displays "Writing data..."
4. Each digit in myNumber is converted to 4 bits
5. Each bit is written sequentially:
   - Bit 1: Relay 1 ON + Relay 3 pulse (write + rotate)
   - Bit 0: Relay 3 pulse only (rotate)
6. Final compensation: disk advances, head moves back
7. LCD displays "Write complete!"
8. flag_write is set to prevent overwrite

READ PROCESS:
1. User presses Button 3
2. LCD clears and prepares to display bits
3. Head/disk rotates (Relay 2) and reads each bit
4. Each bit is displayed IMMEDIATELY on LCD as it's read
5. Bits fill the display left-to-right, top-to-bottom (4 lines x 20 chars)
6. Complete sequence is also printed to Serial Monitor
7. After 5 seconds, LCD shows ready message
8. flag_write is reset to allow new writes

REAL-TIME DISPLAY:
The bits appear on LCD in this pattern:
  Line 0: [20 bits]
  Line 1: [20 bits]
  Line 2: [20 bits]
  Line 3: [20 bits]
Total: 80 bits can be displayed (15 digits = 60 bits fits perfectly)

MECHANICAL SYNCHRONIZATION:
After writing, the system performs:
- activateRele(rele_3) → Extra disk rotation
- activateRele(rele_2) → Head moves back one position
This compensates for positioning offset during write operations.

=============================================================================
USAGE INSTRUCTIONS
=============================================================================

STEP 1: Configure Number to Store
Edit this line in the sketch:
  String myNumber = "0123456789012";  // Max 15 digits (0-9)

STEP 2: Upload & Initialize
- Upload sketch to Arduino
- LCD shows "System starting..."
- System auto-positions head
- LCD displays "Positioning head..."
- Wait for "Ready!" message with button instructions

STEP 3: Write Data
- Press Button 1
- LCD shows "Writing data..."
- Relays activate and write sequence to memory
- LCD shows "Write complete!"
- System locks to prevent accidental overwrite
- LCD prompts "Press Btn3 to read data"

STEP 4: Read Data & View on LCD
- Press Button 3
- LCD clears and begins displaying bits in real-time
- Watch bits appear character by character as memory is read
- Bits fill line by line (20 chars per line, 4 lines total)
- Complete sequence also appears in Serial Monitor (9600 baud)
- After 5 seconds, LCD returns to ready state
- System unlocks for new write operation

STEP 5: Decode Output (if needed)
Group bits in sets of 4 and convert to decimal:
  0001 0000 0010
    ↓    ↓    ↓
    1    0    2
  Result: "102"

=============================================================================
LCD DISPLAY MESSAGES
=============================================================================

Startup Messages:
"System starting..."       - Initial boot
"Positioning head..."      - Auto-positioning to CCW block
"Ready!"                   - System ready
"Btn1: Write"              - Button 1 function
"Btn3: Read & Display"     - Button 3 function

Write Operation:
"Writing data..."          - Currently writing to memory
"Write complete!"          - Write operation successful
"Press Btn3"               - Prompt to read data
"to read data"             

Write Protection:
"Write blocked!"           - Attempted write while locked
"Read data first!"         - Instructions to unlock

Read Operation:
[Real-time bit display]    - Bits appear as 1s and 0s
(No text message during read - only bits)

After Read:
"Ready!"                   - System ready again
"Btn1: Write"              - Can write new data
"Btn3: Read & Display"     - Can read again

Error Messages:
"No data available!"       - Disk is already at end position

=============================================================================
CONFIGURATION PARAMETERS
=============================================================================

Timing Constants (in milliseconds):
  sleep_time_rotate = 100;  // Disk rotation timing
  sleep_time_write = 100;   // Write operation timing
  sleep_time_read = 100;    // Read operation timing

Adjust these values based on your mechanical system:
- Increase if bits are unreliable
- Decrease for faster operation
- Typical range: 50-200ms

Memory Limits:
  const int max_numbers = 15;  // Maximum digits (15 × 4 = 60 bits)

Write Protection:
  int flag_write = 0;  // 0=write allowed, 1=blocked

LCD Display Settings:
  LiquidCrystal_I2C lcd(0x27, 20, 4);
  - I2C Address: 0x27 (or 0x3F for some modules)
  - Columns: 20
  - Rows: 4

To Change LCD I2C Address:
If display doesn't work, try changing to 0x3F:
  LiquidCrystal_I2C lcd(0x3F, 20, 4);

To Find Your LCD Address:
Use an I2C scanner sketch to detect the correct address.

Display Timing:
  delay(5000);  // Show read result for 5 seconds
Adjust this value to show results longer or shorter.

=============================================================================
TECHNICAL DETAILS
=============================================================================

BINARY CONVERSION ALGORITHM:
The digitToBits() function extracts 4 bits using bitwise operations:
  bits[0] = (num >> 3) & 1;  // Bit 3 (MSB)
  bits[1] = (num >> 2) & 1;  // Bit 2
  bits[2] = (num >> 1) & 1;  // Bit 1
  bits[3] = num & 1;         // Bit 0 (LSB)

READ LOGIC INVERSION:
The read head switch uses inverted logic:
  int bitValue = !switchState_read;
This is because the mechanical sensor reads:
  HIGH → bit 0
  LOW  → bit 1
So we invert in software.

REAL-TIME LCD DISPLAY ALGORITHM:
During read, bits are displayed character by character:
  int currentLine = 0;   // Start at line 0
  int currentPos = 0;    // Start at position 0
  
  For each bit read:
    lcd.setCursor(currentPos, currentLine);
    lcd.print(bitValue);
    currentPos++;
    if (currentPos >= 20) {  // End of line reached
      currentPos = 0;         // Reset to start
      currentLine++;          // Move to next line
    }

Maximum 80 bits can be displayed (4 lines × 20 chars).
With 15 digits maximum (60 bits), all data fits on display.

WRITE COMPENSATION:
At the end of writeNumberSequence():
  activateRele(rele_3, sleep_time_rotate);  // Extra disk rotation
  activateRele(rele_2, sleep_time_read);    // Head back one step
This ensures proper alignment for subsequent read operations.

=============================================================================
TROUBLESHOOTING
=============================================================================

Problem: LCD backlight on but no text visible
Solution:
  - Adjust contrast potentiometer on I2C module backpack (blue component)
  - Use small screwdriver to turn clockwise/counterclockwise
  - Text should become visible when properly adjusted

Problem: LCD shows no backlight or text
Solution:
  - Check I2C connections (SDA→A4, SCL→A5, VCC→5V, GND→GND)
  - Verify I2C address (try 0x3F if 0x27 doesn't work)
  - Run I2C scanner sketch to detect address
  - Check 5V power supply is adequate

Problem: Bits read incorrectly
Solution:
  - Increase timing values (sleep_time_read, sleep_time_rotate)
  - Verify switch_read sensor alignment
  - Check inverted logic is correct (!switchState_read)
  - Ensure smooth mechanical movement

Problem: System doesn't initialize
Solution:
  - Check Switch C is working (CCW limit)
  - Verify Relay 2 and 3 connections
  - Manually position head at CCW, restart
  - Monitor Serial output for errors

Problem: "Write blocked!" message appears
Solution:
  - This is normal after writing
  - Press Button 3 to read first
  - Reading resets flag_write

Problem: Bits appear garbled on LCD
Solution:
  - Check Serial Monitor output - does it look correct?
  - If Serial is correct but LCD is wrong, reduce read speed
  - Increase delay between bit reads
  - Check LCD refresh rate

Problem: Not all bits displayed on LCD
Solution:
  - LCD can show 80 bits maximum (4 lines × 20 chars)
  - Reduce myNumber length to 15 digits or less
  - Check bitSequence string length in Serial Monitor

=============================================================================
SERIAL MONITOR OUTPUT
=============================================================================

Baud Rate: 9600

Example session when writing/reading "102":

Starting...
LCD initialized
Positioning head...
Ready!
Writing sequence...
Write complete!
Reading data...
000100000010
Bit sequence: 000100000010

To decode:
  Split into 4-bit groups: 0001 | 0000 | 0010
  Convert to decimal: 1, 0, 2
  Result: "102"

=============================================================================
CODE STRUCTURE
=============================================================================

Main Functions:
- activateRele()       : Pulse a relay on/off
- digitToBits()        : Convert digit to 4-bit array
- writeSingleBit()     : Write one bit to memory
- writeNumberSequence(): Write entire number sequence
- displayBitSequence() : Display bits on LCD (not used in real-time version)

Setup Section:
- Initialize pins and relays
- Initialize LCD2004 display with backlight
- Auto-position head to CCW block
- Display ready message with instructions

Loop Section:
- Monitor Button 1: trigger write (if flag_write=0)
- Monitor Button 3: trigger read with real-time LCD display
- Implement write protection logic
- Display operation status on LCD

Real-Time Display Section (in read loop):
- Track current line and position on LCD
- Display each bit immediately as it's read
- Automatically wrap to next line after 20 characters
- Store complete sequence in bitSequence string

=============================================================================
VERSION HISTORY
=============================================================================

v2.0 (2026-01-10)
  - Replaced OLED SSD1306 with LCD2004
  - Implemented real-time bit display during read
  - Simplified display messages
  - Removed displayBitSequence() function (not needed)
  - Added Serial debug messages
  - Added LCD contrast troubleshooting notes

v1.0 (2026-01-10)
  - Initial working version with OLED
  - BCD encoding implementation
  - Write protection flag system
  - Mechanical compensation

=============================================================================
NOTES
=============================================================================

- Maximum 15 digits can be stored (60 bits total)
- LCD can display up to 80 bits (all 4 lines filled)
- Only digits 0-9 are supported (alphabetic characters not implemented)
- Write operations are blocked after first write until read is performed
- Read operation automatically unlocks system for new writes
- Mechanical timing may need adjustment for different hardware setups
- Both Serial Monitor and LCD show the read data
- LCD provides better visualization than Serial for bit sequences
- Contrast adjustment is critical for LCD visibility

=============================================================================
SUPPORT
=============================================================================

For issues, questions, or improvements:
- Check main project README.md in repository root
- Review wiring diagram in /docs folder
- Test with simpler sketches (simple_relay_control.ino) first
- Verify mechanical system operation independently
- Use Serial Monitor for debugging
- Adjust LCD contrast if text not visible

=============================================================================
