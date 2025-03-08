#ifndef _H_STC15_CONF
#define _H_STC15_CONF

#include "STDDEF.H"
#include "INTRINS.H"

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;

#ifndef __VSCODE_C51__
#define at(x) _at_ x
#define interrupt(x) interrupt x
#define interrupt_using(x, y) interrupt x using y
#endif

#ifndef MAIN_Fosc
#define MAIN_Fosc 16000000L // Ö÷Ê±ÖÓ 16MHz
#endif

#ifndef MCU_Type
#define MCU_Type STC15F_L2K60S2 // 1KB
#endif

#define MainFosc_KHZ (MAIN_Fosc / 1000)

#include "STC15Fxxxx.H"

#endif