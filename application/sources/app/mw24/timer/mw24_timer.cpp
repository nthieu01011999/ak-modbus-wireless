#include "mw24_timer.h"
#include "xprintf.h"
#include "mw24_config.h"
/************************************************************************************
 * Timer 6 - timer
*************************************************************************************/
#define MW24_TIMEOUT_SEND_RECV			(3) /* ms */

static uint32_t timer_counter = 0;
static uint32_t timer_time_out;

void mw24_timer_init() {
	TIM_TimeBaseInitTypeDef  timer_10us;
	NVIC_InitTypeDef NVIC_InitStructure;
	timer_time_out = MW24_TIMEOUT_SEND_RECV;

	/* timer 10ms to polling receive IR signal */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	timer_10us.TIM_Period = 3196;		/* HCLK=8Mhz and 3196 respective HCLK=32Mhz;*/
	timer_10us.TIM_Prescaler = 10;
	timer_10us.TIM_ClockDivision = 0;
	timer_10us.TIM_CounterMode = TIM_CounterMode_Down;

	TIM_TimeBaseInit(TIM6, &timer_10us);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	/* highest priority level */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, DISABLE);
}

void mw24_timer_increase_counter() {
	/* counter */
	timer_counter++;
	if (timer_counter == timer_time_out) {
		TIM_Cmd(TIM6, DISABLE);
		MW24_DEBUG_TX_ON();
		mw24_end_sending();
		MW24_DEBUG_TX_OFF();
	}
}

void mw24_timer_reset() {
	ENTRY_CRITICAL();
	timer_counter = 0;
	EXIT_CRITICAL();
}

void mw24_timer_enable() {
	ENTRY_CRITICAL();
	TIM_Cmd(TIM6, ENABLE);
	EXIT_CRITICAL();
}

void mw24_timer_disable() {
	ENTRY_CRITICAL();
	TIM_Cmd(TIM6, DISABLE);
	EXIT_CRITICAL();
}
