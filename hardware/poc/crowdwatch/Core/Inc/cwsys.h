/*
 * cwsys.h
 *
 * Interface for initialization and control of CrowdWatch system.
 *
 *  Created on: May 27, 2021
 *      Author: pipat
 */

#ifndef INC_CWSYS_H_
#define INC_CWSYS_H_

#include "stm32f4xx_hal.h"

/**
 * Parameter for initializing CWSYS
 */
struct cwsys_init_params {
	UART_HandleTypeDef *infoSerial; //UART to log info
};

/**
 * Initialize CWSYS and run the control loop
 */
void cwsys(struct cwsys_init_params params);

/**
 * Logging to serial terminal. Blocking.
 */
void info(const char *fmt, ...);


#endif /* INC_CWSYS_H_ */
