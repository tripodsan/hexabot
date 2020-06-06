/*
     ╭───╮        ╭───╮     ╭───╮        ╭───╮
     │LFT│        │LMN│╭───╮│RMN│        │RFT│         R F T
     ╰───╯        ╰───╯│HTL│╰───╯        ╰───╯         │ │ │
          ╲            ╰───╯            ╱              │ │ └─────▶ Part: Coxa / Femur / Tibia / Mandible
          ╭───╮        ╭───╮        ╭───╮              │ └───────▶ Pos:  Front / Middle / Rear
          │LFF│        │HPN│        │RFF│              └─────────▶ Side: Left / Right
          ╰───╯        ╰───╯        ╰───╯
              ╲        ╭───╮        ╱
              ┌╭───╮───┤HRT├───╭───╮
              ││LFC│ ▲ ╰───╯   │RFC│
              │╰───╯ |         ╰──┬╯                                                    Coxa  Femur
              │      |   ▲        │                                           ▲        Length Length
              Coxa DY|   │        │                                           │Z      ◀-----▶◀------▶
              │      |   │Y       │                                 ┌─────────┼─────────┐
 ╭───╮  ╭───╮ │╭───╮ ▼   │     ╭──┴╮  ╭───╮  ╭───╮    ╭───╮  ╭───╮  ╭───╮     │     ╭───╮  ╭───╮  ╭───╮
 │LMT├──┤LMF├─┼┤LMC│ ────0───▶ │RMC├──┤RMF├──┤RMT│    │LMT├──┤LMF├──┤LMC│ ────0───▶ │RMC├──┤RMF├──┤RMT│   ▲
 ╰───╯  ╰───╯ │╰───╯     │  X  ╰──┬╯  ╰───╯  ╰───╯    ╰─┬─╯  ╰───╯  ╰───╯     │  X  ╰───╯  ╰───╯  ╰─┬─╯   |Tibia
              │          │◀------▶│                     │           └─────────┼─────────┘           │     |Length
              │          │Coxa DX │                     │                     │                     │     |
              │                   │                     ●                                           ●     ▼
              │╭───╮           ╭──┴╮
              ││LRC│   ╭───╮   │RRC│
              └╰───╯───┤TPN├───╰───╯
              ╱        ╰───╯        ╲
          ╭───╮        ╭───╮        ╭───╮
          │LRF│        │TTL│        │RRF│              HTL: Head Tilt
          ╰───╯        ╰───╯        ╰───╯              HPN: Head Pan
          ╱                             ╲              HRT: Head Rotate
     ╭───╮                               ╭───╮
     │LRT│                               │RRT│         TPN: Tail Pan
     ╰───╯                               ╰───╯         TTL: Tail Tilt
*/

#define NUMSERVOSPERLEG uint8_t(3)
#define NUMSERVOSHEAD 5
#define NUMSERVOSTAIL 2
#define NUMSERVOS uint8_t(NUMSERVOSPERLEG * 6 + NUMSERVOSHEAD + NUMSERVOSTAIL)

#define cSSC_ADC_VOLT  'A' // Voltage ADC Pin
#define cSSC_ADC_GRIP  'B' // Grip Sensor ADC Pin

//--------------------------------------------------------------------
// Servo Allocation

#define cRRCoxaPin       2  // Rear Right legs Hip Horizontal
#define cRRFemurPin      1  // Rear Right legs Hip Vertical
#define cRRTibiaPin      0  // Rear Right legs Knee

#define cRMCoxaPin       5  // Middle Right legs Hip Horizontal
#define cRMFemurPin      6  // Middle Right legs Hip Vertical
#define cRMTibiaPin      4  // Middle Right legs Knee

#define cRFCoxaPin      13  // Front Right legs Hip Horizontal
#define cRFFemurPin     14  // Front Right legs Hip Vertical
#define cRFTibiaPin     15  // Front Right legs Knee

#define cLRCoxaPin      18  // Rear Left legs Hip Horizontal
#define cLRFemurPin     17  // Rear Left legs Hip Vertical
#define cLRTibiaPin     16  // Rear Left legs Knee

#define cLMCoxaPin      21  // Middle Left legs Hip Horizontal
#define cLMFemurPin     22  // Middle Left legs Hip Vertical
#define cLMTibiaPin     23  // Middle Left legs Knee

#define cLFCoxaPin      29  // Front Left legs Hip Horizontal
#define cLFFemurPin     30  // Front Left legs Hip Vertical
#define cLFTibiaPin     31  // Front Left legs Knee

#define cLMandPin       26  // Left Mandible
#define cRMandPin       27  // Right Mandible
#define cHeadPanPin     24  // Head Pan (horizontal)
#define cHeadTiltPin    25  // Head Tilt (vertical)
#define cHeadRotPin     11  // Head Rotate

#define cTailPanPin     10  // Tail Pan (horizontal)
#define cTailTiltPin     9  // Tail Tilt (vertical)

//--------------------------------------------------------------------
// leg numbers

#define  cRR      0
#define  cRM      1
#define  cRF      2
#define  cLR      3
#define  cLM      4
#define  cLF      5

//--------------------------------------------------------------------
// min/max angles

#define cRRCoxaMin1     -65.0f      //Mechanical limits of the Right Rear Leg
#define cRRCoxaMax1      65.0f
#define cRRFemurMin1    -60.0f
#define cRRFemurMax1     90.0f
#define cRRTibiaMin1    -65.0f
#define cRRTibiaMax1     90.0f

#define cRMCoxaMin1     -65.0f      //Mechanical limits of the Right Middle Leg
#define cRMCoxaMax1      65.0f
#define cRMFemurMin1    -60.0f
#define cRMFemurMax1     90.0f
#define cRMTibiaMin1    -53.0f
#define cRMTibiaMax1     90.0f

#define cRFCoxaMin1     -65.0f      //Mechanical limits of the Right Front Leg
#define cRFCoxaMax1      65.0f
#define cRFFemurMin1    -60.0f
#define cRFFemurMax1     90.0f
#define cRFTibiaMin1    -65.0f
#define cRFTibiaMax1     90.0f

#define cLRCoxaMin1     -65.0f      //Mechanical limits of the Left Rear Leg
#define cLRCoxaMax1      65.0f
#define cLRFemurMin1    -60.0f
#define cLRFemurMax1     90.0f
#define cLRTibiaMin1    -65.0f
#define cLRTibiaMax1     90.0f

#define cLMCoxaMin1     -65.0f      //Mechanical limits of the Left Middle Leg
#define cLMCoxaMax1      65.0f
#define cLMFemurMin1    -60.0f
#define cLMFemurMax1     90.0f
#define cLMTibiaMin1    -65.0f
#define cLMTibiaMax1     90.0f

#define cLFCoxaMin1     -65.0f      //Mechanical limits of the Left Front Leg
#define cLFCoxaMax1      65.0f
#define cLFFemurMin1    -60.0f
#define cLFFemurMax1     90.0f
#define cLFTibiaMin1    -65.0f
#define cLFTibiaMax1     90.0f

#define cMandibleMin1   -20.0f
#define cMandibleMax1    90.0f
#define wMandibleInc      4.0f  // mandible increment
#define wMandibleTime     1  // mandible update time
#define cHeadPanMin1    -38.0f
#define cHeadPanMax1     38.0f
#define cHeadTiltMin1   -37.0f
#define cHeadTiltMax1    47.0f
#define cHeadRotMin1    -90.0f
#define cHeadRotMax1     90.0f

#define cTailPanMin1    -50.0f
#define cTailPanMax1     50.0f
#define cTailTiltMin1   -50.0f
#define cTailTiltMax1    50.0f

//--------------------------------------------------------------------
// Universal dimensions for each legs in mm

#define cXXCoxaLength     50.0f   // distance from coxa origin to femur servo
#define cXXFemurLength    80.0f   // distance from femur servo to tibia servo
#define cXXTibiaLength    120.0f  // distance from tibia servo to foot

//--------------------------------------------------------------------
// body dimensions

#define cRRCoxaAngle1   -45.0f
#define cRMCoxaAngle1     0.0f
#define cRFCoxaAngle1    45.0f
#define cLRCoxaAngle1   -45.0f
#define cLMCoxaAngle1     0.0f
#define cLFCoxaAngle1    45.0f

#define cRROffsetX       70.0f    // Distance X from center of the body to the Right Rear coxa
#define cRROffsetY      -85.0f    // Distance Y from center of the body to the Right Rear coxa
#define cRMOffsetX       70.0f    // Distance X from center of the body to the Right Middle coxa
#define cRMOffsetY        0.0f    // Distance Y from center of the body to the Right Middle coxa
#define cRFOffsetX       70.0f    // Distance X from center of the body to the Right Front coxa
#define cRFOffsetY       85.0f    // Distance Z from center of the body to the Right Front coxa

#define cLROffsetX       -70.0f   // Distance X from center of the body to the Left Rear coxa
#define cLROffsetY       -85.0f   // Distance Z from center of the body to the Left Rear coxa
#define cLMOffsetX       -70.0f   // Distance X from center of the body to the Left Middle coxa
#define cLMOffsetY         0.0f   // Distance Z from center of the body to the Left Middle coxa
#define cLFOffsetX       -70.0f   // Distance X from center of the body to the Left Front coxa
#define cLFOffsetY        85.0f   // Distance Z from center of the body to the Left Front coxa

//--------------------------------------------------------------------
// start positions

#define cHexInitXY	     (cXXFemurLength+cXXCoxaLength)
#define CHexInitXYCos45  (cHexInitXY*M_SQRT1_2)
#define CHexInitXYSin45  (cHexInitXY*M_SQRT1_2)
#define CHexInitZ        (-cXXTibiaLength)

#define cRRInitPosX     CHexInitXYCos45 + cRROffsetX
#define cRRInitPosY     -CHexInitXYSin45 + cRROffsetY
#define cRRInitPosZ     CHexInitZ

#define cRMInitPosX     cHexInitXY + cRMOffsetX
#define cRMInitPosY     cRMOffsetY
#define cRMInitPosZ     CHexInitZ

#define cRFInitPosX     CHexInitXYCos45 + cRFOffsetX
#define cRFInitPosY     CHexInitXYSin45 + cRFOffsetY
#define cRFInitPosZ     CHexInitZ

#define cLRInitPosX     -CHexInitXYCos45 + cLROffsetX
#define cLRInitPosY     -CHexInitXYSin45 + cLROffsetY
#define cLRInitPosZ     CHexInitZ

#define cLMInitPosX     -cHexInitXY + cLMOffsetX
#define cLMInitPosY     cLMOffsetY
#define cLMInitPosZ     CHexInitZ

#define cLFInitPosX     -CHexInitXYCos45 + cLFOffsetX
#define cLFInitPosY     CHexInitXYSin45 + cLFOffsetY
#define cLFInitPosZ     CHexInitZ

