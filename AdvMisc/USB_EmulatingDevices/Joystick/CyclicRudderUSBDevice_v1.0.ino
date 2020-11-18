// The Program tests/executes a USB Joystick device driver running on Arduino Leonardo or Arduino Micro.  The Arduino Joystick class 'Joystick_' 
// is part of the Arduino Library. It allows the Arduino Leonardo to be seen by Windows OS as a USB device.  In this case here, 
// the USB device is a "Game Controller" or more specifically in my case it is the hardware 'cyclic and rudder controller for my Flight Simulator.
//
// Threr are two distinct parts of the code. The first part is reads the status of the five push-buttons and the four-way hat switch,  The second part
// reads the status of the helicopter cyclic potentiomers and the rudder potentiomers that represent degree angle of rotation,
//
// In the first section the code reads nine push-buttons including four of the nine designated as part of hat-switch.
// The code reads these buttons attached to the Arduino and maps them to the Arduino Joystick library.
//
// The second section reads the voltage values given by the center tap of the potentiometers embedded in their respective rotation
// mechanics for pitch, roll and rudder on the helicopter flight simulator mechannical hardware: the'cyclic' stick and the anti-torgue peddals . 
// These are the X-Axis and Y-Axis joystick potentiometer voltages and the anti-torque peddal potentiometer.

//
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// TESTING: From 'Contol Panel', right click on the Arduino Thumbnail, select 'Game controller settings',
//          select 'Properties', the 'Arduino Leonardo properties' popup window should appear.
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// 2020-11-08 Versio 1.0 Prototype version on protoboard breadboard.
//                       Changed testAutSendMode to 'true',  Removed 'Joystick.sendState();'   ----   Tom Ricci
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// NOTE: This code file is for use with Arduino Leonardo and Arduino Micro only. The Arduino 'UNO' and 
//       the like will not work with a personal computer PC for USB data transmission as a USB divice.
// -------------------------------------------------------------------------------------------------------------------------------------------------------
//
/*                                    // Create an joystick object: 'Joystick'
  Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  9, 0,                               // Button Count, Hat Switch Count 
  false, false, false,                // No X, Y, Z Axis
  true, true, true,                   // Rx, Ry, Rz axis rotation enabled
  false, false,                       // No rudder or throttle inputs
  false, false, false);               // No accelerator, brake, or steering inputs
*/
                                                  // Joystick.h file and Joystick.cpp file originate from a C++ language code file
#include "Joystick.h"                             // The Joystick files then interface with Windows Human Interfae Device files
Joystick_ Joystick;                               // Create Joystick object from 'Joystick_' class
const bool testAutoSendMode = true;               // Set 'Send Mode' to true to test "Auto Send" mode or              
                                                  // Set false to test "Manual Send" mode.                                  
//
                                // Variable Definitions for push-button switches
// Btn Var Name - Pin#          // Digital input pins 2 - 10 are grounded by push-buttons on 'Cyclic' when pressed  
const int btn1  =  2;           // Cyclic Button 1 is connected to Arduino pin 2  => Cyclic button assignment - TBD
const int btn2  =  3;           // Cyclic Button 2 is connected to Arduino pin 3  => Cyclic button assignment - TBD
const int btn3  =  4;           // Cyclic Button 3 is connected to Arduino pin 4  => Cyclic button assignment - TBD
const int btn4  =  5;           // Cyclic Button 4 is connected to Arduino pin 5  => Cyclic button assignment - TBD
const int btn5  =  6;           // Cyclic Button 5 is connected to Arduino pin 6  => Cyclic button assignment - TBD
const int btn6  =  7;           // Hat Switch up    button is connected to pin 7  -  Hat Switch position direction UP
const int btn7  =  8;           // Hat Switch right butoon is connected to pin 8  -  Hat Switch position direction RIGHT
const int btn8  =  9;           // Hat Switch down  button is connected to pin 9  -  Hat Switch position direction DOWN
const int btn9  = 10;           // Hat Switch left  button is connected to pin 10 -  Hat Switch position direction LEFT
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

                                // Variable Definitions for X, Y, Z Axis Rotations
const int rollPotPin = A0;      // A/D input Pin for reading roll pot input voltage (0-5 Vdc)
int rllAD_Val;                  // Roll A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at rollPotPin
int rollMapDeg;                 // Mapped analog value of A/D output converted to degree angle (0-360)

const int ptchPotPin = A1;      // A/D input Pin for reading rudder pot input voltage (0-5 Vdc)
int pchAD_Val;                  // Pitch A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at ptchPotPin
int pitchMapDeg;                // Mapped analog value of A/D output converted to degree angle (0-360)

const int rudrPotPin = A2;      // A/D input Pin for reading rudder pot input voltage (0-5 Vdc)
int rudAD_Val;                  // Rudder A/D output value result (0 - 1023) converted from input voltage (0-5 Vdc) at rudrPotPin
int rudderMapDeg;               // Mapped analog value of A/D output converted to degree angle (*0-360)

                                // Function Definitions: X, Y, Z Axis Rotations - Declarations in Joystick_ class ("Joystick.h")
void setRoll(unsigned int degree)  {    // Roll degree set
     Joystick.setRxAxis(degree);   }  
 
void setPitch(unsigned int degree) {    // Pitch degree set
     Joystick.setRyAxis(degree);   } 

void setRudder(unsigned int degree){    // Rudder degree set
     Joystick.setRzAxis(degree);   } 





                                                 
void loop()                      // *****  Main loop  *****
{  
// --------------------------------------------------------    Start / Pause Execution   ------------------------------------------------------------------                                               
  if (digitalRead(A5) != 0)                  // System Disabled if 'A5' is found HIGH - LED indicator is 'off' when sensor polling is not enabled
  { digitalWrite(LED_BUILTIN, 0); return; }  // Code runs in this tight loop when 'A5' is HIGH   
  digitalWrite(LED_BUILTIN, 1);              // Ground 'A5' to fall out of loop and begin code - LED indicator 'on' and sensor polling begins
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

      if ((lastButtonState[5] == 0)               // Check if Hatswitch contact are set to neutral
      && (lastButtonState[6] == 0)
      && (lastButtonState[7] == 0)
      && (lastButtonState[8] == 0)) 
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
