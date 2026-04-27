
#include "interrupt_manager.h"
#include "mcc.h"

extern void interrupt_tmr0 (void);
extern void interrupt_sensor (void);
extern void rx_interrupt (void);

void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
        interrupt_tmr0();
    }
    if(INTCONbits.PEIE == 1)
    {
        if(PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
        {
            rx_interrupt();
            EUSART_RxDefaultInterruptHandler();
        } 
    }      
}
/**
 End of File
*/
