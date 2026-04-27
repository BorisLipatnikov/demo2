 #include <xc.h>
 #include "pwm4.h"

 /**
   Section: PWM Module APIs
 */

 void PWM4_Initialize(void)
 {
    // Set the PWM to the options selected in the PIC10 / PIC12 / PIC16 / PIC18 MCUs.
    // PWM4POL active_lo; PWM4EN enabled; 
    PWM4CON = 0x90;   

    // DC 0; 
    PWM4DCH = 0x00;   

    // DC 0; 
    PWM4DCL = 0x00;   

    // Select timer
    CCPTMRS2bits.P4TSEL = 1;
 }

 void PWM4_LoadDutyValue(uint16_t dutyValue)
 {
     // Writing to 8 MSBs of PWM duty cycle in PWMDCH register
     PWM4DCH = (dutyValue & 0x03FC)>>2;
     
     // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
     PWM4DCL = (dutyValue & 0x0003)<<6;
 }
 /**
  End of File
 */
