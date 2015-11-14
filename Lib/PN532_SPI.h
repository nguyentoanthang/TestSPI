/*
 * Written by Nguyen Toan Thang
 * contact: 
 *			Phone: 0965021364
 * 			email: nguyentoanthang18@gmail.com
 * 
 */

#ifndef _PN532_SPI_H_
#define _PN532_SPI_H_

#include <bcm2835.h>
#include <stdio.h>

#define PN532_PREAMBLE      					0x00
#define PN532_STARTCODE1						0x00
#define PN532_STARTCODE2						0xFF
#define PN532_HOSTTOPN532   					0xD4
#define PN532_PN532TOHOST             		0xD5
#define PN532_POSTAMBLE     					0x00
#define PN532_COMMAND_GETFIRMWAREVERSION		0x02
#define PN532_INVALID_FRAME          		-3
#define PN532_ACK_WAIT_TIME           		10
#define PN532_INVALID_ACK             		-1
#define PN532_TIMEOUT                 		-2
#define PN532_NO_SPACE                		-4

#ifdef _cplusplus
extern "C" {
#endif
	/*
	 * Init SPI
	 */
	extern void begin(void);
	extern void end(void);
    extern void wakeup(void);
    /*
     * Write a command to PN532 and check ACK frame
     * Return 0 if success
     * 		  not 0 failed
     */
    extern int8_t writeCommand(uint8_t *header, int8_t hlen, uint8_t *body, int8_t blen);
    /*
     * Read a number of byte from PN532 and store them in buffer
     * timeout is the max time to wait, 0 mean no timeout
     * return length of byte without prefix and suffix: >= 0 success
     *   												< 0 failed
     */
    extern int16_t readResponse(uint8_t buf[], int8_t len, uint16_t timeout);
    /*
     * get Firmware Version of PN532 chip to ensure the connection of rpi and PN532
     * return
     */
    extern uint16_t getFirmwareVersion(uint8_t buf[]);

#ifdef _cplusplus
}
#endif

#endif
