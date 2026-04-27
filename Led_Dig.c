#include "Led_Dig.h"
#include <stdbool.h>


void SetChar (uint8_t value) {
    
    switch (value) {
        case('0'):
            DIG(1,1,1,1,1,1,0); break;           
        case('1'):
            DIG(0,1,1,0,0,0,0); break;
        case('2'):
            DIG(1,1,0,1,1,0,1); break;
        case('3'):
            DIG(1,1,1,1,0,0,1); break;
        case('4'):
            DIG(0,1,1,0,0,1,1); break;
        case('5'):
            DIG(1,0,1,1,0,1,1); break;
        case('6'):
            DIG(1,0,1,1,1,1,1); break;
        case('7'):
            DIG(1,1,1,0,0,0,0); break;    
        case('8'):
            DIG(1,1,1,1,1,1,1); break;    
        case('9'):
            DIG(1,1,1,0,0,1,1); break;    
        case('Р'):
            DIG(1,1,0,0,1,1,1); break;    
        case('У'):
            DIG(0,1,1,1,0,1,1); break;    
        case('Ч'):
            DIG(0,1,1,0,0,1,1); break; 
        case('-'):
            DIG(0,0,0,0,0,0,1); break;    
        case('d'):
            DIG(0,1,1,1,1,0,1); break;  
        case('b'):
            DIG(0,0,1,1,1,1,1); break;     
        case('S'):
            DIG(1,0,1,1,0,1,1); break;    
        case('A'):
            DIG(1,1,1,0,1,1,1); break;   
        case('E'):
            DIG(1,0,0,1,1,1,1); break;     
        default:
            DIG(0,0,0,0,0,0,0); break;    
        /*
        case():
            DIG();
            break;         
         */    
    }
}

void SetOutDig (uint8_t value) {
    OUT_DIG1(false); 
    OUT_DIG2(false); 
    OUT_DIG3(false); 
    switch(value) {
        case(1):
            OUT_DIG1(true);  break;
        case(2): 
            OUT_DIG2(true);  break;
        case(3): 
            OUT_DIG3(true);  break;                   
    }    
}

uint8_t operation_flash(void) { // ~12ms
    uint8_t ret = 0;
    
    if (display.flash == false) {
        display.timer_flash = 0;
        return ret;
    }
    
    ++display.timer_flash;
    if (display.timer_flash > 80) display.timer_flash = 0;
    if (display.timer_flash < 20) ret = 1;
    
    return ret;
}

void DigLedReload (void) {  // период вызова ~4мс
    RESET_FLAG(); 
    TimerWorkDigLed();  // длительность работы дисплея
    if (display.en == false) return;    
    
    SetOutDig(0);
    ++display.dig ;
    if (display.dig > Num_Digits) {   
        if (operation_flash()) {
            display.dig = Num_Digits; 
            return;
        }
        else  
            display.dig = 1;            
    }
    
    SetChar(display.buffer[display.dig-1]);
    SetOutDig(display.dig);    
}

void SendDisplayText(uint8_t* txt) {
    // сброс вывода поверх
    display.top_mes = false;
    // макс Num_Digits символов, 
    // кодировка win1251
    StartDisplayDigLed ();
    uint8_t array[Num_Digits];       
    for (char a=0; a<Num_Digits; ++a) 
        display.buffer[a] = 0;
    
    uint8_t count = 0; 
    while((txt[count] != '\0')&&(count != Num_Digits)) {        
        display.buffer[count] = txt[count]; 
        ++count;
    }   
}

void SendDisplayNumber (uint16_t value) {
    display.top_mes = false;
    // value <= 999 ;
    StartDisplayDigLed ();
    uint8_t array[Num_Digits] = {100,10,1};
    uint8_t count;
    for (char i=0; i<Num_Digits; i++) {   
        count = 0;
        while (value >= array[i]) {
            value -= array[i];
            ++count ;
        }
        array[i] = count;
    }
    array[2] += '0';
    if (array[0] != 0) {
        array[0] += '0';
        array[1] += '0';
    }
    else if (array[1] != 0) array[1] += '0';
    
    for (char a=0; a<3; ++a) 
        display.buffer[a] = array[a];
}

void SendTopDisplayText(uint8_t* txt){
    if(!display.top_mes) {
        for (char a=0; a<Num_Digits; ++a) {
            display.save_buffer[a] = display.buffer[a];
        }
    }    
    SendDisplayText(txt);   
    display.top_mes = true;
}

void TimerWorkDigLed (void) {
    if (display.timer < PeriodTimerWorkDigLed) ++display.timer;
    else StopDisplayDigLed(); 
}

void StartDisplayDigLed (void) {    
    ResetTimerWorkDisplay();
    display.en = true;
    StartDigLedTimer;    
}

void StopDisplayDigLed(void) {
    StopDigLedTimer;    
    SetOutDig(0); 
    display.en = false;
}

void ResetTimerWorkDisplay (void){
    display.timer = 0;    
}

void ReturnBuffer(void){
    for (char a=0; a<Num_Digits; ++a) 
        display.buffer[a] = display.save_buffer[a];
    display.top_mes = false;
}