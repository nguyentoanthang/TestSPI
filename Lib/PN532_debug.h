#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

#include <stdio.h>

#ifdef DEBUG
	#define DMSG_DEC(num)       printf("%d ", num)
	#define DMSG_STR(str)       printf("%s", str)
	#define DMSG_HEX(num_hex)   printf("0x%02X ", num_hex)
#else
	#define DMSG_STR(str)
	#define DMSG_HEX(num)
	#define DMSG_DEC(num)
#endif

#endif
