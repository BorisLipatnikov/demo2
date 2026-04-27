#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "Led_Dig.h"
#include "rdm_dmx_hardware.h"


void ReadMemory(void) {  
    uint8_t buf[32];
    
    for (uint16_t addr = 0x1FC0, ind=0; addr<0x1FE0; ++addr, ++ind) {
        buf[ind] = (uint8_t)FLASH_ReadWord(addr);
    }
    
    uint16_t L=buf[0], H=buf[1];
    dmx.start_address = (H<<8)|L;
    dmx.new_address = dmx.start_address;
    switch_device_mode (buf[2]); // device.mode
    motor.target_speed = buf[3]; 
        
    // подпись устройства    
    // memory [8,31]   от 1FC8h 
    if (buf[8] == 0xFF) {    // нет подписи устройства 
        uint8_t* a = DEVICE_MODEL_DESCRIPT;   // !!!!        
        uint8_t b = 0;        
        do {
            ++b ;            
            Device_Label [b] = *a++;  
        } while ( *a != '\0');
        Device_Label[0] = b;
    } 
    else {            // внедрена подпись устройства
        for (uint8_t a=0, b=8; a<size_device_label; ++a, ++b)
            Device_Label[a] = buf[b]; 
    }
    
    // UID    
    UID[0] = (uint8_t)FLASH_ReadWord(0x1FFF);
    UID[1] = (uint8_t)FLASH_ReadWord(0x1FFE);
    UID[2] = (uint8_t)FLASH_ReadWord(0x1FFD);
    UID[3] = (uint8_t)FLASH_ReadWord(0x1FFC);
    UID[4] = (uint8_t)FLASH_ReadWord(0x1FFB);
    UID[5] = (uint8_t)FLASH_ReadWord(0x1FFA);
/*  
            UID[0] = 0x04;      
            UID[1] = 0x1C;
            UID[2] = 0xAA;
            UID[3] = 0xBC;
            UID[4] = 0xDE;
            UID[5] = 0xFF;  
 */ 
    // версия ПО, IDLock    
    Software_Version_ID [0] = (uint8_t)FLASH_readConfig (0x8000); // memory.h
    Software_Version_ID [1] = (uint8_t)FLASH_readConfig (0x8001); 
    Software_Version_ID [2] = (uint8_t)FLASH_readConfig (0x8002);
    Software_Version_ID [3] = (uint8_t)FLASH_readConfig (0x8003);    
}

void WriteMemory(void) {    
    device.save_memory_flag = false;
    di();
    uint16_t buf[32]={0};
    
    buf[0] = (uint8_t) dmx.start_address;
    buf[1] = dmx.start_address >> 8;  
    buf[2] = device.mode;
    buf[3] = motor.target_speed;
    
    for (uint8_t a=0, b=8; a<size_device_label; ++a, ++b)
        buf[b] = Device_Label[a]; 
    
    FLASH_WriteBlock(0x1FC0, buf);
    ei();
}               

void main(void) {
    // initialize the device
    SYSTEM_Initialize();
    ReadMemory();      
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();    
    HandlerMenu();    
                Personality = device.mode + 1; // !!!
                                
    while (1) {
        CLRWDT();
        if (TIMER_DIG_LED_FLAG) DigLedReload(); 
        if (dmx.flag_rx) HandlerDMX();
        if (rdm.flag) {UART_OFF; Handler_RDM (); UART_ON;}
        if (UART_OERR) UART_Reset();
        if (OVERTIME_KEY_FLAG)  ReadKey();  
        if (key.flag) HandlerMenu();
        if (rdm.write) {device.save_memory_flag = true; rdm.write = 0;}
        if (device.save_memory_flag) WriteMemory();
    }
}
//------------------------------------------------------------------------------
void HandlerDMX(void){
    dmx.flag_rx = 0;
    
    switch(device.mode) {
        case(DMX_1):
            motor.target_speed = dmx.data[0];
            break;
        case(DMX_2):
            motor.target_speed = dmx.data[0];
            RunFan(dmx.data[1], dmx.data[1]);            
            break;
        case(DMX_3):
            motor.target_speed = dmx.data[0];
            RunFan(dmx.data[1], dmx.data[2]);
            break;
        default:
            motor.target_speed = dmx.data[0];
            RunFan(0, 0);
    }    
}

void switch_device_mode (mode_t new_mode){
    di();
    // список контролируемых обнуляемых переменных
    motor.target_speed = 0;
    UART_OFF;
    dmx.data[0] = 0;
    dmx.data[1] = 0;
    dmx.data[1] = 0;
    
    // переключение и определение зависимых переменных
    static mode_t prev_mode;
    // сохранение текущего режима при переходе в ручной
    if ((new_mode == MANUAL)&&(device.mode != MANUAL)) 
        prev_mode = device.mode ;    
    // обратное переключение из ручного режима на предшествовавший
    else if (new_mode == RET)
        new_mode = prev_mode;
    
    device.mode = new_mode;        
    if (device.mode <= DMX_3) UART_ON;
    
    ei();
}
//------------------------------------------------------------------------------
void interrupt_tmr0 (void) {  // 1ms
     TimerReloadMotor ();
     TimerDMX (UART_CREN);
     TimerErrorMes();
     TimerControlBlock();
}

void interrupt_sensor (void) {
    return;
}

void rx_interrupt (void){
    do { Receive();
    } while(BUF_RX_FLAG);
}

//------------------------------------------------------------------------------
void ReadKey (void) {
// дребезг гасит большой период опроса кнопок
    OVERTIME_KEY_FLAG = 0;
        
    if (Key_U) key.press = KeyU;
    else if (Key_C) key.press = KeyC;
    else if (Key_D) key.press = KeyD;
    else { 
        key.duration = 0;
        key.press = 0;
    }
    
    key.free = false;
    if ((key.press == 0)&&(key.prev != 0)) {
        key.free = true;
        key.flag = true;
    }
    if (key.free) key.lock = false;
    
    if (key.press) {
        // искл случай смены нажатой клавиши м/у опросами
        if (key.prev == key.press) {
            ++key.duration;
            key.flag = true;
        }
        else key.duration = 0;
        ResetTimerWorkDisplay();
    }
    
    key.prev = key.press;       
}

void switch_focus (menu_t set_focus, bool lock ) {
    menu.focus = set_focus;
    menu.first_flag = true;
    key.lock = lock;
}

void HandlerMenu(void) {
    key.flag = false;
    if (key.lock) return;
    
    // вкл дисплея при первом нажатии с блокировкой функции кнопки    
    if ((display.en == false)&&(key.press)) {
        StartDisplayDigLed();
        key.lock = true;
        key.press = 0;        
    }
    
    switch(menu.focus) {
        // первоначальное отображение и переадресация при вкл 
        case(item_start):  
            if (device.mode == STANDALONE) {
                send_string(motor.target_speed);    
                switch_focus(item_standalone, false);
            }
            else {
                SendDisplayNumber(dmx.start_address);
            }
            if (key.press == KeyC)  {                                   
                switch_focus(item_select_branch , false);
            }
            break;
        
        case(item_select_branch):            
            if (key.duration > big_interval) { 
                display.flash = true;
                device.new_mode = MANUAL;
                switch_device_mode(device.new_mode);
                SendDisplayText(label_device_mode[device.new_mode]);
                switch_focus (item_select_device_mode, true);
            }
            else if(key.free) {
                display.flash = true;
                dmx.new_address = dmx.start_address;  
                switch_focus(item_set_dmx, false);   
            }
            break;
            
        case(item_set_dmx):            
            if (key.press == KeyC) {
                dmx.start_address = dmx.new_address;
                display.flash = false;
                device.save_memory_flag = true;
                switch_focus(item_start, true);
            }
            else if ((key.duration == 1)||(key.duration > 15)) {
                    if(key.press == KeyU) {
                        dmx.new_address += (1 + (key.duration >> 4)); 
                        if (dmx.new_address > 512) dmx.new_address = 1;                        
                    } 
                    else if (key.press == KeyD) {
                        dmx.new_address -= (1 + (key.duration >> 4));
                        if ((dmx.new_address > 512)||(dmx.new_address == 0))
                            dmx.new_address = 512;                        
                    }   
                SendDisplayNumber(dmx.new_address);
            }            
            break;
        
        case(item_select_device_mode):     
            if (key.free) return;                        
            if (key.press == KeyU) {
                if (device.new_mode < STANDALONE) ++device.new_mode;
                key.lock = true; 
            }
            else if (key.press == KeyD) {
                if (device.new_mode > DMX_1) --device.new_mode;   
                key.lock = true; 
            }                
            SendDisplayText(label_device_mode[device.new_mode]); //            
               
            if (key.press == KeyC) {
                switch_device_mode(device.new_mode);
                display.flash = false;
                if (device.mode == MANUAL) {
                    switch_focus(item_manual_mode, true);
                }
                else if (device.mode == STANDALONE) {
                    device.save_memory_flag = true;
                    switch_focus(item_standalone, true);                    
                }
                else {
                    device.save_memory_flag = true;
                    switch_focus(item_start, true);
                }  
            }
            break;
            
        case(item_manual_mode):
            motor.target_speed = 0;
            if (key.press == KeyC) {
            // восстанавливает предыдущий режим 
                switch_device_mode(RET);
                switch_focus(item_start, true);
            }
            else if (key.press) {
                motor.target_speed = 60; // ед дмикс, подбирается на глаз
            }            
            break;
            
        case(item_standalone):
            if (key.free) return;
            if ((key.press == KeyC)&&(key.duration > big_interval)) {   
                switch_focus (item_select_branch, false);  
                motor.target_speed = 0; 
                return;
            }
            else if ((key.duration == 1)||(key.duration > 15)) {
                if (key.press == KeyU) {
                    if (motor.target_speed < 250) {
                        motor.target_speed += 10;
                        device.save_memory_flag = true;
                    }
                }
                else if (key.press == KeyD) {
                    if (motor.target_speed >= 10) {
                        motor.target_speed -= 10;
                        device.save_memory_flag = true;
                    }
                } 
                
            }    
            send_string(motor.target_speed);            
            break;
    }        
}

//------------------------------------------------------------------------------
void RunFan (uint16_t fan1, uint16_t fan2) {
    if (fan1) PWM3_LoadDutyValue(500 + (fan1<<1));
    else PWM3_LoadDutyValue(0);
    
    if (fan2) PWM4_LoadDutyValue(500 + (fan2<<1));
    else PWM4_LoadDutyValue(0);
}

void RunMotor (uint16_t value) {    
    const uint16_t min_value = 230;   
    // откл питания до перезагрузки прибора
    if (device.block_error) value = 0;        
            
    if (value <= min_value) {
        DAC1_Load10bitInputData(min_value);
        MotorEn = false;
        motor.en = false;
    }
    else {    
        DAC1_Load10bitInputData(value);
        MotorEn = true;
        motor.en = true;
    }
}

void TimerReloadMotor (void) {
    ++motor.timer;
    if(motor.timer < 4) return;    
    motor.timer = 0;
    
    uint16_t value = motor.target_speed;
    value = 200 + (value<<1) + (3*value>>3) + (9*value>>4); 
    
    if (value == motor.current_speed) return;
    else if(value > motor.current_speed)  ++motor.current_speed;  
    else   --motor.current_speed; 
            
    RunMotor (motor.current_speed);
}

void TimerControlBlock (void) {
    static bool prev_stat_pin_block = 0;
    if (motor.en == false) {
        motor.timer_block = 0;
        return;
    }
    // фиксация перехода 0->1 , прерыватель на моторе, признак вращения
    if (PinBlock > prev_stat_pin_block) {
        motor.timer_block = 0;
    }
    else if (motor.timer_block <= PeriodContrlBlock) ++motor.timer_block;
    
    prev_stat_pin_block = PinBlock;
    
    if (motor.timer_block == PeriodContrlBlock) {
        device.block_error = true;
        motor.target_speed = 0;
        motor.current_speed = 0;
        RunMotor(0);
    }    
}
//------------------------------------------------------------------------------
void send_string(uint8_t number) {    
    SendDisplayNumber(number/10);
    display.buffer[0] = 'A';
}

void TimerErrorMes(void) {
    static const uint16_t period = 2000; 
    ++device.timer_error;
    if (device.timer_error < period) return;
    device.timer_error = 0;
    // костыль, мигающий экран - ошибку не выводим
    if (display.flash) return;
        
    if (dmx.error) {
        SendTopDisplayText("E-d");
        motor.target_speed = 0;
    }
    else if(device.block_error) {
        SendTopDisplayText("E-b");
        motor.target_speed = 0;
    }
    else if (device.mode == MANUAL) { // небольшой костыль
        ResetTimerWorkDisplay();
    }
    else if (display.top_mes)ReturnBuffer();
    
}