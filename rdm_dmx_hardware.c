#include <xc.h>
#include "rdm_dmx_hardware.h"
 
extern unsigned char Footprint_Personality [];
extern unsigned char Personality; 

void Receive (void) {        
    char buffer = BUF_RX;    
// сигнал BREAK
    if ((READ_FERR)&&(buffer == 0)&&(BUF_RX9 == 0)) {        
            dmx.Break = 1;          
            dmx.slot_counter = 0;
            dmx.time = 0;  
            dmx.flag_rx = 0;
            dmx.error = 0;
            dmx.synchro_raduga = 1;   
            rdm.slot_count = 1;
            rdm.overflow = 0;
    }        
// сигнал START
    else if (dmx.Break) {
        dmx.Start = buffer ;  // прием значений каналов    
        if (dmx.Start == 0) dmx.Start = 1;
        dmx.Break = 0;
    }
// выбор функции приЄма данных
    else if (dmx.Start == 1) DMX_Receive (buffer);
    else if (dmx.Start == 0xCC) RDM_Receive (buffer);
}

// прием значений каналов      
void DMX_Receive (uint8_t buffer) {    
    ++dmx.slot_counter ;        
    if (dmx.slot_counter >= dmx.start_address) {
        uint16_t x = dmx.slot_counter - dmx.start_address; 
        if (x < DMX_BUFFER_SIZE)  // защ при смене адреса
            dmx.data[x] = buffer;    
        if ((x+1) >= Footprint_Personality [Personality]) {
            dmx.flag_rx = 1;
            dmx.Start = 0; // отк вызов функции
        }
    }  
}

// приЄм пакета RDM
void RDM_Receive (uint8_t buffer) {
    static unsigned char lenght = 0 ; //
    
    rdm.buffer [rdm.slot_count] = buffer;
// общее кол-во слотов в пакете с контр суммой
    if (rdm.slot_count == 2) lenght = rdm.buffer [2] + 2 ;      
    rdm.slot_count ++ ;
    
// окончание пакета    
    if (rdm.slot_count == RDM_BUFFER_SIZE) rdm.overflow = 1; // переполнение
    if ((rdm.slot_count == lenght)|| (rdm.overflow)) {        
        rdm.buffer [0] = 0xCC;        
        dmx.Start = 0; // отк вызов функции
        rdm.flag = 1;
        lenght = 0;            
    }   
}
    
// отправка пакета рдм
void RDM_Transmit (uint8_t respond){

    //__delay_us(100);  //  176us EOP -> SOP 
    DRV_TRANSMIT; // перекл вых драйвера на трансл€цию
        
    rdm.slot_count = 0 ;
    unsigned char lenght;
    if (respond) {
        PIN_BREAK_ON ;       // BREAK , >=200us           
        __delay_us(200);     
        PIN_BREAK_OFF;         
        lenght = rdm.buffer [2] + 2; // 2 контр сум
    }
    else lenght = 24;
    
    TRANSMIT_ON;
    __delay_us(12); // MAB 
        
    do {
        NOP();
        while (!READY_TRANSMIT);
        BUF_TX9 = 1;
        BUF_TX = rdm.buffer [rdm.slot_count];
        rdm.slot_count ++ ;        
    } while ((rdm.slot_count < lenght));
    while (TRANSMIT_END);
    NOP();
    TRANSMIT_OFF;
    DRV_RECEIVE; NOP(); // перекл вых драйвера на приЄм
}

void UART_Reset(void) {
    
    UART_OFF;         // сброс флага OERR		
	dmx.slot_counter = BUF_RX;
	dmx.slot_counter = BUF_RX;
	dmx.slot_counter = BUF_RX;
	dmx.slot_counter = 0; 	
	dmx.Break = 0; 
    dmx.Start = 0; 
    dmx.flag_rx=0;
    UART_ON;    
}

void TimerDMX (bool rx_en) {
    const uint16_t period = 1000; // прерываний, ms
    if (!rx_en) {
        dmx.time = 0; 
        dmx.error = 0;  
        return;
    }         
    // 2-й сброс в BREAK
    if (dmx.time > period) dmx.error = 1;   
    else  ++dmx.time;
     
}