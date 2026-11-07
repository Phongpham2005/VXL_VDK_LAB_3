/*
 * timer.h
 *
 *  Created on: Nov 2, 2026
 *      Author: Acer
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#define TIMER_CYCLE 10  // Chu kỳ ngắt timer cơ sở là 10ms
#define MAX_TIMERS 10   // Số lượng software timer tối đa

void setTimer(int duration, int index);
int isTimerExpired(int index);
void timer_run(void);

#endif /* INC_TIMER_H_ */
