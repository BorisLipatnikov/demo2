#pragma once

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>
#include "rdm_protocol.h"

// аппаратная привязка
#define DMX_BUFFER_SIZE     5   // макс кол-во каналов dmx прибора 
#define RDM_BUFFER_SIZE     70  // мин кол-во слотов пакета rdm 25+32+2
#define BUF_RX      RCREGbits.RC1REG
#define BUF_RX9     RC1STAbits.RX9D
#define BUF_RX_FLAG  RCIF
#define UART_CREN   RC1STAbits.CREN
#define UART_ON     UART_CREN = true // активация схемы приёмника USART
#define UART_OFF    UART_CREN = false 
#define READ_FERR   RC1STAbits.FERR
#define UART_OERR   RC1STAbits.OERR
#define DMX_Dir     RC5
#define PIN_BREAK_ON   RC6 = 0, RC6PPS = 0 
#define PIN_BREAK_OFF   RC6PPS = 0x24 //TRANSMIT_ON // настройка по камню
#define DRV_RECEIVE    DMX_Dir = 0
#define DRV_TRANSMIT   DMX_Dir = 1
#define BUF_TX      TX1REG
#define BUF_TX9     TX9D
#define TRANSMIT_ON     TX1STAbits.TXEN = 1
#define TRANSMIT_OFF    TX1STAbits.TXEN = 0
#define READY_TRANSMIT  TXIF
#define TRANSMIT_END    !(TX1STAbits.TRMT) // 0-full , 1-empty

#ifndef _XTAL_FREQ
#include "mcc_generated_files/device_config.h"
#endif

void Receive(void); // 
void UART_Reset(void);
void TimerDMX (bool rx_en); // 

struct DMX{    
    bool Break  ;
    bool error  ;
    bool flag_rx ;   //флаг приема пакета, сброс в BREAK и при чтении        
    bool synchro_raduga ;
    
    uint8_t Start ;
    uint16_t start_address;    // 1-512 !
    uint16_t new_address;    
    uint16_t slot_counter;   // счетчик DMX каналов
    uint16_t time;      // счетчик таймаута, 1с
    uint8_t data[DMX_BUFFER_SIZE];   // приемный буфер dmx
} dmx;

void DMX_Receive (uint8_t) ;
void RDM_Receive (uint8_t) ;
void RDM_Transmit (uint8_t ); // 0-без break, 1-break

struct {     
    unsigned flag : 1 ; // флаг приёма пакета рдм
    unsigned broadcast : 1 ;
    unsigned personal : 1 ;
    unsigned mute : 1 ;  // 1-немой, 0-отвечает
    unsigned write : 1 ;  // 1-приняты данные для сохранения в память мк
    unsigned overflow : 1; // переполлнение буфера рдм
        
    unsigned char buffer [RDM_BUFFER_SIZE]; // приёмный буффер пакета рдм
    unsigned char slot_count;
    unsigned int PID;
    unsigned char Class;
    unsigned char* PDL;
    unsigned char * PD;
    unsigned char Identif;
    
} rdm;
