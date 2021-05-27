/*
 * cwsys.c
 *
 *  CrowdWatch System Main
 *
 *  Created on: May 27, 2021
 *      Author: pipat
 */

#include "cwsys.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static struct cwsys_init_params cwparam;

void cwsys(struct cwsys_init_params params)
{
	cwparam = params;

	info("[CWSYS] Crowd Watch System initializing. t=+%dms\r\n", HAL_GetTick());
}

void info(const char *fmt, ...)
{
	const size_t buffSize = 128;

	char buff[buffSize];

	va_list args;
	va_start(args, fmt);
	int l = vsniprintf(buff, buffSize, fmt, args);
	if (HAL_UART_Transmit(cwparam.infoSerial, (uint8_t*) buff, l, 1000) != HAL_OK) {
		char *warnstr = "ERROR: Log UART transmission failed";
		HAL_UART_Transmit(cwparam.infoSerial, (uint8_t*) warnstr, strlen(warnstr), 1000);
	}
}
