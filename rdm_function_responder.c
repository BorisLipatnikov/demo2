#include <xc.h>
#include "rdm_dmx_hardware.h"


//------------------------------------------------------------------------------
// функции поиска 
void Discovery_UnMute_Message (char* Slot23) {
    
    *(Slot23 ) = 0x02; //PDL 2
    *(Slot23+1) = 0;    // 0, 0
    *(Slot23+2) = 0;    
}

void Discovery_Mute_Message (char* Slot23) {
    *(Slot23 ) = 0x02; //PDL 2
    *(Slot23+1) = 0;    // 0, 0
    *(Slot23+2) = 0;
}

char Discovery_Unique_Branch_Message (char* buffer) {
    
    char local = 0;  
// Lower Bound UID - Slot 24-29 , Upper Bound UID -  Slot 30-35
// проверка на соответствие заявленному диапазону   
    // по нижней границе
    for(char a=0; a<6; a++) {
       local = *(buffer+24+a);
       if (local > UID[a]) return 0;   // ниже нижней границы 
       if (local < UID[a]) break;  // выше нижней границы
    }  // входит по нижней границе

    // по верхней границе
    for(char a=0; a<6; a++) {
        local = *(buffer+30+a);
       if (local < UID[a]) return 0;   // выше верхней границы  
       if (local > UID[a]) break;  // ниже верхней границы
    }  // входит по верхней границе     
    
// если UID входит в диапазон формируем ответный пакет    
    unsigned int b = 0;   
    *(buffer)     = 0xFE;
    *(buffer + 1) = 0xFE;     
    *(buffer + 2) = 0xFE;
    *(buffer + 3) = 0xFE;
    *(buffer + 4) = 0xFE;
    *(buffer + 5) = 0xFE;
    *(buffer + 6) = 0xFE;
    *(buffer + 7) = 0xAA;
    *(buffer + 8) = UID[0] | 0xAA; // MID
    *(buffer + 9) = UID[0] | 0x55;
    *(buffer +10) = UID[1] | 0xAA;
    *(buffer +11) = UID[1] | 0x55;
    *(buffer +12) = UID[2] | 0xAA; // DID
    *(buffer +13) = UID[2] | 0x55;
    *(buffer +14) = UID[3] | 0xAA;
    *(buffer +15) = UID[3] | 0x55;
    *(buffer +16) = UID[4] | 0xAA;
    *(buffer +17) = UID[4] | 0x55;
    *(buffer +18) = UID[5] | 0xAA;
    *(buffer +19) = UID[5] | 0x55;
    
    for (char c=8; c<20; c++)
        b = b + *(buffer + c); 
    char d = 0;
    d = (char)(b >> 8); 
    *(buffer + 20) = d | 0xAA; // ChSum
    *(buffer + 21) = d | 0x55;
    d = (char) b;
    *(buffer + 22) = d | 0xAA;
    *(buffer + 23) = d | 0x55;
        
    return 1; // 1-UID входит в диапазон , 0- за пределами диапазона
}

//------------------------------------------------------------------------------
void Device_Label_Get(char* pdl) {
    *pdl = Device_Label[0];
    for (char a=1; a<=Device_Label[0]; a++)
        *(pdl+a) = Device_Label[a] ;
}
void Device_Label_Set(char* pdl) {
    Device_Label[0] = *pdl;
    if (Device_Label[0] > size_device_label) Device_Label[0] = size_device_label;
    for (char a=1; a<=Device_Label [0]; a++)
        Device_Label [a] = *(pdl+a);
    *pdl = 0;
}
//------------------------------------------------------------------------------
char RDM_Support_parametr (char* pdl) {      
    // 115 пид за 1 заход
    *pdl = DEVICE_PID << 1 ;    // 2 слота на 1 пид
    char* pd = pdl; 
    
    for (char b=0; b < DEVICE_PID; b++) {           
        *(++pd) = (Support_PID[b] >> 8);
        *(++pd) = (char)Support_PID[b];        
    }        
    return E120_RESPONSE_TYPE_ACK;
}
//------------------------------------------------------------------------------
void DMX_Starting_Address_Get (char* pd, int adr) {
    *pd = adr >> 8;
    *(pd+1) = (char)adr;
    *(pd-1) = 2;    // PDL
}
void DMX_Starting_Address_Set (char* pd, int* adr) {
    *adr = (*pd << 8)|(*(pd+1));
    *(pd-1) = 0;    // PDL
}
//------------------------------------------------------------------------------
void Identify_Device_Get (char* pd, char* Identif) {
    *pd = *Identif;
    *(pd-1) = 1;
}
void Identify_Device_Set (char* pd, char* Identif) {
    *Identif = *pd;
    *(pd-1) = 0;
}
//------------------------------------------------------------------------------
void DMX_Personality_Get (char* pd, char per, char total) {
    *(pd-1) = 2;
    *pd = per;
    *(pd+1) = total;
}
char DMX_Personality_Set (char* pdl) {
    *pdl = 0;    
    return *(pdl+1);
}
//------------------------------------------------------------------------------
void DMX_Personal_Description ( char* pd) {
    char* pdl = pd-1; 
    char data = *pd;
    //*pd = *pd;  // запрошеный профиль
    *(++pd) = 0;    // Footprint
    *(++pd) = Footprint_Personality [Personality];
    char count = 0;
    Label (&count, (pd+1), Personal_Descript [data]);
    *pdl = 3 + count;
}
//------------------------------------------------------------------------------
void Label ( char* PDL, char* PD , char* Label) {
    char count = 0;
    do {
        *PD = *Label;
        PD++ ;        
        Label++ ;
        count++;
    } while ((*Label) != '\0');  
    *PDL = count;
}
//------------------------------------------------------------------------------
void Software_Version_Label (char* PDL, uint8_t* soft_id) {
    char* PD = PDL + 1;    
    *PDL = 6;     
    *PD     = soft_id[0] + '0'; // soft_id[0]
    *(++PD) = 0x2E;    // '.' 
    *(++PD) = soft_id[1] + '0'; // soft_id[1] 
    *(++PD) = soft_id[2] + '0';  // soft_id[2]
    *(++PD) = 0x2E;     // '.'
    *(++PD) = soft_id[3] + '0';  // soft_id[3]
}
//------------------------------------------------------------------------------
void Device_Info (char* PDL, int adress, char soft_id[4], char personality, char total) {
    char * PD ;
    PD = PDL + 1;
    *PDL = 0x13; //19
    *(PD) = 0x01;    // pdm vers 1.20
    *(++PD) = 0;
    *(++PD) = DEVICE_MODEL_ID >> 8; // Device_Model_ID
    *(++PD) = (char)DEVICE_MODEL_ID;
    *(++PD) = Product_Category >> 8; // Product Category
    *(++PD) = (char)Product_Category;
    *(++PD) = soft_id[0]; // Soft vers, H
    *(++PD) = soft_id[1];
    *(++PD) = soft_id[2];
    *(++PD) = soft_id[3]; //L
    *(++PD) = 0; // dmx footprint , занимаемые каналы, 255 макс
    *(++PD) = Footprint_Personality [Personality];
    *(++PD) = personality; // dmx personality , профиль dmx
    *(++PD) = total; //total; // total personality
    *(++PD) = adress >> 8; // dmx start adress
    *(++PD) = (char)adress;
    *(++PD) = 0;   // Sub-Device Count, кол-во используемых подустройст
    *(++PD) = SubDevice_Count;
    *(++PD) = Sensor_Count; //sensor count
}
//------------------------------------------------------------------------------
char Modulation_Frequency_Description (char* pd, unsigned int freq)  {
    // *(pd-1) -requested modulation
    *(pd) = 0xFF; *(++pd) = 0xFF; 
    *(++pd) = 0xFF; *(++pd) = 0xFF; 
    //*(++pd) = freq>>8; *(++pd) = (char)freq;
    
    int array[5]={10000,1000,100,10,1};
    char count;
    for (char i=0;i<5;i++) {   
        count = 0;
        while (freq >= array[i]) {
            freq -= array[i];
            count++ ;
        }
        array[i] = count;
    }
    /**/
    count = 5;
    for (char i=0; i<5; i++) 
        if (array[i] == 0) count--;  else break;
    
    for (char i=5-count; i<5; i++) 
        *(++pd) = (array [i] + '0');        
    
    *(++pd) = 'H'; *(++pd) = 'z';    //!!!
    return (5+count+2);
}
    
    
    





