#include <PN532_SPI.h>
#include <PN532_debug.h>
#include <string.h>

/*
 * define status byte have to send to PN532 before send or read data
 */
#define DATA_WRITE		1
#define STATUS_READ		2
#define DATA_READ    	3

uint8_t PN532_packgeBuffer[64];
uint8_t pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};
uint8_t command;
uint8_t response[12];

void begin(void) {

	if(!bcm2835_init()) {
		return;
	}

    bcm2835_spi_begin();
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST);      // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
    bcm2835_gpio_fsel(17, BCM2835_GPIO_FSEL_OUTP);
}

void wakeup(void) {
	bcm2835_gpio_write(17, LOW);
	bcm2835_delay(2);
	bcm2835_gpio_write(17, HIGH);
}

void write(uint8_t data) {
	data = (data * 0x0202020202ULL & 0x010884422010ULL) % 1023;
    bcm2835_spi_transfer(data);
}

uint8_t read() {
	uint8_t data = bcm2835_spi_transfer(0);
	data = (data * 0x0202020202ULL & 0x010884422010ULL) % 1023;
	return data;
}

/*
 * Read a ACK frame
 * return 0 if recieve success
 *        not 0 if failed
 */
int8_t readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};   // an ACK frame

    uint8_t ackBuf[sizeof(PN532_ACK)];
    bcm2835_gpio_write(17, LOW);
    bcm2835_delay(1);
    bcm2835_spi_transfer(DATA_READ);
    uint8_t i;
    for (i = 0; i < sizeof(PN532_ACK); i++) {
        ackBuf[i] = read();
    }
    bcm2835_gpio_write(17, HIGH);
    bcm2835_delay(1);
    return memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK));
}

/*
 * Write a frame to PN532
 */
void writeFrame(uint8_t* header, int8_t hlen, uint8_t* body, int8_t blen) {

	bcm2835_gpio_write(17, LOW);
	bcm2835_delay(1);
	bcm2835_spi_transfer(DATA_WRITE);			// write a status byte to PN532
	bcm2835_spi_transfer(PN532_PREAMBLE);	// write a Preamble byte
	bcm2835_spi_transfer(PN532_STARTCODE1);	// write a start code byte
	bcm2835_spi_transfer(PN532_STARTCODE2);

	int8_t length = hlen + blen + 1;			// length of data to write

	bcm2835_spi_transfer(length);			// write a length
	bcm2835_spi_transfer(~length + 1);		// write a length checksum, (LCS + LEN) = 0x00
	bcm2835_spi_transfer(PN532_HOSTTOPN532);	// write byte frame identifier

	uint8_t sum = PN532_HOSTTOPN532;			// data checksum byte

	DMSG_STR("Write: ");

	uint8_t i, j;
	for (i = 0; i < hlen; i++) {
        bcm2835_spi_transfer(header[i]);
        sum += header[i];

        DMSG_HEX(header[i]);
    }
    for (j = 0; i < blen; j++) {
        bcm2835_spi_transfer(body[j]);
        sum += body[j];

        DMSG_HEX(body[j]);
    }

    uint8_t checksum = ~sum + 1;         // checksum of TFI + DATA
    bcm2835_spi_transfer(checksum);		// write checksum byte
    bcm2835_spi_transfer(PN532_POSTAMBLE);
    bcm2835_gpio_write(17, HIGH);
    bcm2835_delay(1);
    DMSG_STR("\n");
}

/*
 * Wait for Pn532 say ready
 */
uint8_t isReady(void) {
	bcm2835_gpio_write(17, LOW);
	bcm2835_delay(1);
    bcm2835_spi_transfer(STATUS_READ);
    uint8_t status = read() & 1;
    bcm2835_gpio_write(17, HIGH);
    bcm2835_delay(1);
    return status;
}

uint16_t getFirmwareVersion(uint8_t buf[]) {

    PN532_packgeBuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

    if (!writeCommand(PN532_packgeBuffer, 1, 0, 0)) {
        return 0;
    }

    uint16_t len;

    // read data packet
    len = readResponse(buf, 12, 1000);
/*
    // check some basic stuff
    if (0 != strncmp((char *)PN532_packgeBuffer, (char *)pn532response_firmwarevers, 6)) {
        //#ifdef PN532DEBUG
        Serial.println(F("Firmware doesn't match!"));
        //#endif
        return 0;
    }

    // int offset = 6;
    // response = PN532_packgeBuffer[offset++];
    // response <<= 8;
    // response |= PN532_packgeBuffer[offset++];
    // response <<= 8;
    // response |= PN532_packgeBuffer[offset++];
    // response <<= 8;
    // response |= PN532_packgeBuffer[offset++];
*/
    return len;
}

int8_t writeCommand(uint8_t *header, int8_t hlen, uint8_t *body, int8_t blen) {

	command = header[0];
	writeFrame(header, hlen, body, blen);

    uint8_t timeout = PN532_ACK_WAIT_TIME;
    
    while (isReady() == 0) {
        bcm2835_delay(1);
        timeout--;
        if (timeout == 0) {
            DMSG_STR("Time out when waiting for ACK\n");
            return -2;
        }
    }
    
    if (readAckFrame()) {
        DMSG_STR("Invalid ACK\n");
        return PN532_INVALID_ACK;
    }

    return 0;
}

int16_t readResponse(uint8_t buf[], int8_t len, uint16_t timeout) {
    
    uint16_t time = 0;
    
    while (isReady() == 0) {
        bcm2835_delay(1);
        time++;
        if (time > timeout) {
            return PN532_TIMEOUT;
        }
    }


	bcm2835_gpio_write(17, LOW);
	bcm2835_delay(1);
    int16_t result;

    bcm2835_spi_transfer(DATA_WRITE);


    if (0x00 != read()     ||       // PREAMBLE
            0x00 != read()  ||       // STARTCODE1
            0xFF != read()           // STARTCODE2
        ) {

        result = PN532_INVALID_FRAME;
        return result;
    }

    uint8_t length = read();
    if (0 != (uint8_t)(length + read())) {   // checksum of length
        result = PN532_INVALID_FRAME;
        return result;
    }

    uint8_t cmd = command + 1;               // response command
    if (PN532_PN532TOHOST != read() || (cmd) != read()) {
        result = PN532_INVALID_FRAME;
        return result;
    }

    DMSG_STR("read:  ");
    DMSG_HEX(cmd);

    length -= 2;
    if (length > len) {
        uint8_t i;
        for (i = 0; i < length; i++) {
            DMSG_HEX(read());                 // dump message
        }
        DMSG_STR("\nNot enough space\n");
        read();
        read();
        result = PN532_NO_SPACE;  // not enough space
        return result;
    }

    uint8_t sum = PN532_PN532TOHOST + cmd;
    uint8_t j;
    for (j = 0; j < length; j++) {
        buf[j] = read();
        sum += buf[j];

        DMSG_HEX(buf[j]);
    }
    DMSG_STR("\n");

    uint8_t checksum = read();
    if (0 != (uint8_t)(sum + checksum)) {
        DMSG_STR("checksum is not OK\n");
        result = PN532_INVALID_FRAME;
        return result;
    }
    read();         // POSTAMBLE

    result = length;
    DMSG_DEC(result);
	bcm2835_gpio_write(17, HIGH);
	bcm2835_delay(1);
    return result;
}

void end(void) {
	bcm2835_spi_end();
	bcm2835_close();
}
