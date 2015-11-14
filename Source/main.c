/*
 * main.c
 *
 *  Created on: Oct 27, 2015
 *      Author: mac
 */

#include <PN532_SPI.h>
#include <PN532_debug.h>
int main(int argc, char* argv[]) {
	uint8_t buf[12];
	uint16_t len;
	printf("Get Firmware Version\n");
	begin();
	len = getFirmwareVersion(buf);
	printf("%d", len);
	end();
	return 0;
}


