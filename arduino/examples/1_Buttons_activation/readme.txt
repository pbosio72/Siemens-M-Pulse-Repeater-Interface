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
