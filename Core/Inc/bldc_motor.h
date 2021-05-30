/*
 * bldc_motor.h
 *
 *  Created on: 28 maj 2021
 *      Author: piotr
 */

#ifndef INC_BLDC_MOTOR_H_
#define INC_BLDC_MOTOR_H_

typedef enum
{
	CCW = 0,
	CW = 1
}direction;

struct bldc_control
{
	TIM_HandleTypeDef	*tim_pwm;
	TIM_HandleTypeDef	*tim_com;

	TIM_OC_InitTypeDef sConfigOC;

	uint8_t step_number;
	uint32_t speed_pulse;
	uint8_t dir;
};

void bldc_motor_init(TIM_HandleTypeDef *_tim_pwm, TIM_HandleTypeDef *_tim_com);
void bldc_motor_six_step_algorithm(void);
void bldc_motor_set_speed(uint32_t speed, direction dir);

void bldc_motor_PWM_Config_Channel(uint32_t pulse, uint32_t channel);
void bldc_motor_OC_Config_Channel(uint32_t mode, uint32_t channel);

#endif /* INC_BLDC_MOTOR_H_ */
