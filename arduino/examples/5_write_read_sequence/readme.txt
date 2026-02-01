=============================================================================
WRITE/READ SEQUENCE SKETCH - README
=============================================================================

Sketch Name: write_read_sequence.ino
Version: 1.0
Date: 2026-01-10
Description: Complete number encoding and storage system for mechanical memory

=============================================================================
OVERVIEW
=============================================================================

This sketch implements a complete write/read system for a mechanical memory
device. It converts decimal numbers (0-9) into 4-bit BCD sequences and stores
them mechanically on a rotating disk. The stored data can then be read back
and displayed via Serial Monitor.

Key Features:
- Automatic initialization and head positioning
- Number to binary (BCD) conversion
- Sequential bit writing to mechanical memory
- Bit sequence reading with inverted logic
- Write protection flag to prevent accidental overwrites
- Real-time OLED display feedback

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
- 3 Push Buttons (pins 3, 4, 5)
- 4 Position Switches (pins 6, 7, 8, 9)
- 3 Relays (pins 10, 11, 12)
- OLED Display SSD1306 128x64 I2C (SDA: A4, SCL: A5)
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

Note for IC2 connections
 Arduino UNO
   Analog Pin A4  → I2C SDA (LCD + MCP23017)
   Analog Pin A5  → I2C SCL (LCD + MCP23017)
 Arduino MEGA 2560
   Digital Pin 20  → I2C SDA (LCD + MCP23017)
   Digital Pin 21  → I2C SCL (LCD + MCP23017)
 Arduino M0
   Dedicated Pin
   
=============================================================================
SOFTWARE REQUIREMENTS
=============================================================================

Arduino Libraries (install via Library Manager):
- Wire.h (built-in)
- Adafruit_GFX.h
- Adafruit_SSD1306.h

Arduino IDE: 1.8.x or 2.x

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
3. Each digit in myNumber is converted to 4 bits
4. Each bit is written sequentially:
   - Bit 1: Relay 1 ON + Relay 3 pulse (write + rotate)
   - Bit 0: Relay 3 pulse only (rotate)
5. Final compensation: disk advances one step, head moves back one step (see MECHANICAL SYNCHRONIZATION detail)
6. flag_write is set to prevent overwrite

MECHANICAL SYNCHRONIZATION:
After writing, the system performs:
- activateRele(rele_3) → Extra disk rotation
- activateRele(rele_2) → Head moves back one position
This compensates for positioning offset during write operations.


READ PROCESS:
1. User presses Button 3
2. Head/disk rotates (Relay 2) and reads each bit
3. Bit values are read from switch_read (inverted logic)
4. Complete sequence is printed to Serial Monitor
5. flag_write is reset to allow new writes



=============================================================================
USAGE INSTRUCTIONS
=============================================================================

STEP 1: Configure Number to Store
Edit this line in the sketch:
  String myNumber = "0123456789012";  // Max 15 digits (0-9)

STEP 2: Upload & Initialize
- Upload sketch to Arduino
- Display shows "Ready!" then auto-initializes
- Wait for "Ready to write!" message

STEP 3: Write Data
- Press Button 1
- Display shows "Writing sequence..."
- Wait for "Write complete!"
- System locks (prevents accidental overwrite)

STEP 4: Read Data
- Press Button 3
- Display shows "Reading data..."
- Open Serial Monitor (9600 baud)
- Bit sequence appears (e.g., "000100000010")
- Display shows "Read complete!"
- System unlocks for new write

STEP 5: Decode Output
Group bits in sets of 4 and convert to decimal:
  0001 0000 0010
    ↓    ↓    ↓
    1    0    2
  Result: "102"

=============================================================================
DISPLAY MESSAGES
=============================================================================

"Initial SetUp..."         - Positioning head to start position
"Ready to write!"          - System ready for write operation
"Writing sequence..."      - Currently writing bits to memory
"Write complete!"          - Write operation successful
"Write blocked! Read first!" - Attempted write while locked
"Reading data..."          - Currently reading from memory
"Read complete!"           - Read operation successful
"No data available!"       - Disk is at end position

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
  Serial.print(!switchState_read);
This is because the mechanical sensor reads:
  HIGH → bit 0
  LOW  → bit 1
So we invert in software.

WRITE COMPENSATION:
At the end of writeNumberSequence():
  activateRele(rele_3, sleep_time_rotate);  // Extra disk rotation
  activateRele(rele_2, sleep_time_read);    // Head back one step
This ensures proper alignment for subsequent read operations.


=============================================================================
SERIAL MONITOR OUTPUT
=============================================================================

Baud Rate: 9600

Example output when reading "102":
  000100000010

To decode:
  Split into 4-bit groups: 0001 | 0000 | 0010
  Convert to decimal: 1, 0, 2
  Result: "102"

=============================================================================
CODE STRUCTURE
=============================================================================

Main Functions:
- activateRele()       : Pulse a relay on/off
- display_option()     : Show message on OLED
- digitToBits()        : Convert digit to 4-bit array
- writeSingleBit()     : Write one bit to memory
- writeNumberSequence(): Write entire number sequence

Setup Section:
- Initialize pins and relays
- Initialize OLED display
- Auto-position head to CCW block
- Display ready message

Loop Section:
- Monitor Button 1: trigger write (if flag_write=0)
- Monitor Button 3: trigger read and reset flag
- Implement write protection logic
- Display operation status

=============================================================================
FUTURE IMPROVEMENTS FOR THIS SKETCH
=============================================================================

Planned enhancements:
- Replace myNumber hardcoded value with keypad input
- Add error detection (parity bit or CRC)
- Display decoded numbers on OLED (not just Serial)

=============================================================================
NOTES
=============================================================================

- Maximum 15 digits can be stored (60 bits total)
- Only digits 0-9 are supported (alphabetic characters not implemented)
- Write operations are blocked after first write until read is performed
- Read operation automatically unlocks system for new writes
- Mechanical timing may need adjustment for different hardware setups
- Serial Monitor must be open at 9600 baud to see read output


