/*
 * bldc_motor.c
 *
 *  Created on: 28 maj 2021
 *      Author: piotr
 */

#include "main.h"

#define ARR_TIM3_VALUE			100
#define BLDC_MOTOR_MAX_SPEED	100

struct bldc_control bldc;

void bldc_motor_init(TIM_HandleTypeDef *_tim_pwm, TIM_HandleTypeDef *_tim_com)
{
	bldc.tim_pwm = _tim_pwm;
	bldc.tim_com = _tim_com;

	bldc.step_number = 1;
	bldc.speed_pulse = 0;
	bldc.dir = CW;

	bldc.sConfigOC.OCMode = TIM_OCMODE_PWM1;
	bldc.sConfigOC.Pulse = bldc.speed_pulse;
	bldc.sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	bldc.sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	bldc.sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	bldc.sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	bldc.sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	__HAL_TIM_SET_AUTORELOAD(bldc.tim_com, ARR_TIM3_VALUE);

	HAL_TIM_Base_Start(bldc.tim_com);
	HAL_TIMEx_ConfigCommutationEvent_IT(bldc.tim_pwm, TIM_TS_ITR2, TIM_COMMUTATION_TRGI);
}

void bldc_motor_six_step_algorithm(void)
{
	switch (bldc.step_number)
	{
		case 1:
		{
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_1);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_2);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_3);
		}
		break;

		case 2:
		{
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_1);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_2);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_3);
		}
		break;

		case 3:
		{
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_1);
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_2);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_3);
		}
		break;

		case 4:
		{
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_1);
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_2);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_3);
		}
		break;

		case 5:
		{
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_1);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_2);
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_3);
		}
		break;

		case 6:
		{
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_INACTIVE, TIM_CHANNEL_1);
			bldc_motor_OC_Config_Channel(TIM_OCMODE_FORCED_ACTIVE, TIM_CHANNEL_2);
			bldc_motor_PWM_Config_Channel(bldc.speed_pulse, TIM_CHANNEL_3);
		}
		break;

	}

	if(CW == bldc.dir)
	{
		bldc.step_number++;

		if(bldc.step_number > 6)
			bldc.step_number = 1;
	}
	else if(CCW == bldc.dir)
	{
		bldc.step_number--;

		if(bldc.step_number < 1)
			bldc.step_number = 6;
	}
}

void bldc_motor_set_speed(uint32_t speed, direction dir)
{
	if(speed > BLDC_MOTOR_MAX_SPEED)
	{
		bldc.speed_pulse = BLDC_MOTOR_MAX_SPEED;
	}
	else
	{
		bldc.speed_pulse = speed;
	}

	bldc.dir = dir;
}

void bldc_motor_PWM_Config_Channel(uint32_t pulse, uint32_t channel)
{
	bldc.sConfigOC.OCMode = TIM_OCMODE_PWM1;
	bldc.sConfigOC.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(bldc.tim_pwm, &bldc.sConfigOC, channel);

	HAL_TIM_PWM_Start(bldc.tim_pwm, channel);
	HAL_TIMEx_PWMN_Start(bldc.tim_pwm, channel);
}

void bldc_motor_OC_Config_Channel(uint32_t mode, uint32_t channel)
{
	bldc.sConfigOC.OCMode = mode;
	HAL_TIM_OC_ConfigChannel(bldc.tim_pwm, &bldc.sConfigOC, channel);

	HAL_TIM_OC_Stop(bldc.tim_pwm, channel);
	HAL_TIMEx_OCN_Start(bldc.tim_pwm, channel) ;
}
