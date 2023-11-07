#ifndef __COMMON_H__869E7B00_67AB_446B_B145_B196865FAA26
#define __COMMON_H__869E7B00_67AB_446B_B145_B196865FAA26

#define XSKY_VERSION "1.0.0.0"

static const char * ROUTE_URL = "xsky.xsky.xsky";
static const unsigned short ROUTE_CONFIG_PORT = 0xABAB;
static const unsigned short ROUTE_CAPTURE_PORT = 0xBABA;

#define CHECKSUM_HW 1

#define TCPOPT_OP_V4 253
#define TCPOPT_OP_V6 254
#define TCPOPT_LEN_V4 8  /* |opcode|size|port|ipv4| = 1 + 1 + 2 + 4 */
#define TCPOPT_LEN_V6 20 /* |opcode|size|port|ipv6| = 1 + 1 + 2 + 16 */

#define IPV6_ALEN 16

#define ETH_ALEN 6
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_IPV6 0x86DD

#define IP_VERSION_4 0x0100
#define IP_VERSION_6 0x0110
#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17
#define IPV6_NEXTHDR_UDP 136

#define DNS_TYPE_A 1
#define DNS_TYPE_AAAA 28
#define DNS_CLASS_IN 1

#define DNS_FIXED_PORT 53
#define HTTP_FIXED_PORT 80
#define HTTPS_FIXED_PORT 443

enum command
{
    DROP,
    QUEUE,
    RESET,
    ACCEPT,
    SERVER,
};

typedef unsigned int ipv4_addr;

typedef union _ipv6_addr
{
    unsigned char addr8[IPV6_ALEN];
    unsigned short addr16[IPV6_ALEN / 2];
    unsigned int addr32[IPV6_ALEN / 4];
} ipv6_addr;

typedef struct _endpoint
{
    unsigned short port;
    union
    {
        ipv4_addr v4;
        ipv6_addr v6;
    }addr;
} endpoint;

typedef struct _mac_addr
{
    unsigned char mac[ETH_ALEN];
} mac_addr;

typedef struct _protocol_eth
{
    mac_addr dst;
    mac_addr src;
    unsigned short type;
} protocol_eth;

typedef struct _protocol_ipv4
{
    unsigned int version : 4;
    unsigned int ihl : 4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
} protocol_ipv4;

typedef struct _protocol_ipv6
{
    unsigned int version : 4;
    unsigned int priority : 8;
    unsigned int flow_lbl : 20;
    unsigned short payload_len;
    unsigned char nexthdr;
    unsigned char hop_limit;
    ipv6_addr saddr;
    ipv6_addr daddr;
} protocol_ipv6;

typedef struct _protocol_tcp
{
    unsigned short source;
    unsigned short dest;
    unsigned int seq;
    unsigned int ack_seq;
    unsigned short res1 : 4;
    unsigned short doff : 4;
    unsigned short fin : 1;
    unsigned short syn : 1;
    unsigned short rst : 1;
    unsigned short psh : 1;
    unsigned short ack : 1;
    unsigned short urg : 1;
    unsigned short res2 : 2;
    unsigned short window;
    unsigned short check;
    unsigned short urg_ptr;
} protocol_tcp;

typedef struct _protocol_udp
{
    unsigned short source;
    unsigned short dest;
    unsigned short len;
    unsigned short check;
} protocol_udp;

typedef struct _protocol_dns
{
    unsigned short tid;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
} protocol_dns;

#endif // !__COMMON_H__869E7B00_67AB_446B_B145_B196865FAA26
