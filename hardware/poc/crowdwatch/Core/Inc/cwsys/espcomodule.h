/*
 * espcomodule.h
 *
 *  Created on: Jun 1, 2021
 *      Author: pipat
 */

#ifndef INC_CWSYS_ESPCOMODULE_H_
#define INC_CWSYS_ESPCOMODULE_H_

void img_buffer_send_init();
int img_buffer_send(uint8_t *buff, int len, int *ord);

#endif /* INC_CWSYS_ESPCOMODULE_H_ */
