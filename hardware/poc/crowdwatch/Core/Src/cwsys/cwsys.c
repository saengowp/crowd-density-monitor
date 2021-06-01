/*
 * cwsys.c
 *
 *  CrowdWatch System Main
 *
 *  Created on: May 27, 2021
 *      Author: pipat
 */

#include "cwsys/cwsys.h"
#include "cwsys/camera.h"
#include "cwsys/espcomodule.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

int cwsys_loop();

struct cwsys_init_params cwparam;

void cwsys(struct cwsys_init_params params)
{
	cwparam = params;
	int e;

	info("[CWSYS] Crowd Watch System initializing.");
	/*
	e = camera_init();
	if (e != CWSYS_OK) {
		info("[CWSYS] Failed to init camera with error code = %d", e);
		return;
	}
	info("[CWSYS] Crowd Watch System initialized. Starting loop.");
*/
	while (1) {
		e = cwsys_loop();
		if (e != CWSYS_OK) {
			info("[CWSYS] Main Control Loop failed with error=%d", e);
			return;
		}
	}
}

int cwsys_loop() {
/*
	camera_capture();
	while (camBuffer.state != CAPTURED);
	info("Captured %d lines, first 2 line is %d %d, clk = %d", camBuffer.hsyncSz, camBuffer.hsync[0], camBuffer.hsync[1], cwparam.cameraParams.tim1Sync->Instance->CNT);
	if (camBuffer.hsyncSz < 144) {
		info("[CAM] Bad frame, Check connection");
		return CWSYS_OK;
	}

	*/
	/*
	info("Writing out img buf, TRIGTRIGTRIG");
	for (int i = 0; i < 144; i++) {
		HAL_UART_Transmit(cwparam.infoSerial, camBuffer.img + camBuffer.hsync[i], 348, 10000);
	}
	*/

	for (int i = 0; i < IMG_BUFFER_SIZE; i++) {
		camBuffer.img[i] = 'A' + (i%26);
	}
	img_buffer_send(camBuffer.img, IMG_BUFFER_SIZE);
	HAL_Delay(1000);
	return CWSYS_OK;
}

void info(const char *fmt, ...)
{
	const size_t buffSize = 128;

	char buff[buffSize];

	va_list args;
	va_start(args, fmt);
	int l = vsniprintf(buff, buffSize-2, fmt, args);
	buff[l++] = '\r';
	buff[l++] = '\n';
	if (HAL_UART_Transmit(cwparam.infoSerial, (uint8_t*) buff, l, 1000) != HAL_OK) {
		char *warnstr = "ERROR: Log UART transmission failed";
		HAL_UART_Transmit(cwparam.infoSerial, (uint8_t*) warnstr, strlen(warnstr), 1000);
	}
}
