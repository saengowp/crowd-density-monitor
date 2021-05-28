/*
 * camera.c
 *
 *  Created on: May 28, 2021
 *      Author: pipat
 */


#include "cwsys/camera.h"
#include "cwsys/cwsys.h"
#include "cwsys/camera_blob.h"

void camsync_tim_oncapture(TIM_HandleTypeDef *tim);

int camera_sccb_init();

int camera_init()
{
	struct cameraparams *params = &cwparam.cameraParams;
	int e;
	info("[CAM] Initializing.");

	camBuffer.state = IDLE;


	// Generate XCLK
	e = HAL_TIM_PWM_Start(params->camClkPwm, TIM_CHANNEL_1);
	if (e != HAL_OK) {
		info("[CAM] ERR: Can't start clk");
		return e;
	}

	// DMA
	e = HAL_DMA_Start(params->pixdma, (uint32_t) params->pixDataGPIOIdr, (uint32_t) camBuffer.img, IMG_BUFFER_SIZE);
	if (e != HAL_OK) {
		info("[CAM] ERR: Can't start DMA");
		return e;
	}

	// Timer
	__HAL_TIM_ENABLE_DMA(params->tim1Sync, TIM_DMA_CC1);
	e = HAL_TIM_RegisterCallback(params->tim3Sync, HAL_TIM_IC_CAPTURE_CB_ID, &camsync_tim_oncapture);
	if (e != HAL_OK) {
			info("[CAM] ERR: Can't register timer ic handler");
			return e;
	}

	// Check CLK
	int iT = HAL_GetTick();
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
		if (HAL_GetTick() - iT > 1000) break;
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 1)
		if (HAL_GetTick() - iT > 1000) break;
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
		if (HAL_GetTick() - iT > 1000) break;

	if (HAL_GetTick() - iT > 1000) {
		info("[CAM] ERR: No PCLK");
		return -1;
	}
	info("[CAM] PCLK wave detected OK");

	// Configure Camera
	e = camera_sccb_init();
	if (e != CWSYS_OK) {
		info("[CAM] ERR: Can't configure camera using SCCB");
		return e;
	}

	return CWSYS_OK;
}

int camera_sccb_init()
{
	int e;
	uint8_t *reg = (uint8_t*) ov7670_regsval;
	while (*reg != 0xFF || *(reg+1) != 0xFF) {
		int try = 0;
		while ((e = HAL_I2C_Master_Transmit(cwparam.cameraParams.sccb, SCCB_ADDR, reg, 2, 1000)) != HAL_OK) {
			try++;
			info("[CAM] SCCB: WARN: Error while writing register no. %d, Restarting", (reg- (uint8_t*) ov7670_regsval)/2);
			HAL_I2C_DeInit(cwparam.cameraParams.sccb);
			HAL_Delay(10);
			HAL_I2C_Init(cwparam.cameraParams.sccb);
			if (try > 10) {
				break;
			}
		}

		if (e != HAL_OK) {
			info("[CAM] SCCB: ERR: Error while writing register no. %d", (reg- (uint8_t*) ov7670_regsval)/2);
			return e;
		}
		HAL_Delay(10);
		reg += 2;
	}

	return CWSYS_OK;
}

int vsyncCaptured;

void camsync_tim_oncapture(TIM_HandleTypeDef *tim)
{
	if (camBuffer.state != CAPTURING) return;

	switch (tim->Channel)
	{
	case HAL_TIM_ACTIVE_CHANNEL_1: // VSYNC
		if (vsyncCaptured) {
			/*
			 * There supose to be an empty frame here, but there's none for some reason.
			if (camBuffer.imgStart == IMG_BUFFER_SIZE - cwparam.cameraParams.pixdma->Instance->NDTR) {
				//Probably captured trailing vsync, recapture
				return;
			}
			*/
			// Stop capturing
			HAL_TIM_IC_Stop(cwparam.cameraParams.tim1Sync, TIM_CHANNEL_1);
			HAL_TIM_IC_Stop_IT(tim, TIM_CHANNEL_1);
			HAL_TIM_IC_Stop_IT(tim, TIM_CHANNEL_2);
			camBuffer.state = CAPTURED;

		} else {
			// Capture offset
			vsyncCaptured = 1;
			int offset = IMG_BUFFER_SIZE - cwparam.cameraParams.pixdma->Instance->NDTR;
			camBuffer.imgStart = offset;
			cwparam.cameraParams.tim3Sync->Instance->CNT = offset;
			cwparam.cameraParams.tim3Sync->Instance->ARR = IMG_BUFFER_SIZE-1;
		}
		break;
	case HAL_TIM_ACTIVE_CHANNEL_2: //HSYNC
		if (camBuffer.hsyncSz < IMG_HSYNC_LEN)
			camBuffer.hsync[camBuffer.hsyncSz] = cwparam.cameraParams.tim3Sync->Instance->CCR2;
		camBuffer.hsyncSz++;
		break;
	default:
		break;
	}
}

int camera_capture()
{
	info("[CAM] Start Capturing.");
	struct cameraparams *params = &cwparam.cameraParams;
	int e;

	vsyncCaptured = 0;
	camBuffer.hsyncSz = 0;

	//VSYNC
	e = HAL_TIM_IC_Start_IT(params->tim3Sync, TIM_CHANNEL_1);
	if (e != HAL_OK) {
		info("[CAM] ERR: Can't start tim3 IC CH1");
		return e;
	}

	//HSYNC
	e = HAL_TIM_IC_Start_IT(params->tim3Sync, TIM_CHANNEL_2);
	if (e != HAL_OK) {
		info("[CAM] ERR: Can't start tim3 IC CH2");
		return e;
	}

	//Start DMA clock
	e = HAL_TIM_IC_Start(params->tim1Sync, TIM_CHANNEL_1);
	if (e != HAL_OK) {
		info("[CAM] ERR: Can't start sync clk");
		return e;
	}

	camBuffer.state = CAPTURING;

	return CWSYS_OK;
}

