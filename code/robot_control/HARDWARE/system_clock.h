#ifndef __SYSTEM_CLOCK_H
#define __SYSTEM_CLOCK_H

#include "timer.h"

typedef struct _sys_count_time_t 
{
    char count_time_state;
    long long int start_time;
    long long int count_now_time;
    long long int count_end_time;

}sys_count_time_t;

extern long long int sys_time_ms;
extern u8 timer_10ms_flag;

void system_clock_init(void);
void start_count_time(sys_count_time_t *timer);
char count_time(sys_count_time_t *timer);
void reset_timer(sys_count_time_t *timer);

long long int get_minutes(void);
long long int get_seconds(void);
long long int* ms_2_minutes_seconds(void);

#endif