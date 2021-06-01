/*
 * espcomodule.c
 *
 *  Created on: Jun 1, 2021
 *      Author: pipat
 */

#include "cwsys/cwsys.h"
#include "stm32f4xx_hal.h"

int img_buffer_send()
{
	info("Transmiting to ESP");
	SPI_HandleTypeDef *spi = cwparam.espSpi;
	uint8_t data[32+2] = {0x02, 0x00};
	for (int i = 2; i < 32+2; i++)
		data[i] = i;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(spi, data, 32+2, 10000) == HAL_OK) {
		info("Transmitted");
	} else {
		info("Transmission Failed");
	}

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	info("Status read start");
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	data[0] = 0x04;
	if (HAL_SPI_Transmit(spi, data, 1, 10000) != HAL_OK)
		info("Read Transmission Failed");
	HAL_SPI_Receive(spi, data, 4, 10000);
	info("Got %u", *((uint32_t*) data));
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}
