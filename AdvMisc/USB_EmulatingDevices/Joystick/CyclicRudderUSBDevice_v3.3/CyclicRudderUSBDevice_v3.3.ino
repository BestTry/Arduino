// The purpose of this code to to allow helicopter cyclic and rudder controls interface with the Windows OS  Windows recognizes the USB device as a 
// keyboard/mouse/game-controller and will therefore will allow the physical hardware and electonics embedded within to be used with a flight control
// simulator or game software. Only the helicopter cyclic and rudder controle are implemented with one USB cable plugged into the PC for both controls.  
//
// The Program tests/executes a USB Joystick device driver running on Arduino Leonardo or Arduino Micro.  The Arduino Joystick class 'Joystick_' 
// is part of the Arduino Library. It allows the Arduino Leonardo to be seen by Windows OS as a USB device.  In this case here,the USB device
// is seen by Windows as a "Game Controller", or in my case it is the 'cyclic and rudder' controller for my Flight Simulator.
//
// There are two distinct parts of the code. The first part is reads the status of the five push-buttons and the four-way hat switch,  The code's 
// second part reads the status of the helicopter cyclic potentiomers and the rudder potentiomers that represent angle of degrees of rotation.
//
// In more detail, in the first section, the code reads nine push-buttons including four contacts that are a part of the hat-switch. The code reads
// these buttons attached to the Arduino's digital pins and maps them to the Arduino Joystick library.
//
// The second section reads the voltage values given by the center tap of the potentiometers embedded in their respective rotation mechanics for 
// pitch, roll and rudder on the helicopter flight simulator mechannical hardware: the'cyclic' stick and the anti-torgue peddals.  These are the 
// X-Axis and Y-Axis joystick potentiometer voltages and the anti-torque peddal potentiometer. All potentiometers are excited with 5 Vdc at one 
// end and are grounded to the Arduino's ground at the other end.
//
//----------------------------------------------------------------------------------------------------------------------------------------------------------
// TESTING: Observe the movement and status of the axis and pushbuttons in Windows: From 'Contol Panel', right click on the Arduino Thumbnail,
//          select 'Game controller settings', select 'Properties', then the 'Arduino Leonardo properties' popup window should appear.
//----------------------------------------------------------------------------------------------------------------------------------------------------------
// *** Note! *** - Code designed for the Arduino 'Pro Micro' using the USB translator abord the Atmel MEGA32U4 Other versions will require pin re-assignments.
// 2020-11-25 Versio 3.1 Prototype version on protoboard breadboard.  Replace the Arduino Leonardo with the Arduino 'Pro Micro' a significantly reduced 
// real estate micro-controller that is compatible with its larger big brother version. Re-assigned the analog inputs pins from ('A0', 'A1', 'A2') to
// to ('A1', 'A2', 'A3') in order to streamline the traces on the perf board circuit board ********** This version is for the 'Pro Micro"  ----   Tom Ricci
//----------------------------------------------------------------------------------------------------------------------------------------------------------
// NOTE 1: This code file is for use with Arduino Leonardo Pro Micro only. The Arduino 'UNO' and the like will not work with a personal computer
//         PC for USB data transmission as a USB divice.  NOTE 2: The 'A5' pin usage has ben replaced with groundng of the 'A3' pin to jump start program
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// Diagram shows details of cable and connectors between helicopter cyclic bulkhead connector and the Micro-B connector on the 'Mico' micro-controller board
//     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     - 
// Documentation for making USB cable                           Red      White     Green     N/A      Black
// Looking at connectors face on: USB Pin Assignments:          1: 5V    2: Data   3:Data+   4: Mode  5: Gnd / <Shield>
//         USB Micro-b Plug                                                                                                    USB Type-B Receptacle
//           _____________                                       Cable Termination Connectors                                          ______                                                        ______
//          |             |    Cable Plug End ....................................................... Cable Bulhead Receptacle End    | 2  1 |   
//           \ 5 4 3 2 1 /     <<<<<======================================   Cable   =======================================>>>>>>    | 3  4 |--- 5 Shield
//             ---------                                                                                                               ------
//   USB 'Micro-B' Pin MALE (plug) cable end                                                                    USB 'Type-B' FEMALE Receptacle Bulkhead end 
// (Attaches to Arduino USB Micro-B female recept)                                             (Mates with USB Type-BUSB male plug on cable that goes to PC)
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
//     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -     -
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
//  Diagram shows pin details of the DB-15 connector mounted on the Cyclic bulkhead for transmitting the Anti-Torque' Pedal signal values  
////                                       Looking at the face of the DB-15 receptacle connector:  All pins are N/C unless the three otherwise noted   
//         DB-15 Receptacle                Pin Assignments:    Pin 1: Gnd    ...   Pin 4: 5V   ...   Pin 11: Pot Volts    
//     _________________________                                 
//    |  8  7  6  5  4  3  2  1 |          This DB-15 recptacle connector attaches to the DB-15 plug connector on the bulkhead of the anti-torque pedals    
//     \  15 14 13 12 11 10 9  /           via a standard RS-232 extention cable with a DB-15 male on one end and a female DB-15 on the opposite end
//       ---------------------                                                                                                             
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
/*                                    // Not necessary to use this version constructor
                                      // Create an joystick object: 'Joystick'
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  5, 4,                               // Button Count, Hat Switch Count 
  false, false, false,                // No X, Y, Z Axis
  true, true, true,                   // Rx, Ry, Rz axis rotation enabled
  false, false,                       // No rudder or throttle inputs
  false, false, false);               // No accelerator, brake, or steering inputs
*/
                                      // Joystick.h file and Joystick.cpp file originate from a C++ language code file
#include "Joystick.h"                 // The Joystick files then interface with Windows Human Interfae Device files
Joystick_ Joystick;                   // Create 'Joystick' object from 'Joystick_' class
const bool testAutoSendMode = true;   // Set 'Send Mode' to true to test "Auto Send" mode or              
                                      // Set false to test "Manual Send" mode
                                                                                  
                                // Digital Input Section - Reading push-button & hat-switch status on  Cyclic control                                                        
// Arduino Board I/O Pins       // Variable Definitions to Arduino 'Micro' pin numbers for push-button switches           Cyclic Bulkhead DB-15      
// Btn Var Name - Pin#          // Digital input pins 2 - 10 are grounded by push-buttons on 'Cyclic' when pressed        --- Pin# --- Wire ---
const int btn1  =  2;           // Cyclic Button 1 is connected to Arduino pin 2  => Cyclic button position - Trigger      -    2   -   Rd
const int btn2  =  3;           // Cyclic Button 2 is connected to Arduino pin 3  => Cyclic button position - Left Mid     -    3   -   Org
const int btn3  =  4;           // Cyclic Button 3 is connected to Arduino pin 4  => Cyclic button position - Left Rear    -    4   -   Ylw
const int btn4  =  5;           // Cyclic Button 4 is connected to Arduino pin 5  => Cyclic button position - Left Top     -    5   -   Grn
const int btn5  =  6;           // Cyclic Button 5 is connected to Arduino pin 6  => Cyclic button position - Right Top    -    6   -   Blu
const int btn6  =  7;           // Hat Switch up    button is connected to pin 7  -  Hat Switch position direction UP      -    7   -   Vlt
const int btn7  =  8;           // Hat Switch right butoon is connected to pin 8  -  Hat Switch position direction RIGHT   -    8   -   Gry
const int btn8  =  9;           // Hat Switch down  button is connected to pin 9  -  Hat Switch position direction DOWN    -    9   -   Wht
const int btn9  = 10;           // Hat Switch left  button is connected to pin 10 -  Hat Switch position direction LEFT    -   10   -   Blk
//                GND           // Two ground labelled (designated) pins on the 'Arduino Micro connected the DB-15 pin #   -    1   -   Brn
//
                                // Array btnPin[] holds which digital pins are declared/defined to be read
                                // Array 'lastButtonState' holds last state of button                               
int btnPin[9] = { btn1, btn2, btn3, btn4, btn5, btn6, btn7, btn8, btn9 };
int lastButtonState[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int currentBtnState = 0;
int pinNum;
                                // The digital pins 2, 3, 4, 5, 6, 7, 8, 9, 10 
                                // are grounded when they are pressed.
void setup()
{                               // Initialize Button Pins: Enable internal pullup resistors keeping pins HIGH till pressed
  pinMode(btn1,  INPUT_PULLUP); // I could have used the following code:
  pinMode(btn2,  INPUT_PULLUP); //  ......................................................
  pinMode(btn3,  INPUT_PULLUP); //        for (int btn = 2; bnt < 10; btn++)
  pinMode(btn4,  INPUT_PULLUP); //        {
  pinMode(btn5,  INPUT_PULLUP); //            pinMode (btn, INPUT_PULLUP);
  pinMode(btn6,  INPUT_PULLUP); //        }
  pinMode(btn7,  INPUT_PULLUP); //  .....................................................                             
  pinMode(btn8,  INPUT_PULLUP); // But in this way, buttons can be defined/assigned uniquely 
  pinMode(btn9,  INPUT_PULLUP); // They do not have to be assigned sequentiallystic

  pinMode(A5,    INPUT_PULLUP); // Used to pause / start code
  pinMode(LED_BUILTIN, OUTPUT); // 'LED_BUILTIN' must be part of compilers defined word 

                                // Set rotation range in degrees for X,Y,Z axis
  Joystick.setRxAxisRange(0, 360);
  Joystick.setRyAxisRange(0, 360);
  Joystick.setRzAxisRange(0, 360);
  

/*
  Serial.begin(9600);           // Enable the serial monitor for testing / demo purposes  */
  Joystick.begin();             // Initialize Joystick Library
}                               // End of setup()

                                // Analog Input Section
                                // Variable Definitions pin designations for reading pot analog voltages for X, Y, Z Axis Rotations
const int rollPotPin = A1;      // A/D input pin for reading roll pot input voltage (0-5 Vdc)
int rllAD_Val;                  // Roll A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at rollPotPin
int rollMapDeg;                 // Mapped analog value of A/D output converted to degree angle (0-360)

const int ptchPotPin = A2;      // A/D input pin for reading rudder pot input voltage (0-5 Vdc)
int pchAD_Val;                  // Pitch A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at ptchPotPin
int pitchMapDeg;                // Mapped analog value of A/D output converted to degree angle (0-360)

const int rudrPotPin = A3;      // A/D input pin for reading rudder pot input voltage (0-5 Vdc)
int rudAD_Val;                  // Rudder A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at rudrPotPin
int rudderMapDeg;               // Mapped analog value of A/D output converted to degree angle (*0-360)

                                // Function Definitions: X, Y, Z Axis Rotations - Function declarations in Joystick_ class ("Joystick.h")
void setRoll(unsigned int degree)  {    // Roll degree set
     Joystick.setRxAxis(degree);   }  
 
void setPitch(unsigned int degree) {    // Pitch degree set
     Joystick.setRyAxis(degree);   } 

void setRudder(unsigned int degree){    // Rudder degree set
     Joystick.setRzAxis(degree);   } 




// zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
                                                 
void loop()                      // >>>>>>>>>>>   Main Loop   <<<<<<<<<<<<
{
/* The following code section was / is enabled during developemt and demonstration.  It delays the start of code execution until the 'A0' is grounded    
// --------------------------------------------------------    Start / Pause Execution   ------------------------------------------------------------------                                               
  if (digitalRead(A0) != 0)                  // System Disabled if 'A0' is found HIGH - LED indicator is 'off' when sensor polling is not enabled
  { digitalWrite(LED_BUILTIN, 0); return; }  // Code runs in this tight loop when 'A0' is HIGH   
  digitalWrite(LED_BUILTIN, 1);              // Ground 'A0' to fall out of loop and begin code - LED indicator 'on' and sensor polling begins  */
// --------------------------------------------------------------------------------------------------------------------------------------------------------                                                                                                                 
/* ****************************************************************************************************************************************************** */
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   Part I - Polling Switches and Hatswitch    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  for (int ndx = 0; ndx < 9; ndx++)               // Polling status of every pin
  {
    pinNum = btnPin[ndx];                         // Read Arduino predefined input digial pin
    currentBtnState = !digitalRead(pinNum);       // Reverse logic for PC: '0' defines button press 
    if (currentBtnState != lastButtonState[ndx])  // Skip if no change in pin status
    {                                             // Otherwise service the button that is pressed
      lastButtonState[ndx] = currentBtnState;

      if ((lastButtonState[5] == 0)               // Check if Hatswitch contacts are set to center/neutral
      && (lastButtonState[6]  == 0)
      && (lastButtonState[7]  == 0)
      && (lastButtonState[8]  == 0)) 
      {Joystick.setHatSwitch(0, -1);     }
      
      switch (ndx)                                //Service handlers for each button      
      {
        case 0:                                   // Index 0, button 1, pin 2    
          Joystick.setButton(btn1, currentBtnState); 
          // Use for testg - Serial.print( "Button 1 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          break;
        case 1:                                   // Index 1, button 2, pin 3    
          Joystick.setButton(btn2, currentBtnState); 
          // Use for testg - Serial.print( "Button 2 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          break;        
        case 2:                                   // Index 2, button 3, pin 4    
          Joystick.setButton(btn3, currentBtnState); 
          // Use for testg - Serial.print( "Button 3 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          break;  
        case 3:                                   // Index 3, button 4, pin 5    
          Joystick.setButton(btn4, currentBtnState); 
          // Use for testg - Serial.print( "Button 4 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          break;
        case 4:                                   // Index 4, button 5, pin 6    
          Joystick.setButton(btn5, currentBtnState); 
          // Use for testg - Serial.print( "Button 5 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          break;
        case 5:                                   // Index 5, button 6, pin 7    
          Joystick.setButton(btn6, currentBtnState); 
          // Use for testg - Serial.print( "Button 6 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          if (lastButtonState[5] == 1) {         // Degree 0 - Up
          Joystick.setHatSwitch(0, 0); }          
          break;
        case 6:                                   // Index 6, button 7, pin 8    
          Joystick.setButton(btn7, currentBtnState); 
          // Use for testg - Serial.print( "Button 7 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          if (lastButtonState[6] == 1) {          // Degree 90 - Right
          Joystick.setHatSwitch(0, 90);}
          break;        
        case 7:                                   // Index 7, button 8, pin 9    
          Joystick.setButton(btn8, currentBtnState); 
          // Use for testg - Serial.print( "Button 8 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          if (lastButtonState[7] == 1)  {         // Degree 180 - Down
          Joystick.setHatSwitch(0, 180);}
          break;  
        case 8:                                   // Index 8, button 9, pin 10    
          Joystick.setButton(btn9, currentBtnState); 
          // Use for testg - Serial.print( "Button 9 state at pin: "); // Use for testg - Serial.print(pinNum); 
          // Use for testg - Serial.print(" is "); Serial.println(currentBtnState);
          if (lastButtonState[8] == 1)  {         // Degree 270 - Left
          Joystick.setHatSwitch(0, 270);}          
          break;                   
      }
    }
  }
/* ****************************************************************************************************************************************************** */  
// ---------------------------------------------------------------    End of Part I     -------------------------------------------------------------------
// ........................................................................................................................................................

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>    Part II Transponding X Y Z Axis Positions    <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  rllAD_Val = analogRead(rollPotPin);
  rollMapDeg = map(rllAD_Val, 0, 1023, 0, 360);   // Pot travel is limited from 0 to 270 degrees
  // Use for testg - Serial.print(" Roll degree: "); Serial.println(rollMapDeg);
  setRoll(rollMapDeg); 
  
  pchAD_Val = analogRead(ptchPotPin);
  pitchMapDeg = map(pchAD_Val, 0, 1023, 0, 360);  // Pot travel is limited from 0 to 270 degrees
  // Use for testg - Serial.print(" Pitch degree: "); Serial.println(pitchMapDeg);
  setPitch(pitchMapDeg);

  rudAD_Val = analogRead(rudrPotPin);
  rudderMapDeg = map(rudAD_Val, 0, 1023, 0, 360); // Pot travel is limited from 0 to 270 degrees
  // Use for testg - Serial.print(" Rudder degree: "); Serial.println(rudderMapDeg);
  setRudder(rudderMapDeg);  
    
  // Joystick.sendState();
  // Use for testg - Serial.print(""); Serial.println("\n");
                                                     // delay(5000);  Use 5000 (Five seconds for demo on Serail Nonitor)
   delay(10);                                        // Loop delay for main loop
}
