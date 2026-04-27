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

// идентификатор версии программы по 4 бита
#pragma config IDLOC0 = 1 // тип
#pragma config IDLOC1 = 0 // номер сборки(десятки) 0-9
#pragma config IDLOC2 = 3 // номер сборки(единицы) 1-9-0
#pragma config IDLOC3 = 0 // резерв

void interrupt_tmr0 (void);
void HandlerDMX(void);

#define OptoSens RA3
// активация питания мотора
#define MotorEn COG1CON0bits.EN 
#define PeriodContrlBlock   40000   // ms
#define PinBlock    RA3

struct Motor {
    bool en ;
    bool sensor ;
    uint16_t current_speed; // пересчет скор в значения ЦАП 200-1000
    uint8_t target_speed; // целевая скорость в значении дмикс 0-255
    uint8_t timer; // таймер обновления текущей скорости 
    uint16_t timer_block ; // тамер контроля блокировки барабана
} motor;
void RunMotor (uint16_t );
void TimerReloadMotor (void);
void TimerControlBlock (void);

void interrupt_sensor (void);
bool OptoSensorRead (void) ;

#define OVERTIME_KEY_FLAG   TMR2IF
#define Key_U   !RE0
#define Key_C   !RE1
#define Key_D   !RE2
void ReadKey (void);
struct Key {    
    bool flag; // обновлённые данные, нажал/держишь/отпустил
    bool free ; // флаг освобождения клавиши в этом цикле, сбрасывается в след цикле
    bool lock;  // блокировка обработки кнопок до освобождения, key.free
    uint8_t duration; //продолжительность удержания
    uint8_t press; // код нажатой клавиши
    uint8_t prev; // предыдущий код клавиши
    //uint16_t timer_counter; // обработка дребезга
} key;
// коды клавиш
enum {
    KeyU = 1, KeyC, KeyD
};

void HandlerMenu(void);
typedef enum MenuFocus {
    item_start, item_set_dmx , item_select_branch , item_select_device_mode ,
    item_manual_mode , item_standalone      
} menu_t ;

struct Menu {
    bool first_flag; // первый заход в пункт меню
    menu_t focus; // активный пункт меню
    
} menu;
const uint8_t big_interval = 20; // 20*66ms 

// список режимов работы прибора
// последовательность привязана к работе функций!
// dmx меняет профиль по rdm
typedef enum DeviceMode {
    DMX_1 , DMX_2, DMX_3, MANUAL, STANDALONE , RET   
} mode_t;
// последовательности связаны 
const uint8_t* label_device_mode[] = {
    "d-1", "d-2", "d-3", "РУЧ", "A 0"    
};
// вывод составной строки
void send_string(uint8_t);
// переключение режима работы прибора
void switch_device_mode (mode_t);
// структура состояния устройства
struct Device {
    bool save_memory_flag; // есть данные для сохранения в памяти
    bool block_error; // барабан заблокирован, мотор отключен
    bool save_error; // не получается сохранить настройки
    mode_t mode;
    mode_t new_mode;
    uint16_t timer_error;  // пока только дмикс
} device;

void TimerErrorMes(void);

//работа модуля вентиляторов
void RunFan (uint16_t fan1, uint16_t fan2);

