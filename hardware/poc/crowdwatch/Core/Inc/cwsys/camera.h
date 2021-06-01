/*
 * camera.h
 *
 * OV7670 QCIF mode image capturing
 *
 *  Created on: May 28, 2021
 *      Author: pipat
 */

#ifndef INC_CWSYS_CAMERA_H_
#define INC_CWSYS_CAMERA_H_

#include "stm32f4xx_hal.h"

/**
 * Camera buffer and IO configuration
 *
 * I/O Configuration:
 * - I2C for SCCB camera control
 *
 * Since GPIO on AHB1 bus only connected to DMA2 and DMA2 only connected to TIM1
 * We need to configure TIM1 to use PCLK rising edge on CH1 as an external clock.
 * DMA2 should be configured to trigger on TIM1_UP (RCR=0) from peripheral to memory in circular mode
 * TIM1 Should be reset by PCLK IC on CH1
 * TIM2 CH1 should generate XCLK ARR=7, PULSE=4 at 10.5MHz
 * TIM3 Should be TIM1 Slave tracking Buffer size, CH1 IC VSYNC, CH2 IC HSYNC
 * It should also detect both edge for unknown reason. Otherwise it seems to capture 2 frames
 * GPIOIdr lower byte should be sensor data pin 0-7
 *
 * Cam Reset = High, PWDN = Low
 */
struct cameraparams
{
	TIM_HandleTypeDef *tim1Sync;
	TIM_HandleTypeDef *tim3Sync;
	TIM_HandleTypeDef *camClkPwm;
	DMA_HandleTypeDef *pixdma;
	I2C_HandleTypeDef *sccb;
	uint32_t *pixDataGPIOIdr;
};

/**
 * Initialize the camera.
 * return 0 if OK
 */
int camera_init();

#define IMG_BUFFER_SIZE (174*2*144)
#define IMG_HSYNC_LEN 150

struct camerabuffer {
	uint8_t img[IMG_BUFFER_SIZE];
	size_t hsync[IMG_HSYNC_LEN];
	size_t hsyncSz;
	size_t imgStart;
	enum {IDLE, CAPTURING, CAPTURED} state;
};

extern struct camerabuffer camBuffer;

/**
 * Capture an image to camBuffer
 */
int camera_capture();

#endif /* INC_CWSYS_CAMERA_H_ */
