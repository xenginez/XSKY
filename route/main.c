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
#include <linux/icmp.h>
#include <linux/icmpv6.h>

#include "common/common.h"
#include "common/common.c"

#ifdef _WIN32
#define __init
#define __exit
#define module_init()
#define module_exit()
#define MODULE_LICENSE()
#define SKY_DBG(...)	do { printf(KERN_DEBUG "[DEBUG] SKY: " __VA_ARGS__); } while (0)
#else
#define SKY_DBG(msg...)	do { printk(KERN_DEBUG "[DEBUG] SKY: " msg); } while (0)
#endif // _WIN32

#define NIPV4(addr) ((unsigned char *)&addr)[0], ((unsigned char *)&addr)[1], ((unsigned char *)&addr)[2], ((unsigned char *)&addr)[3]
#define NIPV6(addr) ((unsigned short *)addr)[0], ((unsigned short *)addr)[1], ((unsigned short *)addr)[2], ((unsigned short *)addr)[3], ((unsigned short *)addr)[4], ((unsigned short *)addr)[5], ((unsigned short *)addr)[6], ((unsigned short *)addr)[7]


static bool skb_is_get_dns( struct sk_buff * skb );
static bool skb_is_set_config( struct sk_buff * skb );
static bool skb_is_get_config( struct sk_buff * skb );
static bool skb_is_set_capture( struct sk_buff * skb );
static bool skb_is_get_capture( struct sk_buff * skb );
static bool skb_is_client_to_server( struct sk_buff * skb );
static bool skb_is_server_to_client( struct sk_buff * skb );

static unsigned int skb_recv_dns( struct sk_buff * skb );
static unsigned int skb_recv_client( struct sk_buff * skb );
static unsigned int skb_recv_server( struct sk_buff * skb );
static unsigned int skb_recv_config( struct sk_buff * skb );
static unsigned int skb_recv_capture( struct sk_buff * skb );

static unsigned int skb_sendto_dns( struct sk_buff * skb );
static unsigned int skb_sendto_client( struct sk_buff * skb );
static unsigned int skb_sendto_server( struct sk_buff * skb );
static unsigned int skb_sendto_config( struct sk_buff * skb );
static unsigned int skb_sendto_capture( struct sk_buff * skb );

static unsigned int route_arp_handle( struct sk_buff * skb );
static unsigned int route_ipv4_handle( struct sk_buff * skb );
static unsigned int route_ipv6_handle( struct sk_buff * skb );
static unsigned int route_icmpv4_handle( struct sk_buff * skb );
static unsigned int route_icmpv6_handle( struct sk_buff * skb );
static unsigned int route_tcp_handle( struct sk_buff * skb );
static unsigned int route_udp_handle( struct sk_buff * skb );
static unsigned int route_dns_handle( struct sk_buff * skb );
static unsigned int route_http_handle( struct sk_buff * skb );
static unsigned int route_https_handle( struct sk_buff * skb );

struct
{
	endpoint server;
	config_info config;
	capture_info capture;
	struct hash_map * hash_map;
} _device;

static bool skb_is_get_dns( struct sk_buff * skb )
{
	return false;
}
static bool skb_is_set_config( struct sk_buff * skb )
{
	return false;
}
static bool skb_is_get_config( struct sk_buff * skb )
{
	return false;
}
static bool skb_is_set_capture( struct sk_buff * skb )
{
	return false;
}
static bool skb_is_get_capture( struct sk_buff * skb )
{
	return _device.config.capture != 0;
}
static bool skb_is_client_to_server( struct sk_buff * skb )
{
	return false;
}
static bool skb_is_server_to_client( struct sk_buff * skb )
{
	return false;
}

static unsigned int skb_recv_dns( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_recv_client( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );

	if( eth_h->h_proto == htons( ETH_TYPE_ARP ) && _device.config.arp != 0 )
	{
		return route_arp_handle( skb );
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		if( _device.config.icmp != 0 || _device.config.tcp != 0 || _device.config.udp != 0 || _device.config.dns != 0 || _device.config.http != 0 )
		{
			return route_ipv4_handle( skb );
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		if( _device.config.icmp != 0 || _device.config.tcp != 0 || _device.config.udp != 0 || _device.config.dns != 0 || _device.config.http != 0 )
		{
			return route_ipv6_handle( skb );
		}
	}

	return NF_ACCEPT;
}
static unsigned int skb_recv_server( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_recv_config( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_recv_capture( struct sk_buff * skb )
{
	return NF_ACCEPT;
}

static unsigned int skb_sendto_dns( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_sendto_client( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_sendto_server( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_sendto_config( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int skb_sendto_capture( struct sk_buff * skb )
{
	return NF_ACCEPT;
}

static unsigned int route_arp_handle( struct sk_buff * skb )
{
	struct arphdr * arp_h = arp_hdr( skb );

	return NF_ACCEPT;
}
static unsigned int route_ipv4_handle( struct sk_buff * skb )
{
	struct iphdr * ip_h = ip_hdr( skb );

	if( ip_h->protocol == htons( IP_PROTO_TCP ) )
	{
		return route_tcp_handle( skb );
	}
	else if( ip_h->protocol == htons( IP_PROTO_UDP ) )
	{
		return route_udp_handle( skb );
	}
	else if( ip_h->protocol == htons( IP_PROTO_ICMP ) )
	{
		return route_icmpv4_handle( skb );
	}

	return NF_ACCEPT;
}
static unsigned int route_ipv6_handle( struct sk_buff * skb )
{
	struct ipv6hdr * ip_h = ipv6_hdr( skb );

	if( ip_h->nexthdr == htons( IPV6_NEXTHDR_TCP ) )
	{
		return route_tcp_handle( skb );
	}
	else if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) )
	{
		return route_udp_handle( skb );
	}
	else if( ip_h->nexthdr == htons( IPV6_NEXTHDR_ICMP ) )
	{
		return route_icmpv6_handle( skb );
	}

	return NF_ACCEPT;
}
static unsigned int route_icmpv4_handle( struct sk_buff * skb )
{
	struct icmphdr * icmp_h = icmp_hdr( skb );

	return NF_ACCEPT;
}
static unsigned int route_icmpv6_handle( struct sk_buff * skb )
{
	struct icmp6hdr * icmp_h = icmp6_hdr( skb );
	
	return NF_ACCEPT;
}
static unsigned int route_tcp_handle( struct sk_buff * skb )
{
	struct tcphdr * tcp_h = tcp_hdr( skb );

	if( tcp_h->source == htons( HTTP_FIXED_PORT ) || tcp_h->dest == htons( HTTP_FIXED_PORT ) )
	{
		return route_http_handle( skb );
	}
	else if( tcp_h->source == htons( HTTPS_FIXED_PORT ) || tcp_h->dest == htons( HTTPS_FIXED_PORT ) )
	{
		return route_https_handle( skb );
	}
	else
	{

	}

	return NF_ACCEPT;
}
static unsigned int route_udp_handle( struct sk_buff * skb )
{
	struct udphdr * udp_h = udp_hdr( skb );

	if( udp_h->source == htons( DNS_FIXED_PORT ) || udp_h->dest == htons( DNS_FIXED_PORT ) )
	{
		return route_dns_handle( skb );
	}
	else
	{

	}

	return NF_ACCEPT;
}
static unsigned int route_dns_handle( struct sk_buff * skb )
{
	struct udphdr * udp_h = udp_hdr( skb );

	protocol_dns * dns_h = (protocol_dns *)( udp_h + 1 );

	return NF_ACCEPT;
}
static unsigned int route_http_handle( struct sk_buff * skb )
{
	return NF_ACCEPT;
}
static unsigned int route_https_handle( struct sk_buff * skb )
{
	return NF_ACCEPT;
}

static unsigned int route_nf_handle( void * priv, struct sk_buff * skb, const struct nf_hook_state * state )
{
	if( skb_is_get_dns( skb ) )
	{
		return skb_recv_dns( skb );
	}

	if( skb_is_set_config( skb ) )
	{
		return skb_recv_config( skb );
	}
	
	if( skb_is_get_config( skb ) )
	{
		return skb_sendto_config( skb );
	}
	
	if( skb_is_set_capture( skb ) )
	{
		return skb_recv_capture( skb );
	}
	
	if( skb_is_get_capture( skb ) )
	{
		skb_sendto_capture( skb );
	}

	if( _device.config.proxy != 0 )
	{
		if( skb_is_client_to_server( skb ) )
		{
			return skb_recv_client( skb );
		}
		else if( skb_is_server_to_client( skb ) )
		{
			return skb_recv_server( skb );
		}
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops route_nf_hook_ops[] =
{
	{
		.hook = route_nf_handle,
		.pf = NFPROTO_ARP,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FIRST,
	},
	{
		.hook = route_nf_handle,
		.pf = NFPROTO_INET,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FIRST,
	}
};

static int __init route_init( void )
{
	memset( &_device, 0, sizeof( _device ) );
	{
		_device.hash_map = create_hash_map();

	}
	nf_register_net_hooks( &init_net, route_nf_hook_ops, 2 );

	SKY_DBG( "xsky route loaded\n" );

	return 0;
}

static void __exit route_exit( void )
{
	nf_unregister_net_hooks( &init_net, route_nf_hook_ops, 2 );
	free_hash_map( _device.hash_map );

	SKY_DBG( "xsky route unloaded\n" );
}

module_init( route_init );
module_exit( route_exit );
MODULE_LICENSE( "GPL" );
