/*
 * audio.c
 *
 *  Created on: Jun 2, 2021
 *      Author: pipat
 */

#include<cwsys/cwsys.h>
#include<cwsys/audio.h>

int isPlaying = -1;

#include<cwsys/audio_file.h>

void audio_update(TIM_HandleTypeDef *tim)
{
	if (isPlaying == -1)
		return;

	isPlaying+=2;
	if (isPlaying >= sizeof(audio_raw)/sizeof(audio_raw[0])) {
		HAL_TIM_Base_Stop_IT(tim);
		HAL_TIM_PWM_Stop(cwparam.audioPwm, TIM_CHANNEL_4);
		isPlaying = -1;
	}

	cwparam.audioPwm->Instance->CCR4 = audio_raw[isPlaying];
}

void audio_init()
{
	info("[AUDIO] Initializing");
	HAL_TIM_RegisterCallback(cwparam.audioTimer, HAL_TIM_PERIOD_ELAPSED_CB_ID, &audio_update);
}

void audio_play()
{
	if (isPlaying != -1)
		return;
	info("[AUDIO] Playing");
	isPlaying = 0;
	HAL_TIM_PWM_Start(cwparam.audioPwm, TIM_CHANNEL_4);
	HAL_TIM_Base_Start_IT(cwparam.audioTimer);
}


