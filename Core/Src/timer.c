/*
 * timer.c
 *
 *  Created on: Nov 2, 2026
 *      Author: Acer
 */


#include "main.h"
#include "timer.h"
#include "input_reading.h"

static int timer_counter[MAX_TIMERS];
static int timer_flag[MAX_TIMERS];

void setTimer(int duration, int index) {
    if (index >= MAX_TIMERS || index < 0) return;
    timer_counter[index] = duration / TIMER_CYCLE;
    timer_flag[index] = 0;
}

int isTimerExpired(int index) {
    if (index >= MAX_TIMERS || index < 0) return 0;
    if (timer_flag[index] == 1) {
        timer_flag[index] = 0;
        return 1;
    }
    return 0;
}

void timer_run(void) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

// Ngắt định kỳ TIM2 (10ms)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        button_reading();
        timer_run();
    }
}
