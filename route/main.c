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
#include <linux/inetdevice.h>

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

struct
{
	hash_map nat_map;
	hash_map hash_map;
	hash_map limit_map;
	config_info config;
	capture_info capture;
} _device;

static void load_config( unsigned char * data );
static void load_capture( unsigned char * data );
static ipv4_addr get_local_ipv4( struct sk_buff * skb );
static ipv6_addr get_local_ipv6( struct sk_buff * skb );
static unsigned char * get_local_mac_addr( struct sk_buff * skb );
static bool is_local_ipv4( struct sk_buff * skb, ipv4_addr addr );
static bool is_local_ipv6( struct sk_buff * skb, struct in6_addr addr );
static bool is_local_mac_addr( struct sk_buff * skb, unsigned char * addr );

static bool skb_is_dns( struct sk_buff * skb );
static bool skb_is_config( struct sk_buff * skb );
static bool skb_is_set_capture( struct sk_buff * skb );
static bool skb_is_get_capture( struct sk_buff * skb );
static bool skb_is_client_to_server( struct sk_buff * skb );
static bool skb_is_server_to_client( struct sk_buff * skb );

static unsigned int skb_recv_dns( struct sk_buff * skb );
static unsigned int skb_recv_client( struct sk_buff * skb );
static unsigned int skb_recv_server( struct sk_buff * skb );
static unsigned int skb_recv_config( struct sk_buff * skb );
static unsigned int skb_recv_capture( struct sk_buff * skb );

static unsigned int skb_sendto_client( struct sk_buff * skb );
static unsigned int skb_sendto_server( struct sk_buff * skb );
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

static void load_config( unsigned char * data )
{
	int i = 0;
	char url[255];
	mac_info mac;
	domain_info domain;
	hash_map limit_map = create_hash_map();
	{
		memcpy( &_device.config, data, sizeof( config_info ) ); data += sizeof( config_info );
		for( i = 0; i < _device.config.macs; i++ )
		{
			memcpy( &mac, data, sizeof( mac_info ) ); data += sizeof( mac_info );
			insert_hash_map( _device.limit_map, mac.mac.mac, ETH_ALEN, mac.type );
		}
		for( i = 0; i < _device.config.domains; i++ )
		{
			memcpy( &domain, data, sizeof( domain_info ) ); data += sizeof( domain_info );
			memset( url, 0, 255 );
			memcpy( url, data, domain.strlen );
			insert_hash_map( _device.limit_map, url, domain.strlen, domain.type );
		}
	}
	free_hash_map( _device.limit_map );
	_device.limit_map = limit_map;
}
static void load_capture( unsigned char * data )
{
	memcpy( &_device.capture, data, sizeof( capture_info ) );
}
static ipv4_addr get_local_ipv4( struct sk_buff * skb )
{
	return skb->dev->ip_ptr->ifa_list->ifa_local;
}
static ipv6_addr get_local_ipv6( struct sk_buff * skb )
{
	ipv6_addr addr;
	addr.addr32[0] = skb->dev->ip6_ptr->ac_list->aca_addr.in6_u.u6_addr32[0];
	addr.addr32[1] = skb->dev->ip6_ptr->ac_list->aca_addr.in6_u.u6_addr32[1];
	addr.addr32[2] = skb->dev->ip6_ptr->ac_list->aca_addr.in6_u.u6_addr32[2];
	addr.addr32[3] = skb->dev->ip6_ptr->ac_list->aca_addr.in6_u.u6_addr32[3];
	return addr;
}
static unsigned char * get_local_mac_addr( struct sk_buff * skb )
{
	return skb->dev->dev_addr;
}
static bool is_local_ipv4( struct sk_buff * skb, ipv4_addr addr )
{
	struct in_ifaddr * ifaddr = skb->dev->ip_ptr->ifa_list;
	while( ifaddr != NULL )
	{
		if( ifaddr->ifa_local == addr )
			return true;

		ifaddr = ifaddr->ifa_next;
	}
	return false;
}
static bool is_local_ipv6( struct sk_buff * skb, struct in6_addr addr )
{
	struct ifacaddr6 * ifaddr = skb->dev->ip6_ptr->ac_list;
	while( ifaddr != NULL )
	{
		if( ifaddr->aca_addr.s6_addr32[0] == addr.s6_addr32[0] &&
			ifaddr->aca_addr.s6_addr32[1] == addr.s6_addr32[1] && 
			ifaddr->aca_addr.s6_addr32[2] == addr.s6_addr32[2] && 
			ifaddr->aca_addr.s6_addr32[3] == addr.s6_addr32[3] )
			return true;

		ifaddr = ifaddr->aca_next;
	}
	return false;
}
static bool is_local_mac_addr( struct sk_buff * skb, unsigned char * addr )
{
	return memcmp( skb->dev->dev_addr, addr, ETH_ALEN ) == 0;
}

static bool skb_is_dns( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );
	if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( ip_h->protocol == htons( IP_PROTO_UDP ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( DNS_FIXED_PORT ) )
			{
				return strnstr( (const char *) ( udp_h + 1 ), ROUTE_URL, ntohs( udp_h->len ) ) != NULL;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( DNS_FIXED_PORT ) )
			{
				return strnstr( (const char *) ( udp_h + 1 ), ROUTE_URL, ntohs( udp_h->len ) ) != NULL;
			}
		}
	}

	return false;
}
static bool skb_is_config( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );
	if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( ip_h->protocol == htons( IP_PROTO_UDP ) && is_local_ipv4( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				return true;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) && is_local_ipv6( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				return true;
			}
		}
	}

	return false;
}
static bool skb_is_set_capture( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );
	if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( ip_h->protocol == htons( IP_PROTO_UDP ) && is_local_ipv4( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CAPTURE_PORT ) )
			{
				return true;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) && is_local_ipv6( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CAPTURE_PORT ) )
			{
				return true;
			}
		}
	}

	return false;
}
static bool skb_is_get_capture( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );

	if( eth_h->h_proto == htons( ETH_TYPE_ARP ) )
	{
		return _device.capture.arp != 0;
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( !is_local_ipv4( skb, ip_h->saddr ) )
		{
			if( _device.capture.ip != 0 )
			{
				return true;
			}
			else if( ip_h->protocol == htons( IP_PROTO_ICMP ) )
			{
				if( _device.capture.icmp != 0 )
					return true;
			}
			else if( ip_h->protocol == htons( IP_PROTO_TCP ) )
			{
				if( _device.capture.tcp != 0 )
					return true;

				struct tcphdr * tcp_h = tcp_hdr( skb );

				if( _device.capture.http && ( tcp_h->dest == htons( HTTP_FIXED_PORT ) || tcp_h->source == htons( HTTP_FIXED_PORT ) ) )
					return true;
				else if( _device.capture.https && ( tcp_h->dest == htons( HTTPS_FIXED_PORT ) || tcp_h->source == htons( HTTPS_FIXED_PORT ) ) )
					return true;
			}
			else if( ip_h->protocol == htons( IP_PROTO_UDP ) )
			{
				if( _device.capture.udp != 0 )
					return true;

				struct udphdr * udp_h = udp_hdr( skb );

				if( _device.capture.dns && ( udp_h->dest == htons( DNS_FIXED_PORT ) || udp_h->source == htons( DNS_FIXED_PORT ) ) )
					return true;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( !is_local_ipv6( skb, ip_h->saddr ) )
		{
			if( _device.capture.ip != 0 )
			{
				return true;
			}
			else if( ip_h->nexthdr == htons( IPV6_NEXTHDR_ICMP ) )
			{
				if( _device.capture.icmp != 0 )
					return true;
			}
			else if( ip_h->nexthdr == htons( IPV6_NEXTHDR_TCP ) )
			{
				if( _device.capture.tcp != 0 )
					return true;

				struct tcphdr * tcp_h = tcp_hdr( skb );

				if( _device.capture.http && ( tcp_h->dest == htons( HTTP_FIXED_PORT ) || tcp_h->source == htons( HTTP_FIXED_PORT ) ) )
					return true;
				else if( _device.capture.https && ( tcp_h->dest == htons( HTTPS_FIXED_PORT ) || tcp_h->source == htons( HTTPS_FIXED_PORT ) ) )
					return true;
			}
			else if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) )
			{
				if( _device.capture.udp != 0 )
					return true;

				struct udphdr * udp_h = udp_hdr( skb );

				if( _device.capture.dns && ( udp_h->dest == htons( DNS_FIXED_PORT ) || udp_h->source == htons( DNS_FIXED_PORT ) ) )
					return true;
			}
		}
	}

	return false;
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
	struct ethhdr * eth = eth_hdr( skb );
	struct iphdr * ip = ip_hdr( skb );
	struct udphdr * udp = udp_hdr( skb );
	protocol_dns * dns = (protocol_dns *) ( udp + 1 );

	char dns_name[] =
	{
		0xc0, 0x0c, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x04, 0x0a, 0x0a, 0x0a, 0xfe
	};

	// dns
	{
		dns->flags = htons( 0x8580 );
		dns->ancount = ntohs( 1 );

		memcpy( &dns_name[sizeof( dns_name ) - 4], get_local_ipv4( skb ), 4 );
		void * p = skb_put( skb, sizeof( dns_name ) );
		memcpy( p, dns_name, sizeof( dns_name ) );


	}
	// udp
	{
		unsigned short tmp = udp->source;
		udp->source = udp->dest;
		udp->dest = tmp;
		udp->len = htons( ntohs( udp->len ) + sizeof( dns_name ) );
	}
	// ip
	{
		unsigned int tmp = ip->saddr;
		ip->saddr = ip->daddr;
		ip->daddr = tmp;

		ip->id = 0;
		ip->frag_off = htons( 0x4000 );
		ip->tot_len = htons( ntohs( ip->tot_len ) + sizeof( dns_name ) );
	}
	// eth
	{
		unsigned char tmp[ETH_ALEN];
		memcpy( tmp, eth->h_source, ETH_ALEN );
		memcpy( eth->h_source, eth->h_dest, ETH_ALEN );
		memcpy( eth->h_dest, tmp, ETH_ALEN );
	}

	skb->csum = skb_checksum( skb, ip->ihl * 4, skb->len - ip->ihl * 4, 0 );
	udp->check = csum_tcpudp_magic( ip->saddr, ip->daddr, skb->len - ip->ihl * 4, IPPROTO_UDP, skb->csum );
	ip_send_check( ip );

	dev_queue_xmit( skb );

	return NF_QUEUE;
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
	struct ethhdr * eth_h = eth_hdr( skb );
	if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( ip_h->protocol == htons( IP_PROTO_UDP ) && is_local_ipv4( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				load_config( (unsigned char *) ( udp_h + 1 ) );

				return NF_DROP;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) && is_local_ipv6( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				load_config( (unsigned char *) ( udp_h + 1 ) );

				return NF_DROP;
			}
		}
	}

	return NF_ACCEPT;
}
static unsigned int skb_recv_capture( struct sk_buff * skb )
{
	struct ethhdr * eth_h = eth_hdr( skb );
	if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
	{
		struct iphdr * ip_h = ip_hdr( skb );

		if( ip_h->protocol == htons( IP_PROTO_UDP ) && is_local_ipv4( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				load_capture( (unsigned char *) ( udp_h + 1 ) );

				return NF_DROP;
			}
		}
	}
	else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
	{
		struct ipv6hdr * ip_h = ipv6_hdr( skb );

		if( ip_h->nexthdr == htons( IPV6_NEXTHDR_UDP ) && is_local_ipv6( skb, ip_h->daddr ) )
		{
			struct udphdr * udp_h = udp_hdr( skb );

			if( udp_h->dest == htons( ROUTE_CONFIG_PORT ) )
			{
				load_capture( (unsigned char *) ( udp_h + 1 ) );

				return NF_DROP;
			}
		}
	}

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
static unsigned int skb_sendto_capture( struct sk_buff * skb )
{
	struct sk_buff * nskb = alloc_skb( skb->len + sizeof( struct iphdr ) + sizeof( struct udphdr ) + LL_RESERVED_SPACE( skb->dev ), GFP_ATOMIC );

	skb_reserve( nskb, LL_RESERVED_SPACE( skb->dev ) );

	struct ethhdr * eth = skb_push( nskb, 14 );
	struct iphdr * ip = skb_put( nskb, sizeof( struct iphdr ) );
	struct udphdr * udp = skb_put( nskb, sizeof( struct udphdr ) );
	void * payload = skb_put( nskb, skb->len );

	nskb->dev = skb->dev;
	nskb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons( ETH_P_IP );
	skb->ip_summed = CHECKSUM_NONE;
	skb->priority = 0;
	skb->network_header = ip;
	skb->transport_header = udp;

	memcpy( payload, skb->data, skb->len );
	// udp
	{
		udp->check = 0;

		udp->len = skb->len;
		udp->dest = htons( ROUTE_CAPTURE_PORT );
		udp->source = htons( ROUTE_CAPTURE_PORT );
	}
	// ip
	{
		ip->version = 4;
		ip->ihl = sizeof( struct iphdr ) >> 2;
		ip->frag_off = 0;
		ip->protocol = IPPROTO_UDP;
		ip->tos = 0;
		ip->daddr = _device.capture.addr.addr.v4;
		ip->saddr = get_local_ipv4(skb);
		ip->ttl = 0x40;
		ip->tot_len = htons( skb->len );
		ip->check = 0;
	}
	// eth
	{
		memcpy( eth->h_dest, _device.capture.mac.mac, ETH_ALEN );
		memcpy( eth->h_source, get_local_mac_addr( skb ), ETH_ALEN );
		eth->h_proto = htons( ETH_P_IP );
	}

	nskb->csum = skb_checksum( nskb, ip->ihl * 4, nskb->len - ip->ihl * 4, 0 );
	udp->check = csum_tcpudp_magic( ip->saddr, ip->daddr, nskb->len - ip->ihl * 4, IPPROTO_UDP, skb->csum );
	ip_send_check( ip );

	dev_queue_xmit( nskb );

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
	if( skb_is_dns( skb ) )
	{
		return skb_recv_dns( skb );
	}

	if( skb_is_config( skb ) )
	{
		return skb_recv_config( skb );
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
		_device.nat_map = create_hash_map();
		_device.hash_map = create_hash_map();
		_device.limit_map = create_hash_map();

	}
	nf_register_net_hooks( &init_net, route_nf_hook_ops, 2 );

	SKY_DBG( "xsky route loaded\n" );

	return 0;
}

static void __exit route_exit( void )
{
	nf_unregister_net_hooks( &init_net, route_nf_hook_ops, 2 );
	free_hash_map( _device.limit_map );
	free_hash_map( _device.hash_map );
	free_hash_map( _device.nat_map );

	SKY_DBG( "xsky route unloaded\n" );
}

module_init( route_init );
module_exit( route_exit );
MODULE_LICENSE( "GPL" );
