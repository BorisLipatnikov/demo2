
#include "interrupt_manager.h"
#include "mcc.h"

void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    if(INTCONbits.PEIE == 1)
    {
        if(PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
        {
            EUSART_RxDefaultInterruptHandler();
        } 
    }      
}
/**
 End of File
*/
