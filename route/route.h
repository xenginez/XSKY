#ifndef __NET__SKY_H__
#define __NET__SKY_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/err.h>
#include <linux/time.h>
#include <linux/skbuff.h>
#include <net/tcp.h>
#include <net/inet_common.h>
#include <linux/uaccess.h>
#include <linux/netdevice.h>
#include <net/net_namespace.h>
#include <linux/fs.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/kallsyms.h>
#include <net/ipv6.h>
#include <net/transp_v6.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/net.h>
#include <net/ip.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <net/protocol.h>
#include <linux/if_vlan.h>

#include "../common/common.h"
#include "../common/hash_map.h"
#include "../common/protocol.h"

#define SKY_DBG(msg...)	do { printk(KERN_DEBUG "[DEBUG] SKY: " msg); } while (0)

typedef struct _flags
{
	unsigned char arp : 1;
	unsigned char icmp : 1;
	unsigned char tcp : 1;
	unsigned char udp : 1;
	unsigned char dns : 1;
	unsigned char http : 1;
	unsigned char capture : 2;
} flags;

typedef struct _capture_info
{
	mac_addr mac;
	endpoint addr;
	struct
	{

	} arp;
	struct
	{

	} ip;
	struct
	{

	} icmp;
	struct
	{

	} tcp;
	struct
	{

	} udp;
	struct
	{

	} dns;
	struct
	{

	} http;

} capture_info;

struct route_device
{
	flags flag;
	endpoint server;
	capture_info capture;
	struct hash_map * routes;
};

#define NIPV4(addr) ((unsigned char *)&addr)[0], ((unsigned char *)&addr)[1], ((unsigned char *)&addr)[2], ((unsigned char *)&addr)[3]
#define NIPV6(addr) ((unsigned short *)addr)[0], ((unsigned short *)addr)[1], ((unsigned short *)addr)[2], ((unsigned short *)addr)[3], ((unsigned short *)addr)[4], ((unsigned short *)addr)[5], ((unsigned short *)addr)[6], ((unsigned short *)addr)[7]

#endif
