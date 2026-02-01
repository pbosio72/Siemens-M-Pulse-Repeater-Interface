=============================================================================
MECHANICAL MEMORY SYSTEM WITH KEYPAD INPUT - README
=============================================================================

Sketch Name: 7_write_read_display_keypad_Arduino_UNO.ino
Version: 1.0
Date: 2026-02-01
Description: Complete mechanical memory system with 3x4 keypad input via MCP23017

=============================================================================
OVERVIEW
=============================================================================

This sketch implements a complete mechanical memory system with 3x4 matrix keypad
input. Numbers (0-9) are entered via the keypad, converted to 4-bit BCD sequences,
and stored mechanically on a rotating disk. The system uses an MCP23017 I2C expander
to handle the keypad matrix, freeing up Arduino pins for other components.

Key Features:
- 3x4 matrix keypad input via MCP23017 I2C expander
- Real-time digit entry and immediate writing to mechanical memory
- Number to binary (BCD) conversion with 4 bits per digit
- Real-time read and decode display on LCD2004
- Automatic system initialization and head positioning
- Write protection through controlled workflow
- Professional LCD display with status messages
- Visual effects with sliding text and progress indicators

Key Differences from Previous Versions:
- Replaces push buttons with 3x4 matrix keypad
- Uses MCP23017 I2C expander for keypad interfacing
- Implements custom keypad scanning function
- Real-time digit display during read operation
- Enhanced user interface with clear status messages
- Improved timing constants for better user experience
- Removed Serial Monitor dependency (optional debug only)

=============================================================================
POWER SUPPLY AND SAFETY WARNING
=============================================================================

CRITICAL SAFETY INFORMATION - READ BEFORE BUILDING

This system requires TWO separate power supplies:

1. LOW VOLTAGE SECTION (5V DC):
   - Arduino board
   - LCD2004 display
   - MCP23017 I2C expander
   - Digital signal lines
   - Control circuitry

2. HIGH VOLTAGE SECTION (48V DC):
   - Relay coils
   - Mechanical memory actuation system
   - Motor drives

ELECTRICAL ISOLATION:
The 5V and 48V sections MUST be electrically isolated:
- Relays provide optical or magnetic isolation between sections
- Arduino controls relay coils (low voltage side)
- Relay contacts switch high voltage power
- NEVER connect 48V directly to Arduino pins
- Use optocouplers or relay modules with built-in isolation

WIRING REQUIREMENTS:
- Use appropriate wire gauge for 48V current levels
- Ensure proper insulation on all 48V connections
- Keep 48V wiring separate from 5V signal wiring
- Use terminal blocks or connectors rated for 48V
- Implement proper grounding for safety

** DANGER - HIGH VOLTAGE WARNING **
=============================================================================
48V DC CAN BE LETHAL UNDER CERTAIN CONDITIONS

- 48V DC can cause electrical shock and serious injury
- Risk increases with wet conditions or broken skin
- Can cause burns, muscle contractions, and cardiac effects
- Always disconnect power before handling circuits
- Use insulated tools when working with live circuits
- Never work on circuits while powered
- Ensure all connections are properly insulated
- Keep work area dry

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

Consult a qualified electrician or engineer if you have any doubts
about electrical safety or proper construction techniques.
=============================================================================

RECOMMENDED SAFETY PRACTICES:

1. ALWAYS disconnect power before touching any part of the circuit
2. Use a multimeter to verify circuits are de-energized
3. Wait for capacitors to discharge before handling
4. Implement emergency stop switch (easily accessible)
5. Use proper enclosure to prevent accidental contact
6. Label all high voltage sections clearly
7. Use appropriate personal protective equipment (PPE)
8. Never bypass safety interlocks or fuses
9. Keep flammable materials away from electrical components
10. Have fire extinguisher suitable for electrical fires nearby

COMPONENT RATINGS:
- Verify current ratings exceed maximum load
- Use fuses or circuit breakers for overcurrent protection
- Select wire gauge appropriate for current and voltage
- Use connectors and terminals rated for the application

TESTING PROCEDURE:
1. Inspect all connections before applying power
2. Test 5V section first (Arduino and logic circuits)
3. With 48V disconnected, verify relay operation (clicking sound)
4. Connect 48V with load disconnected
5. Measure voltages at all test points
6. Connect load only after verifying all safety checks
7. Monitor system during initial operation

LOCAL REGULATIONS:
- Check local electrical codes and regulations
- Some jurisdictions may require certified installation
- Building permits may be required
- Industrial safety standards may apply (OSHA, IEC, etc.)
- Comply with all applicable laws and standards

=============================================================================
HARDWARE REQUIREMENTS
=============================================================================

Components:
- Arduino Uno (or compatible)
- LCD2004 Display (20x4 with I2C module, address 0x27)
- MCP23017 I2C expander (address 0x20)
- 3x4 Matrix Keypad
- 3 Relays (for mechanical memory actuation)
- 2 Position Switches (CCW limit and read head sensor)
- Mechanical memory disk and head assembly

Pin Configuration:

ARDUINO:
  Digital Pin 9  → Read Head Switch
  Digital Pin 10 → Relay 1 (Write bit 1 mechanism)
  Digital Pin 11 → Relay 2 (Head rotation control)
  Digital Pin 12 → Relay 3 (Disk rotation control)
  Arduino UNO
    Analog Pin A4  → I2C SDA (LCD + MCP23017)
    Analog Pin A5  → I2C SCL (LCD + MCP23017)
  Arduino MEGA 2560
    Digital Pin 20  → I2C SDA (LCD + MCP23017)
    Digital Pin 21  → I2C SCL (LCD + MCP23017)
  Arduino M0
    Dedicated Pin

I2C DEVICES:
  LCD2004:   Address 0x27, connected to SDA/SCL
  MCP23017:  Address 0x20, connected to SDA/SCL

3X4 KEYPAD → MCP23017:
  Physical Pin 1 → PA0 (Column 2: keys 2, 5, 8, 0)
  Physical Pin 2 → PA1 (Row 1: keys 1, 2, 3)
  Physical Pin 3 → PA2 (Column 1: keys 1, 4, 7, *)
  Physical Pin 4 → PA3 (Row 4: keys *, 0, #)
  Physical Pin 5 → PA4 (Column 3: keys 3, 6, 9, #)
  Physical Pin 6 → PA5 (Row 3: keys 7, 8, 9)
  Physical Pin 7 → PA6 (Row 2: keys 4, 5, 6)

KEYPAD MATRIX:
       C1(PA2)  C2(PA0)  C3(PA4)
R1(PA1)   1        2        3
R2(PA6)   4        5        6
R3(PA5)   7        8        9
R4(PA3)   *        0        #

NOTE: The keypad pin mapping was determined through physical testing with
      a multimeter in continuity mode. Your keypad may have different wiring.

=============================================================================
SOFTWARE REQUIREMENTS
=============================================================================

Arduino Libraries (install via Library Manager):
- Wire.h (built-in)
- LiquidCrystal_I2C by Frank de Brabander
- Adafruit MCP23XXX by Adafruit (includes MCP23017 support)

Arduino IDE: 1.8.x or 2.x

Installation Steps:
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "LiquidCrystal I2C" and install (by Frank de Brabander)
4. Search for "Adafruit MCP23XXX" and install (by Adafruit)
5. Wire.h is included automatically (built-in library)

=============================================================================
HOW IT WORKS
=============================================================================

NUMBER ENCODING (BCD - Binary-Coded Decimal)
Each decimal digit (0-9) is converted to 4 bits:
  0 → 0000    5 → 0101
  1 → 0001    6 → 0110
  2 → 0010    7 → 0111
  3 → 0011    8 → 1000
  4 → 0100    9 → 1001

Example: User enters "102"
  1 → 0001
  0 → 0000
  2 → 0010
  Result stored in memory: 000100000010 (12 bits total)

KEYPAD SCANNING MECHANISM:
The system uses matrix scanning to detect keypresses:
1. Each row is activated (set LOW) one at a time
2. All columns are read simultaneously
3. If a column reads LOW while a row is active, that key is pressed
4. Key position is determined by row-column intersection
5. Debouncing prevents multiple triggers from single press

WRITE PROCESS:
1. System auto-initializes and positions head at CCW (counter-clockwise) block
2. User presses numeric keys (0-9) on keypad
3. LCD displays "Input Sequence" with digits as they are entered
4. Each digit is IMMEDIATELY converted to 4 bits and written to memory
   - Press "1" → converts to 0001 → writes 4 bits → done
   - Press "0" → converts to 0000 → writes 4 bits → done
   - Press "2" → converts to 0010 → writes 4 bits → done
5. Display shows progress: "X/15" (current digit / maximum)
6. Maximum 15 digits can be entered (60 bits total)
7. User can press * at any time to reset and start over

READ PROCESS:
1. User presses # key to initiate read
2. System performs finalization moves:
   - Extra disk rotation to complete last write
   - Head moves back one position for alignment
3. LCD displays "Read Data" with "-> " prompt
4. System reads bits from memory sequentially
5. Every 4 bits are decoded to a decimal digit
6. Decoded digit appears on LCD IMMEDIATELY (real-time display)
7. Digits fill the display left-to-right, line by line
8. Display shows "Done" when read completes
9. System returns to input mode

REAL-TIME DECODE DISPLAY:
The read operation displays decoded numbers as they are read:
  Line 0: "     Read Data"
  Line 2: "-> 0123456789012"  (digits appear one by one)
  Line 3: "                Done"

Maximum 17 digits can be displayed on line 2 (20 chars - "-> " = 17)
If more digits exist, they wrap to line 3.

RESET FUNCTION (* key):
1. Displays "Reset System" message
2. Repositions head to CCW block (same as startup)
3. Clears all entered digits
4. Resets digit counter
5. Displays welcome screen
6. Returns to input mode

MECHANICAL SYNCHRONIZATION:
After the last digit is entered and before reading:
  activateRele(rele_3, sleep_time_rotate);  // Extra disk rotation
  activateRele(rele_2, sleep_time_read);    // Head back one position
This compensates for positioning offset during write operations and
ensures the read head is aligned properly with the first stored bit.

=============================================================================
USAGE INSTRUCTIONS
=============================================================================

STEP 1: Power Up & Initialization
- Connect Arduino to power
- LCD displays "System starting..."
- System automatically positions head to CCW block
- LCD shows "Reset System" during positioning
- LCD displays "Siemens Memory Ready" when complete
- After 3 seconds, shows:
    Line 0: "   Input Sequence"
    Line 2: "Enter digits 0-9"
    Line 3: " *->Reset    #->Read"
- System is now ready for input

STEP 2: Enter Numbers (Keys 0-9)
- Press any digit key (0-9) on keypad
- LCD updates to show:
    Line 0: "   Input Sequence"
    Line 1: "-> [your digits]"
    Line 3: "Writing Memory..."
- Digit is immediately written to memory (4 bits)
- Progress indicator appears: "X/15"
- Continue entering digits (maximum 15)
- Each digit is written immediately - no need to press "enter"

Example sequence for entering "1234":
  Press 1 → LCD shows "-> 1" → writes 0001 to memory → shows "1/15"
  Press 2 → LCD shows "-> 12" → writes 0010 to memory → shows "2/15"
  Press 3 → LCD shows "-> 123" → writes 0011 to memory → shows "3/15"
  Press 4 → LCD shows "-> 1234" → writes 0100 to memory → shows "4/15"

STEP 3: Reset System (Key *)
- Press * at any time to reset
- LCD displays "Reset System"
- Head returns to CCW position
- All entered digits are cleared
- System returns to input mode
- Use this to start over with a new number

STEP 4: Read Data (Key #)
- After entering digits, press # to read
- LCD clears and shows "Read Data"
- System performs alignment moves (disk forward, head back)
- Digits appear one by one on Line 2: "-> 0123456789"
- Each group of 4 bits is decoded and displayed immediately
- Watch the digits appear in real-time as memory is read
- After read completes, "Done" appears on Line 3
- Display shows result for 5 seconds
- System automatically returns to input mode for next sequence

STEP 5: Decode Output (Automatic)
The system automatically decodes 4-bit sequences to decimal:
  Memory: 0001 0000 0010
  Display: "-> 102"
No manual decoding needed - digits appear correctly on LCD.

=============================================================================
LCD DISPLAY MESSAGES
=============================================================================

Startup & Initialization:
"System starting..."       - Arduino boot sequence
"Reset System"             - Positioning head to start
"Siemens Memory Ready"     - System initialized (sliding text effect)
"Enter digits 0-9"         - Instructions
" *->Reset    #->Read"     - Key functions

Input Mode:
"   Input Sequence"        - Main header during input
"-> [digits]"              - Shows entered digits (line 1)
"Writing Memory..."        - Brief status during write
"X/15"                     - Progress indicator (X = current, 15 = max)
"Max digits reached!"      - Cannot enter more than 15 digits

Reset Operation:
"Reset System"             - System is resetting
(then returns to startup sequence)

Read Operation:
"     Read Data"           - Read operation header
"-> [decoded digits]"      - Decoded numbers appear here
"                Done"     - Read operation complete

Error Messages:
"No data available!"       - Attempted read with disk at end position
"No data to read!"         - Pressed # without entering any digits
"MCP23017 ERROR!"          - MCP23017 not detected (halts execution)

=============================================================================
CONFIGURATION PARAMETERS
=============================================================================

Timing Constants (in milliseconds):
  sleep_time_rotate = 100;  // Disk rotation timing
  sleep_time_write = 100;   // Write operation timing
  sleep_time_read = 100;    // Read operation timing
  delay_small = 200;        // Text display sliding effect
  delay_medium = 2000;      // Status message duration
  delay_long = 3000;        // Important message duration
  delay_huge = 5000;        // Final result display duration

Tuning the Timing Constants:
- sleep_time_* values control mechanical relay actuation
  - Increase if bits are unreliable or mechanisms are slow
  - Decrease for faster operation (after confirming reliability)
  - Typical range: 50-200ms depending on hardware
- delay_* values control LCD message display duration
  - Adjust for preferred user interface speed
  - delay_small affects sliding text speed (character-by-character)
  - delay_huge affects how long final results are shown

Memory Configuration:
  const int max_numbers = 15;  // Maximum digits (15 × 4 = 60 bits)

I2C Addresses:
  LCD2004:  0x27 (or 0x3F for some modules)
  MCP23017: 0x20 (default, configurable with A0/A1/A2 pins)
Note: use the sketch "Utility_01_LCD_address.ino" to detect the correct addresses if needed

To change LCD address (if 0x27 doesn't work):
  LiquidCrystal_I2C lcd(0x3F, 20, 4);

To change MCP23017 address:
  if (!mcp.begin_I2C(0x21)) {  // Change 0x20 to desired address

Keypad Pin Mapping:
  byte rowPins[ROWS] = {1, 6, 5, 3};  // PA1, PA6, PA5, PA3
  byte colPins[COLS] = {2, 0, 4};     // PA2, PA0, PA4

NOTE: These pin assignments are based on physical testing of a specific
      keypad. Different keypad models may have different internal wiring.
      Use a multimeter to verify your keypad's pin configuration.

=============================================================================
TECHNICAL DETAILS
=============================================================================

BINARY CONVERSION ALGORITHM:
The digitToBits() function extracts 4 bits using bitwise operations:
  bits[0] = (num >> 3) & 1;  // Bit 3 (MSB) - weight 8
  bits[1] = (num >> 2) & 1;  // Bit 2 - weight 4
  bits[2] = (num >> 1) & 1;  // Bit 1 - weight 2
  bits[3] = num & 1;         // Bit 0 (LSB) - weight 1

Example for digit 5 (binary 0101):
  5 >> 3 = 0, masked = 0  →  bits[0] = 0
  5 >> 2 = 1, masked = 1  →  bits[1] = 1
  5 >> 1 = 2, masked = 0  →  bits[2] = 0
  5 >> 0 = 5, masked = 1  →  bits[3] = 1
Result: [0, 1, 0, 1] which is 0101 in binary = 5 in decimal

DECODE ALGORITHM (Read Operation):
The readAndDecodeMemory() function converts 4 bits back to decimal:
  for (int j = 0; j < 4; j++) {
    if (fourBits.charAt(j) == '1') {
      decimalValue += (1 << (3 - j));
    }
  }

Example decoding "0101":
  j=0: '0' → skip
  j=1: '1' → add (1 << 2) = 4 → decimalValue = 4
  j=2: '0' → skip
  j=3: '1' → add (1 << 0) = 1 → decimalValue = 5
Result: 5

KEYPAD MATRIX SCANNING:
The scanKeypad() function implements standard matrix scanning:
1. Set all rows HIGH (inactive)
2. For each row:
   a. Set current row LOW (active)
   b. Read all columns
   c. If any column is LOW, key at (row, column) is pressed
   d. Return corresponding character from keys[][] array
3. If no key pressed, return 0

Debouncing is implemented by:
- Waiting for key release before returning
- Adding 50ms delay after release
- 5ms stabilization delay between row changes

READ LOGIC INVERSION:
The read head switch uses inverted logic:
  char bit = (!switchState_read) ? '1' : '0';
This is because the mechanical sensor reads:
  HIGH → bit 0
  LOW  → bit 1
So we invert in software to get correct bit values.

WRITE COMPENSATION:
After entering all digits and before reading:
  activateRele(rele_3, sleep_time_rotate);  // Extra disk rotation
  activateRele(rele_2, sleep_time_read);    // Head back one position

This two-step compensation ensures:
1. The last written bit is properly positioned
2. The read head aligns with the first bit for accurate reading

IMMEDIATE WRITE BEHAVIOR:
Each digit is written as soon as its key is pressed:
  writeDigitToMemory(key);  // Called immediately in loop()
This means:
- Data is never lost (written instantly, not buffered)
- User gets immediate feedback via relay sounds
- No need for a separate "write" or "enter" button
- System state is always synchronized with display

=============================================================================
TROUBLESHOOTING
=============================================================================

Problem: "MCP23017 ERROR!" on LCD at startup
Solution:
  - Check MCP23017 power connections (VCC to 5V, GND to GND)
  - Verify I2C connections (SDA to A4, SCL to A5)
  - Check I2C address (default 0x20)
  - Run I2C scanner sketch to detect MCP23017
  - Verify MCP23017 is properly seated if using breadboard
  - Check for solder bridges if using custom PCB

Problem: LCD shows text but no keypad response
Solution:
  - Verify keypad is connected to correct MCP23017 pins
  - Check rowPins[] and colPins[] arrays match your keypad
  - Use multimeter to test keypad pin mapping
  - Open Serial Monitor and add debug prints to scanKeypad()
  - Verify MCP23017 pin modes are set correctly
  - Test individual MCP23017 pins with simple digitalWrite/digitalRead

Problem: Keypad registers wrong keys
Solution:
  - Your keypad has different internal wiring
  - Use multimeter in continuity mode to map pins
  - Update rowPins[] and colPins[] arrays accordingly
  - Update keys[][] array if button layout differs
  - Refer to "Keypad Pin Mapping" section in header

Problem: Keypad registers multiple keys per press
Solution:
  - Increase debounce delay in scanKeypad()
  - Check for electrical noise on connections
  - Add small capacitors (0.1µF) across keypad pins
  - Ensure pull-up resistors are enabled on column pins
  - Reduce scan speed (increase delay in scanKeypad loop)

Problem: LCD backlight on but no text
Solution:
  - Adjust contrast potentiometer on I2C module
  - Use small screwdriver on blue component on back
  - Turn slowly until text appears
  - Try changing I2C address to 0x3F

Problem: System doesn't reset properly
Solution:
  - Check Switch C (CCW limit sensor) is working
  - Verify switch is normally-open type
  - Check wiring and connections
  - Add debug prints to see switch state
  - Manually position head and restart
  - Increase sleep_time_rotate if mechanism is slow

Problem: Incorrect bits read from memory
Solution:
  - Increase sleep_time_read and sleep_time_rotate
  - Verify read head switch alignment
  - Check inverted logic is correct (!switchState_read)
  - Ensure smooth mechanical movement
  - Add delays between operations
  - Verify write compensation moves are executing

Problem: Not all digits displayed during read
Solution:
  - LCD can show ~17 digits on line 2 (20 chars - "-> ")
  - If entering 15 digits, some will wrap to line 3
  - This is normal behavior
  - Reduce number of input digits if full visibility needed
  - Check Serial Monitor for complete sequence

Problem: "Max digits reached!" appears
Solution:
  - This is normal - 15 digit limit has been reached
  - Press * to reset and start new sequence
  - Or press # to read current sequence
  - Adjust max_numbers constant if more digits needed
  - Consider mechanical memory capacity before increasing

Problem: "No data to read!" message
Solution:
  - You pressed # without entering any digits first
  - Enter digits 0-9 before pressing #
  - System prevents reading empty memory

=============================================================================
SERIAL MONITOR (OPTIONAL DEBUG)
=============================================================================

Baud Rate: 9600

The sketch includes minimal Serial debug output:
- "ERROR: MCP23017 not found!" if MCP23017 initialization fails

Serial Monitor is not required for normal operation but can be useful
for debugging. To add more debug output, uncomment Serial.print() statements
or add new ones as needed.

Example debug additions:
  Serial.print("Key pressed: ");
  Serial.println(key);
  
  Serial.print("Digit written: ");
  Serial.print(digit);
  Serial.print(" as binary: ");
  Serial.println(binary_representation);

=============================================================================
CODE STRUCTURE
=============================================================================

Main Functions:

Keypad Functions:
- scanKeypad()         : Scan 3x4 matrix and return pressed key

Memory Functions:
- activateRele()       : Pulse relay on/off with timing
- digitToBits()        : Convert decimal digit to 4-bit binary array
- writeSingleBit()     : Write one bit to mechanical memory
- writeDigitToMemory() : Convert digit to bits and write all 4

Display Functions:
- printSlow()          : Display text character-by-character (effect)

System Functions:
- resetSystem()        : Clear sequence and reposition head to CCW
- readAndDecodeMemory(): Read bits and decode to decimal in real-time

Setup Section:
- Initialize all pins (Arduino and MCP23017)
- Initialize LCD2004 display
- Initialize MCP23017 I2C expander
- Configure keypad matrix (rows as outputs, columns as inputs)
- Perform initial system reset and head positioning

Loop Section:
- Continuously scan keypad for input
- Handle numeric keys (0-9): add to sequence and write to memory
- Handle * key: reset system
- Handle # key: finalize write and read/decode memory
- Update LCD display with current status
- Implement digit counter and maximum limit

Key Variables:
- inputSequence: String storing entered decimal digits
- digitCount: Counter for number of digits entered
- fourBits: Buffer for accumulating 4 bits during read

=============================================================================
NOTES
=============================================================================

- Maximum 15 decimal digits can be stored (60 bits total)
- LCD2004 can display approximately 17 digits per line
- Only digits 0-9 are supported (alphabetic characters not implemented)
- Each digit is written immediately upon entry (no buffering)
- Keypad pin mapping is specific to tested hardware - verify with multimeter
- System automatically handles write/read synchronization
- MCP23017 shares I2C bus with LCD - different addresses prevent conflicts
- Mechanical timing may require adjustment for different hardware
- Both LCD and Serial Monitor show operation status
- Real-time decode provides instant feedback on stored data

=============================================================================
SUPPORT & ADDITIONAL INFORMATION
=============================================================================

For questions, issues, or improvements:
- Review main project README.md in repository root
- Check wiring diagram in /docs folder
- Test MCP23017 separately with simple I2C scanner
- Verify keypad pin mapping with multimeter
- Test mechanical system independently
- Use Serial Monitor for debugging
- Adjust timing constants for your specific hardware

Keypad Pin Testing Procedure:
1. Set multimeter to continuity mode (beep)
2. Place one probe on keypad pin
3. Place other probe on another keypad pin
4. Press each key systematically
5. Note which key makes the beep for each pin pair
6. Build a matrix showing row/column relationships
7. Update rowPins[] and colPins[] arrays accordingly

I2C Device Testing:
1. Upload I2C scanner sketch to Arduino
2. Open Serial Monitor at 9600 baud
3. Should detect devices at 0x20 (MCP23017) and 0x27 (LCD)
4. If addresses differ, update code accordingly

=============================================================================
