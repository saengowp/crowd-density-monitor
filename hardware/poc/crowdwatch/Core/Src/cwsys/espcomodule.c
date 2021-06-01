/*
 * espcomodule.c
 *
 *  Created on: Jun 1, 2021
 *      Author: pipat
 */

#include "cwsys/cwsys.h"
#include "stm32f4xx_hal.h"

void spi_send_data(uint8_t com, uint8_t *buff, int l) // l < 32
{
	SPI_HandleTypeDef *spi = cwparam.espSpi;

	uint8_t data[32];
	data[0] = com;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	for (int i = 0; i < l && i < 31; i++) {
		data[1+i] = i < l ? buff[i] : 0;
	}

	const uint8_t writeH[] = {0x02, 0x00};

	if (HAL_SPI_Transmit(spi, writeH, 2, 10000) != HAL_OK) {
			info("Transmission Failed SPI");
	}

	if (HAL_SPI_Transmit(spi, data, 32, 10000) != HAL_OK) {
		info("Transmission Failed SPI");
	}


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}

int img_buffer_send(uint8_t *buff, int len)
{

	uint32_t t = HAL_GetTick();

	info("Transmiting to ESP");


	uint8_t *end = buff + len;
	uint8_t *cur = buff;

	while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) HAL_Delay(1);
	//if (HAL_GetTick() - t >= 10000) return -1;
	spi_send_data(0x01, NULL, 0);

	while (cur < end) {
		while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) HAL_Delay(1);
		//if (HAL_GetTick() - t >= 10000) return -1;
		int l = end - cur > 31 ? 31 : end - cur;
		spi_send_data(0x02, cur, l);
		cur += l;
		HAL_Delay(10);
		//while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET) HAL_Delay(1);
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	/*
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

		*/
}
