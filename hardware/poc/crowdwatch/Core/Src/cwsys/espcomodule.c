/*
 * espcomodule.c
 *
 *  Created on: Jun 1, 2021
 *      Author: pipat
 */

#include "cwsys/cwsys.h"
#include "stm32f4xx_hal.h"

void spi_send_data(uint8_t com, uint8_t ordchk, uint8_t *buff, int l) // l <= 29
{
	SPI_HandleTypeDef *spi = cwparam.espSpi;

	uint8_t data[32];
	data[0] = com;
	data[1] = ordchk;
	data[2] = 0xAA ^ ordchk;


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

	for (int i = 0; i < l && i < 29; i++) {
		data[3+i] = i < l ? buff[i] : 0;
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

void img_buffer_send_init()
{
	while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) HAL_Delay(1);
		//if (HAL_GetTick() - t >= 10000) return -1;
		spi_send_data(0x01, 0x00, NULL, 0);
}

int img_buffer_send(uint8_t *buff, int len, int *ord)
{

	uint32_t t = HAL_GetTick();


	uint8_t *end = buff + len;
	uint8_t *cur = buff;



	//uint8_t i = 0;
	while (cur < end) {
		while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) HAL_Delay(1);
		//if (HAL_GetTick() - t >= 10000) return -1;
		int l = end - cur > 12 ? 12 : end - cur;
			spi_send_data(0x02, *ord, cur, l);
			HAL_Delay(10);
		(*ord)++;
		cur += l;

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

uint32_t spi_get_status()
{
	SPI_HandleTypeDef *spi = cwparam.espSpi;
	uint8_t data[32];

	info("Status read start");
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	data[0] = 0x04;
	if (HAL_SPI_Transmit(spi, data, 1, 10000) != HAL_OK)
		info("Read Transmission Failed");
	HAL_SPI_Receive(spi, data, 4, 10000);
	uint32_t *v = ((uint32_t*) data);
	info("Got %u", *v);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	return *v;
}
