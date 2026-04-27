#pragma once
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#endif

#include <xc.h> // include processor files - each processor file is guarded.  
#include "rdm_definition.h"

// базовые установки рдм
#define Manufactur_ID   0x041C      // Manufactur ID Имлайт
unsigned char UID[6];   // HI 0->5 LO
uint8_t Software_Version_ID [4]; // = idlock ,  Software_Version_Label - текстовое отображение Software_Version_ID
#define MANUFACTUR_LABEL      "Imlight" // 
#define DEVICE_MODEL_DESCRIPT    "Mysterium"  // подпись модели прибора
#define DEVICE_MODEL_ID     0x0005    //  уникальный номер модели
#define size_device_label 23 // tyt max 27+1
char Device_Label [size_device_label+1]; // [0] - длина записи
#define Product_Category    0x0400 // ATMOSPHERIC    
#define SubDevice_Count     0   // max 255
#define Sensor_Count        0   // naxyu
#define DEVICE_PID         6   // сверх минимума, дополнительные
 int Support_PID [DEVICE_PID] = { 
    E120_QUEUED_MESSAGE,
    E120_DEVICE_MODEL_DESCRIPTION,
    E120_MANUFACTURER_LABEL,
    E120_DEVICE_LABEL,
    E120_DMX_PERSONALITY,
    E120_DMX_PERSONALITY_DESCRIPTION
    //E137_1_CURVE,
    //E137_1_CURVE_DESCRIPTION,    
    //E137_1_MODULATION_FREQUENCY,
    //E137_1_MODULATION_FREQUENCY_DESCRIPTION
 };
/* базовые PID
 * E120_DISC_UNIQUE_BRANCH
 * E120_DISC_MUTE
 * E120_DISC_UN_MUTE
 * E120_DEVICE_INFO
 * E120_IDENTIFY_DEVICE 
 * E120_DMX_START_ADDRESS   // all device dmx
 * E120_SOFTWARE_VERSION_LABEL
 * E120_SUPPORTED_PARAMETERS // как правило
 * E120_PARAMETER_DESCRIPTION  // выборочно, при необходимости
*/
uint8_t Personality;   // текущий профиль dmx, от 1!
#define Total_Personality   3  // общее кол-во профилей dmx
// подписи профилей
char* const Personal_Descript [Total_Personality+1] = {
    "",
    "Motor", 
    "Motor+Fan1",        
    "Motor+Fan2"   
};
// кол-во занимаемых каналов в каждом профиле dmx
uint8_t Footprint_Personality [Total_Personality+1] = {0,1,2,3};
 
// ошиб 0/1 отл - (Personality), 
char Personality_Set (char); //

void Handler_RDM (void) ;    
char Checksum_verification (void); // 1-ошибка , 0-совпадает
char Adress_verification (void); // адресовано 1-другим, 0-нам(+ широковещ)
void Write_buffer (char);

//  ret 1- UID входит в диапазон , 0- за пределами диапазона
//  char* - rdm.buffer[0]
//  char[6] - UID устройства
char Discovery_Unique_Branch_Message (char* );
void Discovery_Mute_Message (char*);
void Discovery_UnMute_Message (char*);

// *PDL , start adr, soft id, personality, total pers
void Device_Info (char*, int, char[4], char, char);
// *PDL , char Software_Version_ID[4]
void Software_Version_Label (char*, uint8_t* );
// *PDL, *PD, *Label 
void Label ( char*, char*, char*);
// *PDL
void Device_Label_Get (char*);
void Device_Label_Set (char*);
//  *PD, *Identif 
void Identify_Device_Get (char*, char*);
void Identify_Device_Set (char*, char*);
//  *PD , *dmx.adress
void DMX_Starting_Address_Get (char*, int);
void DMX_Starting_Address_Set (char*, int*);
// *PD, Personality, Total pers
void DMX_Personality_Get (char* , char, char);
// ret Personality , *PDL
char DMX_Personality_Set (char* );
//  *PDL, Footprint, *Personality_Description , 
void DMX_Personal_Description ( char* );
// type_respons -  *PD, DEVICE_PID, * Support_PID
char RDM_Support_parametr (char*);
//char RDM_Parametr_Descript (char*);
// не реализован пид
char Error_PID (void);
// переполнение приемного буффера
char Error_Overflow (void);
// выход за пределы диапазона
char Error_Range(void);

// (rdm.PDL, rdm.PD)
//void Curve_Description (char*, char*);
// rdm.PDL (, rdm.PD)
char Modulation_Frequency_Description (char*, unsigned int);
