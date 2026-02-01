# Siemens M Pulse Repeater Interface - Program Explanation

## General Overview

This Arduino program controls the **Siemens M pulse repeater**, an electromechanical memory device that stores bit sequences (0 and 1) on a rotating disk with thin lamellae. The program allows you to:
- **Write** bits to memory
- **Read** bits from memory
- **Display** status on an OLED display

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

## Hardware - Input/Output Pins

### **INPUT PINS - User Buttons**

| Pin | Name | Function |
|-----|------|----------|
| 3 | `button_1` | Button to **write bit 1** |
| 4 | `button_2` | Button to **write bit 0** |
| 5 | `button_3` | Button to **read stored data** |

### **INPUT PINS - Position Switches**

| Pin | Name | Function |
|-----|------|----------|
| 6 | `switch_A` | Switch A - Disk position detection |
| 7 | `switch_B` | Switch B - Disk position detection |
| 8 | `switch_C` | Switch C - **CCW block** detection (initial position) |
| 9 | `switch_read` | Reading head switch - Reads if lamella is in position 0 or 1 |

**Note:** Switches A, B, C detect when the disk reaches specific positions (likely mechanical blocks that prevent further rotation).

### **OUTPUT PINS - Control Relays**

| Pin | Name | Function |
|-----|------|----------|
| 10 | `rele_1` | Relay 1 - **Writes bit 1** (moves lamella to "front" position) |
| 11 | `rele_2` | Relay 2 - **CCW rotation** (counterclockwise) |
| 12 | `rele_3` | Relay 3 - **CW rotation** (clockwise - forward movement) |

---

## Program Logic

### **SETUP - Initialization (executed once at startup)**
```
1. Initialize serial communication (9600 baud)
2. Configure all pins (INPUT/OUTPUT)
3. Turn off all relays (safe state)
4. Initialize OLED display
5. IMPORTANT: Position the disk to initial position (home position)
```

**Home Position Procedure:**
```
a. Read switch_C
b. If switch_C = LOW (disk already at the end):
   - Move 2 steps forward (CW with rele_3)
   - Ensure not on the block
c. Rotate counterclockwise (CCW with rele_2) until switch_C = LOW
d. Now the disk is at initial position (CCW block)
```

---

### **LOOP - Main Cycle (executed continuously)**

#### **1. End of Disk Check**
```cpp
Read: switch_A, switch_B, switch_C
If switch_A = LOW:
  - Display "End of disk!"
  - Set eod_state = 1 (end of disk reached)
Otherwise:
  - eod_state = 0 (can still write)
```

**Why?** Prevents writing beyond the physical disk limit.

---

#### **2. Bit Writing (only if eod_state = 0)**

##### **Write Bit 1 (button_1 pressed):**
```
1. Activate rele_1 (move lamella to position "1")
2. Wait sleep_time_write (60ms)
3. Activate rele_3 (advance disk one position)
4. Deactivate rele_1
5. Display: "Write bit -> 1"
```

**Physical sequence:**
- Electromagnet 1 moves the lamella to the reading track side
- Disk advances one position
- Ready for next write

##### **Write Bit 0 (button_2 pressed):**
```
1. Activate rele_3 (advance disk one position)
2. Display: "Write bit -> 0"
```

**Physical sequence:**
- No lamella is moved (remains in "behind" position)
- Disk advances one position
- The position represents a "0"

---

#### **3. Data Reading (button_3 pressed)**
```
1. Check switch_C:
   - If LOW: display "No data available!!" (already at the end)
   
2. If switch_C = HIGH (there is data):
   WHILE switch_C = HIGH (haven't reached the end):
     a. Read switch_read (lamella in position 0 or 1?)
     b. Print value to serial
     c. Activate rele_2 (rotate disk CCW - back to start)
     d. Recheck switch_C
     e. Display: "Read data..."
```

**Physical sequence:**
- Disk rotates backward (CCW)
- Reading head touches each lamella
- If lamella "front" → switch_read = HIGH (bit 1)
- If lamella "behind" → switch_read = LOW (bit 0)
- Continue until reaching switch_C (initial position)

---

## Simplified Flow Diagram
```
[START]
   ↓
[Initialize Hardware]
   ↓
[Go to Initial Position (Home)]
   ↓
┌──[INFINITE LOOP]────────────────────┐
│                                     │
│ 1. Check End of Disk (switch_A)     │
│    ├─ End? → Block writing          │
│    └─ OK? → Allow writing           │
│                                     │
│ 2. Wait for User Input:             │
│    ├─ Button 1? → Write 1, Advance  │
│    ├─ Button 2? → Write 0, Advance  │
│    └─ Button 3? → Read Backward     │
│                                     │
└─────────────────────────────────────┘
```

---

## Timing Constants
```cpp
sleep_time_rotate = 60ms  // Disk rotation time
sleep_time_write = 60ms   // Lamella writing time
sleep_time_read = 60ms    // Reading time
```

**Important note from code:**
> "Previous tests showed that the disk actuation speed and reading speed must be separate because the disk cannot write Bit 1 at the same speed as disk rotation"

This means that writing bit 1 (lamella movement) is slower than simple rotation, so separate timings are needed.

---

## Usage Example

### **Scenario: Write "101" and read it back**

1. **Startup:** Disk goes to initial position (CCW block)

2. **Write "1":**
   - Press button_1
   - Lamella moved to position "1"
   - Disk advances → Position 1 stored

3. **Write "0":**
   - Press button_2
   - Lamella NOT moved (remains "0")
   - Disk advances → Position 2 stored

4. **Write "1":**
   - Press button_1
   - Lamella moved to position "1"
   - Disk advances → Position 3 stored

5. **Read:**
   - Press button_3
   - Disk rotates backward (CCW)
   - Reading head reads: 1, 0, 1
   - Serial displays: `1`, `0`, `1`

---

## OLED Display - Messages

| Option | Message | When it appears |
|--------|---------|-----------------|
| 1 | "Initial SetUp..." | During home initialization |
| 2 | "Write bit -> 1" | Writing bit 1 |
| 3 | "Write bit -> 0" | Writing bit 0 |
| 4 | "Read data..." | During reading |
| 5 | "End of disk!" | Disk full (switch_A active) |
| 6 | "No data available!!" | Read attempt on empty disk |

---

## Technical Notes

### **Why 3 relays are needed?**
- **Relay 1:** Electromagnet that physically moves the lamella
- **Relay 2:** Electromagnet that rotates disk CCW (reading/reset)
- **Relay 3:** Electromagnet that rotates disk CW (writing/advancement)

### **Why the delay(1)?**
Digital reading stabilization - prevents incorrect readings due to bouncing or electrical noise.

### **System limitations:**
- FIFO memory (First In, First Out)
- Capacity: 72 positions (lamellae on the disk)
- One write can be read only once (then returns to beginning)
- No selective deletion (only complete reset by returning to start)

---

## Memory Operation Principle

The Siemens pulse repeater is an ingenious electromechanical device:

**Storage mechanism:**
- A rotating metal disk divided into 72 thin lamellae
- Each lamella has two stable positions: "behind" or "in front of" an annular track
- These positions represent binary states: 0 and 1

**Three electromagnets control:**
1. **Writing (Relay 1):** Moves lamella from "behind" (0) to "in front" (1) position
2. **Advancement (Relay 3):** Rotates disk clockwise (forward) one position
3. **Reading/Reset (Relay 2):** Rotates disk counterclockwise (backward)

**Reading mechanism:**
- A reading contact runs along the annular track
- When disk rotates, the contact touches lamellae in "front" position (bit 1)
- Lamellae in "behind" position are not touched (bit 0)

**Example - Storing digit "4":**
- Set 4 consecutive lamellae to position "1"
- When read, 4 consecutive pulses are detected
- This represents the telephone digit "4"

---

## Historical Context

This device was used in Italian telephone exchanges (SIP) from 1957 to approximately 1975. It served as a temporary register to store telephone number digits in the form of consecutive pulse trains, allowing single repetition without redialing.

Similar electromechanical memories were used in early computers such as:
- IBM 602 (1946-48)
- IBM 421 tabulator (1950)

This demonstrator preserves and showcases this historical telecommunications technology for educational purposes.
