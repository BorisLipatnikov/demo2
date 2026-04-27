// библиотека работы с цифровым Лед дисплеем 
// ver 1.1

#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef	__cplusplus
}
#endif /* __cplusplus */
#endif	/* XC_HEADER_TEMPLATE_H */

#include "mcc_generated_files/tmr1.h"

// работа с 3-х значным цифровым светодиодным дисплеем
#define Num_Digits 3
// схема с общим катодом , подключение разрядов дисплея
#define OUT_DIG1(x)    RC0 = x, NOP() 
#define OUT_DIG2(x)    RC1 = x, NOP()
#define OUT_DIG3(x)    RC2 = x, NOP()

#define Dig_A(x)    RD0 = x, NOP()
#define Dig_B(x)    RD1 = x, NOP()
#define Dig_C(x)    RD2 = x, NOP()
#define Dig_D(x)    RD3 = x, NOP()
#define Dig_E(x)    RD4 = x, NOP()
#define Dig_F(x)    RD5 = x, NOP()
#define Dig_G(x)    RD6 = x, NOP()
#define Dig_Dp(x)   RD7 = x, NOP()
#define DIG(a,b,c,d,e,f,g)  Dig_A(a), Dig_B(b), Dig_C(c), Dig_D(d), Dig_E(e),\
Dig_F(f), Dig_G(g)

/*
   A
F      B
   G
E      C
   D
 */

#define TIMER_DIG_LED_FLAG  PIR1bits.TMR1IF
#define RESET_FLAG()  TIMER_DIG_LED_FLAG = 0
#define StopDigLedTimer    TMR1_StopTimer()
#define StartDigLedTimer   TMR1_StartTimer()

// перегружает отображаемый символ 
void DigLedReload (void);
// отправить на экран число
void SendDisplayNumber (uint16_t );
// отправить на экран строку
void SendDisplayText(uint8_t* txt) ;
// сохраняет тукущее знач буфера и выводит поверх новое
void SendTopDisplayText(uint8_t* txt);
// возвращ из нижнего буфера на экран значение
void ReturnBuffer(void);

// таймер автоотключения дисплея
void TimerWorkDigLed (void);
// сброс таймера автоотключения
void ResetTimerWorkDisplay (void);

// запуск / стоп дисплея
void StartDisplayDigLed (void);
void StopDisplayDigLed (void);

// период активности дисплея в циклах переполнения(4мс)
const uint16_t PeriodTimerWorkDigLed = 30000;

struct Display {
    bool en ; // активность дисплея
    bool flash; // активация режима мигания текста на дисплее
    bool top_mes; // 
    uint16_t timer_flash; // таймер мигания дисплея в циклах перезагрузки
    uint8_t buffer [Num_Digits] ; // отображаемые символы
    uint8_t save_buffer [Num_Digits] ; // сохр текущее для вывода поверх
    uint8_t dig ;  // текущий отображаемый разряд (1, 2, 3), счет слева на право    
    uint16_t timer; // таймер автоотключения дисплея.
} display;
