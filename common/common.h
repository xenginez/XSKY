#ifndef __COMMON_H__
#define __COMMON_H__

#define XSKY_VERSION "1.0.0.0"

static const unsigned short CONFIG_PORT = 33333;
static const unsigned short CAPTURE_PORT = 44444;
static const char * ROUTE_ADDRESS_STR = "3.3.3.3";
static const unsigned int ROUTE_ADDRESS_NUM = 0x03030303;

#define CHECKSUM_HW 1

#define TCPOPT_V4_SKY  253
#define TCPOLEN_V4_SKY 8		/* |opcode|size|port|ipv4| = 1 + 1 + 2 + 4 */

#define TCPOPT_V6_SKY  254
#define TCPOLEN_V6_SKY 20		/* |opcode|size|port|ipv6| = 1 + 1 + 2 + 16 */

#endif // !__COMMON_H__
