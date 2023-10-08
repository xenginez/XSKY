#include "route.h"

static void route_capture_handle( struct sk_buff * skb );
static unsigned int route_arp_handle( struct sk_buff * skb );
static unsigned int route_ipv4_handle( struct sk_buff * skb );
static unsigned int route_ipv6_handle( struct sk_buff * skb );
static unsigned int route_icmpv4_handle( struct sk_buff * skb );
static unsigned int route_icmpv6_handle( struct sk_buff * skb );
static unsigned int route_tcp_handle( struct sk_buff * skb );
static unsigned int route_udp_handle( struct sk_buff * skb );
static unsigned int route_dns_handle( struct sk_buff * skb );
static unsigned int route_http_handle( struct sk_buff * skb );

struct route_device * _device;

static void route_capture_handle( struct sk_buff * skb )
{
	struct sk_buff * nskb = alloc_skb();
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
	struct icmpv6hdr * icmp_h = icmpv6_hdr( skb );
	
	return NF_ACCEPT;
}
static unsigned int route_tcp_handle( struct sk_buff * skb )
{
	struct tcphdr * tcp_h = tcp_hdr( skb );

	if( tcp_h->source == htons( HTTP_FIXED_PORT ) || tcp_h->dest == htons( HTTP_FIXED_PORT ) )
	{
		return route_http_handle( skb );
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

	struct protocol_dns * dns_h = (protocol_dns *)( udp_h + 1 );

	return NF_ACCEPT;
}
static unsigned int route_http_handle( struct sk_buff * skb )
{
	return NF_ACCEPT;
}

static unsigned int route_nf_handle( void * priv, struct sk_buff * skb, const struct nf_hook_state * state )
{
	struct ethhdr * eth_h = eth_hdr( skb );

	if( eth_h->h_proto == htons( ETH_TYPE_ARP ) && _device->flag.arp != 0 )
	{
		return route_arp_handle( skb );
	}
	else if( _device->flag.icmp != 0 || _device->flag.tcp != 0 || _device->flag.udp != 0 || _device->flag.dns != 0 || _device->flag.http != 0 )
	{
		if( eth_h->h_proto == htons( ETH_TYPE_IPV4 ) )
		{
			return route_ipv4_handle( skb );
		}
		else if( eth_h->h_proto == htons( ETH_TYPE_IPV6 ) )
		{
			return route_ipv6_handle( skb );
		}
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops route_nf_hook_ops[] =
{
	{
		.hook = route_nf_handle,
		.pf = PF_INET,
		.hooknum = NF_ARP_IN,
		.priority = NF_ARP_PRI_FIRST,
	},
	{
		.hook = route_nf_handle,
		.pf = PF_INET,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FIRST,
	},
	{
		.hook = route_nf_handle,
		.pf = PF_INET,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP6_PRI_FIRST,
	}
};

static int __init route_init( void )
{
	_device = vzalloc( sizeof( struct route_device ) );
	_device->routes = create_hash_map();
	nf_register_net_hooks( &init_net, route_nf_hook_ops, 2 );

	SKY_DBG( "xsky route loaded\n" );

	return 0;
}

static void __exit route_exit( void )
{
	nf_unregister_net_hooks( &init_net, route_nf_hook_ops, 2 );
	release_hash_map( _device->routes );
	vzfree( _device );

	SKY_DBG( "xsky route unloaded\n" );
}

module_init( route_init );
module_exit( route_exit );
MODULE_LICENSE( "GPL" );
