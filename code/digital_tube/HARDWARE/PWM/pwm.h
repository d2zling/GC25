#ifndef __PWM_H
#define __PWM_H

#include "sys.h"

#define PWMA        GPIO_Pin_3
#define PWMB        GPIO_Pin_0
#define PWMA_GPIO   GPIOA
#define PWMB_GPIO   GPIOB

#define AO              0
#define BO              1

void TIM1_PWM_Init(u16 arr, u16 psc);
void TB6612_PWM_PIN_Init(u16 arr, u16 psc);
void pwm_out(u8 AO_or_BO, uint16_t duty);

#endif
