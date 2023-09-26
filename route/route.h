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
#include <linux/if_ether.h>
#include <net/protocol.h>
#include <linux/if_vlan.h>

#ifdef __CMAKE_PROGRAM
#define __exit
#define module_exit( F )
#define MODULE_LICENSE( F )
typedef struct { int pte; }pte_t;
#define _PAGE_RW 0
#endif // __CMAKE_PROGRAM


#define SKY_VERSION "1.0.0.0"

#define SKY_DBG(msg...)	do { printk(KERN_DEBUG "[DEBUG] SKY: " msg); } while (0)

#define CHECKSUM_HW 1

#define SKY_DATA_PAIR_SIZE 100
#define SKY_DATA_BUCKET_SIZE 1000

#define TCPOPT_V4_SKY  253
#define TCPOLEN_V4_SKY 8		/* |opcode|size|port|ipv4| = 1 + 1 + 2 + 4 */

#define TCPOPT_V6_SKY  254
#define TCPOLEN_V6_SKY 20		/* |opcode|size|port|ipv6| = 1 + 1 + 2 + 16 */

#define TCPOPT_FLAG_SKY 11111

#define FILE_NAME_SKY "sky_dev"
#define CMD_ROUTE_SKY  _IOW('d', 3, unsigned int)

struct sky_v4_data
{
	__u8 code;
	__u8 size;
	__u16 port;
	__u32 addr;
};
struct sky_v6_data
{
	__u8 opcode;
	__u8 opsize;
	__u16 port;
	union
	{
		__u16 ip[8];
		__u32 wip[4];
	};
};
typedef union
{
	struct sky_v4_data v4;
	struct sky_v6_data v6;
}sky_data;

struct sky_data_pair
{
	sky_data first;
	sky_data second;
};

struct sky_data_page
{
	struct sky_data_page * next;

	unsigned long long used;
	struct sky_data_pair pairs[SKY_DATA_PAIR_SIZE];
};

struct sky_data_bucket
{
	struct sky_data_page * pages;
};

struct sky_device
{
	int chrdev;
	struct class * cls;
	struct device * dev;
	unsigned int servers;
	struct sky_data_bucket * routes;
};

#define NIPV4(addr) ((unsigned char *)&addr)[0], ((unsigned char *)&addr)[1], ((unsigned char *)&addr)[2], ((unsigned char *)&addr)[3]
#define NIPV6(addr) ((unsigned short *)addr)[0], ((unsigned short *)addr)[1], ((unsigned short *)addr)[2], ((unsigned short *)addr)[3], ((unsigned short *)addr)[4], ((unsigned short *)addr)[5], ((unsigned short *)addr)[6], ((unsigned short *)addr)[7]

#endif
