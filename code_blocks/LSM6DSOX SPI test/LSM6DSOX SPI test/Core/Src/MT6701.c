/*
 * MT6701.c
 *
 *  Created on: Sep 15, 2026
 *      Author: david
 */
#include "main.h"
#include "MT6701.h"

// Tracking variables for multi-turn calculation
uint16_t last_counter_val = 0;
int32_t total_ticks = 0;

extern TIM_HandleTypeDef htim3;

void Read_Encoder(float *angle, uint8_t *dir) {
	// 1. Read hardware register directly (TIM3->CNT)
	uint16_t current_counter_val = __HAL_TIM_GET_COUNTER(&htim3);

	// 2. Signed delta calculation (handles 16-bit wrap-around cleanly)
	int16_t delta = (int16_t) (current_counter_val - last_counter_val);
	last_counter_val = current_counter_val;

	// 3. Accumulate continuous signed ticks
	total_ticks += delta;

	*angle = (float) total_ticks * (360.0f / 4096.0f);

	// 5. Hardware direction flag (0 = Clockwise/Up, 1 = Counter-Clockwise/Down)
	*dir = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
}

void Encoder_Init(void) {
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
}

