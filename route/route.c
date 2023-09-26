#include "route.h"

struct route_device * _device;

static unsigned int hash_route_sky( sky_data * data )
{
	int _Idx = 0;
	unsigned int _Val = 2166136261U;
	unsigned char * _First = (unsigned char *)data;

	for ( ; _Idx < sizeof( sky_data ); ++_Idx )
	{
		_Val ^= (unsigned int)( _First[_Idx] );
		_Val *= 16777619U;
	}

	return _Val;
}

static void insert_route_sky( struct nat_data * pair )
{
	int i = 0;
	struct nat_data_page * tmp = NULL;
	struct nat_data_page * page = NULL;
	unsigned int first_hash = hash_route_sky( &pair->key );
	unsigned int second_hash = hash_route_sky( &pair->value );

	i = 0;
	page = _device->routes[first_hash % SKY_DATA_BUCKET_SIZE].pages;
	while ( page != NULL )
	{
		if ( page->used < SKY_DATA_PAIR_SIZE )
		{
			break;
		}

		page = page->next;
	}
	if ( page == NULL )
	{
		page = vzalloc( sizeof( struct nat_data_page ) );

		tmp = _device->routes[first_hash % SKY_DATA_BUCKET_SIZE].pages;
		_device->routes[first_hash % SKY_DATA_BUCKET_SIZE].pages = page;
		page->next = tmp;
	}
	for ( ; i < SKY_DATA_PAIR_SIZE; ++i )
	{
		if ( page->pairs[i].key.v4.code == 0 )
		{
			page->pairs[i] = *pair;
			page->used++;
			break;
		}
	}

	i = 0;
	page = _device->routes[second_hash % SKY_DATA_BUCKET_SIZE].pages;
	while ( page != NULL )
	{
		if ( page->used < SKY_DATA_PAIR_SIZE )
		{
			break;
		}

		page = page->next;
	}
	if ( page == NULL )
	{
		page = vzalloc( sizeof( struct nat_data_page ) );

		tmp = _device->routes[second_hash % SKY_DATA_BUCKET_SIZE].pages;
		_device->routes[second_hash % SKY_DATA_BUCKET_SIZE].pages = page;
		page->next = tmp;
	}
	for ( ; i < SKY_DATA_PAIR_SIZE; ++i )
	{
		if ( page->pairs[i].key.v4.code == 0 )
		{
			page->pairs[i].key = pair->value;
			page->pairs[i].value = pair->key;
			page->used++;
			break;
		}
	}
}

static struct nat_data * find_route_sky( sky_data * data )
{
	int i = 0;
	struct nat_data_page * page = NULL;
	unsigned int hash = hash_route_sky( data );

	page = _device->routes[hash % SKY_DATA_BUCKET_SIZE].pages;
	while ( page != NULL )
	{
		i = 0;
		for ( ; i < SKY_DATA_PAIR_SIZE; ++i )
		{
			if ( page->pairs[i].key.v4.code == data->v4.code && 
				 page->pairs[i].key.v4.size == data->v4.size &&
				 page->pairs[i].key.v4.addr == data->v4.addr &&
				 page->pairs[i].key.v4.port == data->v4.port )
			{
				return &page->pairs[i];
			}
		}
		page = page->next;
	}

	return NULL;
}

static void remove_route_sky( sky_data * data )
{
	struct nat_data * pair = find_route_sky( data );
	int i = 0;
	struct nat_data_page * page = NULL;
	unsigned int first_hash = 0;
	unsigned int second_hash = 0;

	if ( pair != NULL )
	{
		first_hash = hash_route_sky( &pair->key );
		second_hash = hash_route_sky( &pair->value );

		i = 0;
		page = _device->routes[first_hash % SKY_DATA_BUCKET_SIZE].pages;
		while ( page != NULL )
		{
			if ( page->used > 0 )
			{
				for ( ; i < SKY_DATA_PAIR_SIZE; ++i )
				{
					if ( page->pairs[i].key.v4.code == pair->key.v4.code &&
						 page->pairs[i].key.v4.size == pair->key.v4.size &&
						 page->pairs[i].key.v4.addr == pair->key.v4.addr &&
						 page->pairs[i].key.v4.port == pair->key.v4.port )
					{
						memset( &page->pairs[i], 0, sizeof( struct nat_data ) );
						page->used--;
						break;
					}
				}
			}

			page = page->next;
		}

		i = 0;
		page = _device->routes[second_hash % SKY_DATA_BUCKET_SIZE].pages;
		while ( page != NULL )
		{
			if ( page->used > 0 )
			{
				for ( ; i < SKY_DATA_PAIR_SIZE; ++i )
				{
					if ( page->pairs[i].key.v4.code == pair->value.v4.code &&
						 page->pairs[i].key.v4.size == pair->value.v4.size &&
						 page->pairs[i].key.v4.addr == pair->value.v4.addr &&
						 page->pairs[i].key.v4.port == pair->value.v4.port )
					{
						memset( &page->pairs[i], 0, sizeof( struct nat_data ) );
						page->used--;
						break;
					}
				}
			}

			page = page->next;
		}
	}
}

static void set_toa_data_sky( struct sk_buff * skb, sky_data * toa )
{
	int len = 0;
	unsigned char * ptr;
	unsigned char * data;
	struct iphdr * ip_h = ip_hdr( skb );
	struct tcphdr * tcp_h = tcp_hdr( skb );

	int ihead_len = ip_h->ihl * 4;
	int thead_len = tcp_h->doff * 4;
	int opt_len = ( tcp_h->doff * 4 ) - sizeof( struct tcphdr );
	int data_len = skb->len - ip_h->ihl * 4 - tcp_h->doff * 4;

	if ( NULL != skb )
	{
		if ( toa->v4.code == TCPOPT_V4_SKY )
			len = TCPOLEN_V4_SKY;
		else
			len = TCPOLEN_V6_SKY;

		data = skb_push( skb, len );
		ptr = data + len;

		memcpy( data, ptr, ihead_len );
		data += ihead_len; ptr += ihead_len;

		memcpy( data, ptr, sizeof( struct tcphdr ) );
		data += sizeof( struct tcphdr ); ptr += sizeof( struct tcphdr );

		memcpy( data, &toa, len );
		data += len;

		memcpy( data, ptr, opt_len );
		data += opt_len; ptr += opt_len;

		memcpy( data, ptr, data_len );

		ip_h = ip_hdr( skb );
		tcp_h = tcp_hdr( skb );

		ip_h->ihl = ( ihead_len + len ) / 4;
		tcp_h->doff = ( thead_len + len ) / 4;
	}
}

static void * get_toa_data_sky( struct sk_buff * skb, sky_data * pdata )
{
	struct tcphdr * th;
	int length;
	unsigned char * ptr;

	unsigned char buff[( 15 * 4 ) - sizeof( struct tcphdr )];

	if ( NULL != skb )
	{
		th = tcp_hdr( skb );
		length = ( th->doff * 4 ) - sizeof( struct tcphdr );
		ptr = skb_header_pointer( skb, sizeof( struct tcphdr ), length, buff );
		if ( !ptr )
			return NULL;

		while ( length > 0 )
		{
			int opcode = *ptr++;
			int opsize;
			switch ( opcode )
			{
			case TCPOPT_EOL:
				return NULL;
			case TCPOPT_NOP:
				length--;
				continue;
			default:
				opsize = *ptr++;
				if ( opsize < 2 )
					return NULL;
				if ( opsize > length )
					return NULL;
				if ( TCPOPT_V4_SKY == opcode && TCPOLEN_V4_SKY == opsize )
				{
					if ( pdata == NULL )
						pdata = (sky_data *)kmalloc( sizeof( sky_data ), GFP_KERNEL );

					memcpy( pdata, ptr - 2, sizeof( struct sky_v4_data ) );
					SKY_DBG( "find sky v4 data: ip = %u.%u.%u.%u, port = %u\n", NIPV4( pdata->v4.addr ), ntohs( pdata->v4.port ) );
					return pdata;
				}
				else if ( TCPOPT_V6_SKY == opcode && TCPOLEN_V6_SKY == opsize )
				{
					if ( pdata == NULL )
						pdata = (sky_data *)kmalloc( sizeof( sky_data ), GFP_KERNEL );

					memcpy( pdata, ptr - 2, sizeof( struct sky_v6_data ) );
					SKY_DBG( "find sky v6 data: ip = %04u::%04u::%04u::%04u::%04u::%04u::%04u::%04u, port = %u\n", NIPV6( pdata->v6.ip ), ntohs( pdata->v6.port ) );
					return pdata;
				}

				ptr += opsize - 2;
				length -= opsize;
			}
		}
	}

	return NULL;
}

static void xor_data_sky( unsigned char * data, int length )
{
	int i = 1;
	for ( ; i < length; i *= 8 )
	{
		*(int *)( data + i ) ^= length;
	}
}

static unsigned int route_pre_routing_sky( void * priv, struct sk_buff * skb, const struct nf_hook_state * state )
{
	struct iphdr * ip_h = ip_hdr( skb );
	struct tcphdr * tcp_h;
	//struct udphdr * udp_h;
	//struct icmphdr * icmp_h;

	sky_data toa_data;
	int data_len = 0;
	unsigned char * data_ptr = NULL;

	if ( ( (unsigned char *)( &ip_h->daddr ) )[0] == 192 ||
		 ( (unsigned char *)( &ip_h->daddr ) )[0] == 127 )
	{
		return NF_ACCEPT;
	}
	else if ( likely( ip_h->protocol == IPPROTO_TCP ) )
	{
		if ( skb_is_nonlinear( skb ) )
			if ( skb_linearize( skb ) )
				return NF_DROP;

		ip_h = ip_hdr( skb );
		tcp_h = tcp_hdr( skb );

		if ( tcp_h->syn == 1 && tcp_h->ack == 0 )
		{
			toa_data.v4.addr = ip_h->daddr;
			toa_data.v4.port = tcp_h->dest;
			toa_data.v4.code = TCPOPT_V4_SKY;
			toa_data.v4.size = TCPOLEN_V4_SKY;

			set_toa_data_sky( skb, &toa_data );

			ip_h = ip_hdr( skb );
			tcp_h = tcp_hdr( skb );

			ip_h->daddr = _device->servers;
			tcp_h->dest = htons( 48888 );
		}

		data_len = skb->len - ip_h->ihl * 4 - tcp_h->doff * 4;
		data_ptr = skb->data + ip_h->ihl * 4 + tcp_h->doff * 4;

		xor_data_sky( data_ptr, data_len );

		ip_h->check = 0;
		ip_h->check = ip_fast_csum( (unsigned char *)ip_h, ip_h->ihl );
		if ( skb->ip_summed == CHECKSUM_HW )
		{
			tcp_h->check = csum_tcpudp_magic( ip_h->saddr, ip_h->daddr, ( ntohs( ip_h->tot_len ) - ip_h->ihl * 4 ), IPPROTO_TCP, csum_partial( tcp_h, ( ntohs( ip_h->tot_len ) - ip_h->ihl * 4 ), 0 ) );
			skb->csum = offsetof( struct tcphdr, check );
		}
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops nf_route_hooks_sky[] =
{
	{
		.hook = route_pre_routing_sky,
		.pf = PF_INET,
		.hooknum = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FIRST,
	}
};

static int __init route_init( void )
{
	_device = kzalloc( sizeof( struct route_device ), GFP_KERNEL );
	_device->routes = kzalloc( sizeof( struct nat_data_bucket ), GFP_KERNEL );

	if ( nf_register_net_hooks( &init_net, nf_route_hooks_sky, 1 ) != 0 )
	{
		SKY_DBG( "nf_register_net_hooks failed\n" );
	}

	SKY_DBG( "xsky route loaded\n" );

	return 0;
}

static void __exit route_exit( void )
{
	int i = 0;
	struct nat_data_page * page = NULL;

	nf_unregister_net_hooks( &init_net, nf_route_hooks_sky, 1 );

	for ( ; i < SKY_DATA_BUCKET_SIZE; ++i )
	{
		while ( _device->routes[i].pages != NULL )
		{
			page = _device->routes[i].pages->next;
			vfree( _device->routes[i].pages );
			_device->routes[i].pages = page;
		}
	}

	vfree( _device->routes );
	kfree( _device );

	SKY_DBG( "xsky route unloaded\n" );
}


module_init( route_init );
module_exit( route_exit );
MODULE_LICENSE( "GPL" );
