#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/udp.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/version.h>

#include "queue.h"
#include "dev_ctl.h"

#define DEV_NAME XSKY_DEV_NAME
#define DEV_CLASS XSKY_DEV_NAME
#define QUEUE_SIZE 256

void calc_subnet( char * buf, char * ipaddress, unsigned char prefix, char is_ipv6 );
int xsky_set_udp_proxy_subnet( unsigned long arg );
int xsky_set_tunnel( unsigned long arg );
int xsky_is_subnet( char * ipaddress, char is_ipv6 );
unsigned int xsky_push_ipv4_packet_to_user( struct iphdr * ip_header );
unsigned int xsky_push_ipv6_packet_to_user( struct ipv6hdr * ip6_header );
unsigned int handle_ipv4_dgram_in( struct iphdr * ip_header );
unsigned int handle_ipv6_dgram_in( struct ipv6hdr * ip6_header );

unsigned int nf_handle_in(
#if LINUX_VERSION_CODE<=KERNEL_VERSION(3,1,2)
	unsigned int hooknum,
#endif
#if LINUX_VERSION_CODE>=KERNEL_VERSION(4,4,0)
	void * priv,
#endif
#if LINUX_VERSION_CODE<KERNEL_VERSION(4,4,0) && LINUX_VERSION_CODE>(3,1,2)
	const struct nf_hook_ops * ops,
#endif
	struct sk_buff * skb,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)
	const struct nf_hook_state * state
#else
	const struct net_device * in,
	const struct net_device * out,
	int ( *okfn )( struct sk_buff * )
#endif
);

int create_dev( void );
int delete_dev( void );
int chr_open( struct inode * node, struct file * f );
long chr_ioctl( struct file * f, unsigned int cmd, unsigned long arg );
ssize_t chr_read( struct file * f, char __user * u, size_t size, loff_t * loff );
ssize_t chr_write( struct file * f, const char __user * u, size_t size, loff_t * loff );
int chr_release( struct inode * node, struct file * f );
unsigned int chr_poll( struct file * f, struct poll_table_struct * wait );

struct xsky_poll
{
	wait_queue_head_t inq;
	struct xsky_queue * r_queue;
} * poll;

dev_t ndev;
int dev_major = 0;
char flock_flag = 0;
struct cdev chr_dev;
struct class * dev_class;
struct file_operations chr_ops;

char xsky_tunnel_addr[4];
char xsky_tunnel_addr6[16];

struct xsky_subnet subnet;
struct xsky_subnet subnet6;

char is_set_subnet = 0;
char is_set_subnet6 = 0;
char is_set_tunnel_addr = 0;
char is_set_tunnel_addr6 = 0;

struct nf_hook_ops nf_ops =
{
	.hook = nf_handle_in,
	.hooknum = NF_INET_FORWARD,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST
};

struct nf_hook_ops nf6_ops =
{
	.hook = nf_handle_in,
	.hooknum = NF_INET_FORWARD,
	.pf = PF_INET6,
	.priority = NF_IP6_PRI_FIRST
};

struct file_operations chr_ops =
{
	.owner = THIS_MODULE,
	.open = chr_open,
	.unlocked_ioctl = chr_ioctl,
	.read = chr_read,
	.write = chr_write,
	.release = chr_release,
	.poll = chr_poll
};


void calc_subnet( char * buf, char * ipaddress, unsigned char prefix, char is_ipv6 )
{
	int n = 4;
	unsigned char a, b;
	unsigned char tables[] =
	{
		128,192,224,240,248,252,254,
	};

	if ( is_ipv6 ) n = 16;

	memset( buf, 0, n );

	a = prefix / 8;
	b = prefix % 8;

	for ( int i = 0; i < a; i++ )
	{
		buf[i] = ipaddress[i];
	}
	if ( b )
		buf[a] = tables[b - 1] & ipaddress[a];

	return;
}

int xsky_set_udp_proxy_subnet( unsigned long arg )
{
	struct xsky_subnet tmp, * t;

	int err = copy_from_user( &tmp, (unsigned long *)arg, sizeof( struct xsky_subnet ) );
	if ( err ) return -EINVAL;

	if ( tmp.is_ipv6 && tmp.prefix > 128 ) return -EINVAL;
	if ( !tmp.is_ipv6 && tmp.prefix > 32 ) return -EINVAL;

	if ( tmp.is_ipv6 ) t = &subnet6;
	else t = &subnet;

	calc_subnet( t->address, tmp.address, tmp.prefix, tmp.is_ipv6 );

	//printk("%d %d %d %d----\r\n",(UC)tmp.address[0],(UC)tmp.address[1],(UC)tmp.address[2],(UC)tmp.address[3]);

	t->is_ipv6 = tmp.is_ipv6;
	t->prefix = tmp.prefix;

	if ( t->is_ipv6 ) is_set_subnet6 = 1;
	else is_set_subnet = 1;

	return 0;
}

int xsky_set_tunnel( unsigned long arg )
{
	struct xsky_address tmp;

	int err = copy_from_user( &tmp, (unsigned long *)arg, sizeof( struct xsky_address ) );
	if ( err ) return -EINVAL;

	if ( tmp.is_ipv6 )
	{
		memcpy( xsky_tunnel_addr6, tmp.address, 16 );
		is_set_tunnel_addr6 = 1;
	}
	else
	{
		memcpy( xsky_tunnel_addr, tmp.address, 4 );
		is_set_tunnel_addr = 1;
	}

	return 0;
}

int xsky_is_subnet( char * ipaddress, char is_ipv6 )
{
	char buf[16];
	int n = 4;
	struct xsky_subnet * t;

	if ( is_ipv6 )
	{
		t = &subnet6;
		n = 16;
	}
	else
	{
		t = &subnet;
	}

	calc_subnet( buf, ipaddress, t->prefix, is_ipv6 );
	//printk("----%d %d %d %d --%d %d %d %d\r\n",(UC)buf[0],(UC)buf[1],(UC)buf[2],(UC)buf[3],(UC)t->address[0],(UC)t->address[1],(UC)t->address[2],(UC)t->address[3]);
	return !memcmp( buf, t->address, n );
}

unsigned int xsky_push_ipv4_packet_to_user( struct iphdr * ip_header )
{
	int err, tot_len;
	tot_len = ntohs( ip_header->tot_len );
	err = xsky_queue_push( poll->r_queue, (char *)ip_header, tot_len );

	if ( err ) return NF_ACCEPT;
	wake_up_interruptible( &poll->inq );

	return NF_DROP;
}

unsigned int xsky_push_ipv6_packet_to_user( struct ipv6hdr * ip6_header )
{
	int err = 0;
	unsigned short data_len = ntohs( ip6_header->payload_len ) + 40;
	err = xsky_queue_push( poll->r_queue, (char *)ip6_header, data_len );
	if ( err ) return NF_ACCEPT;

	wake_up_interruptible( &poll->inq );

	return NF_DROP;
}

unsigned int handle_ipv4_dgram_in( struct iphdr * ip_header )
{
	unsigned int saddr = (unsigned int)ip_header->saddr;
	unsigned int daddr = (unsigned int)ip_header->daddr;

	if ( is_set_tunnel_addr && 0 == memcmp( xsky_tunnel_addr, (char *)( &daddr ), 4 ) )
		return NF_ACCEPT;

	if ( !xsky_is_subnet( (char *)( &saddr ), 0 ) )
		return NF_ACCEPT;

	return xsky_push_ipv4_packet_to_user( ip_header );
}

unsigned int handle_ipv6_dgram_in( struct ipv6hdr * ip6_header )
{
	unsigned char * saddr = ( ip6_header->saddr ).s6_addr;
	unsigned char * daddr = ( ip6_header->daddr ).s6_addr;

	if ( is_set_tunnel_addr6 && 0 == memcmp( xsky_tunnel_addr6, daddr, 16 ) )
		return NF_ACCEPT;

	if ( !xsky_is_subnet( saddr, 1 ) )
		return NF_ACCEPT;

	return xsky_push_ipv6_packet_to_user( ip6_header );
}

unsigned int nf_handle_in(
#if LINUX_VERSION_CODE<=KERNEL_VERSION(3,1,2)
	unsigned int hooknum,
#endif
#if LINUX_VERSION_CODE>=KERNEL_VERSION(4,4,0)
	void * priv,
#endif
#if LINUX_VERSION_CODE<KERNEL_VERSION(4,4,0) && LINUX_VERSION_CODE>(3,1,2)
	const struct nf_hook_ops * ops,
#endif
	struct sk_buff * skb,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)
	const struct nf_hook_state * state
#else
	const struct net_device * in,
	const struct net_device * out,
	int ( *okfn )( struct sk_buff * )
#endif
)
{
	unsigned char nexthdr;
	struct iphdr * ip_header;
	struct ipv6hdr * ip6_header;

	if ( !flock_flag ) return NF_ACCEPT;

	if ( !skb ) return NF_ACCEPT;

	ip_header = (struct iphdr *)skb_network_header( skb );

	if ( !ip_header )
		return NF_ACCEPT;

	if ( 4 == ip_header->version )
	{
		nexthdr = ip_header->protocol;
	}
	else
	{
		ip6_header = (struct ipv6hdr *)ipv6_hdr( skb );
		
		if ( !ip6_header )
			return NF_ACCEPT;
		
		nexthdr = ip6_header->nexthdr;
	}

	if ( nexthdr != 17 && nexthdr != 136 )
		return NF_ACCEPT;

	if ( 4 == ip_header->version )
		return handle_ipv4_dgram_in( ip_header );

	return handle_ipv6_dgram_in( ip6_header );
}


int create_dev( void )
{
	int ret;
	cdev_init( &chr_dev, &chr_ops );
	ret = alloc_chrdev_region( &ndev, 0, 1, DEV_NAME );

	if ( ret < 0 )
		return ret;

	cdev_add( &chr_dev, ndev, 1 );
	dev_class = class_create( THIS_MODULE, DEV_CLASS );

	if ( IS_ERR( dev_class ) )
	{
		printk( "ERR:failed in creating class\r\n" );
		return -1;
	}

	dev_major = MAJOR( ndev );
	device_create( dev_class, NULL, ndev, "%s", DEV_NAME );

	return 0;
}

int delete_dev( void )
{
	cdev_del( &chr_dev );
	device_destroy( dev_class, ndev );
	class_destroy( dev_class );
	unregister_chrdev_region( ndev, 0 );

	return 0;
}

int chr_open( struct inode * node, struct file * f )
{
	int major, minor;
	major = MAJOR( node->i_rdev );
	minor = MINOR( node->i_rdev );

	if ( flock_flag )
		return -EBUSY;

	flock_flag = 1;
	f->private_data = poll;

	return 0;
}

long chr_ioctl( struct file * f, unsigned int cmd, unsigned long arg )
{
	int ret = 0;
	if ( _IOC_TYPE( cmd ) != XSKY_IOC_MAGIC )
		return -EINVAL;

	switch ( cmd )
	{
	case XSKY_IOC_SET_UDP_PROXY_SUBNET:
		ret = xsky_set_udp_proxy_subnet( arg );
		break;
	case XSKY_IOC_SET_TUNNEL_IP:
		ret = xsky_set_tunnel( arg );
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

ssize_t chr_read( struct file * f, char __user * u, size_t size, loff_t * loff )
{
	struct xsky_queue_data * tmp;

	tmp = xsky_queue_pop( poll->r_queue );

	if ( NULL == tmp )
		return -EAGAIN;

	if ( 0 != copy_to_user( u, tmp->data, tmp->size ) )
		return -EFAULT;

	return tmp->size;
}

ssize_t chr_write( struct file * f, const char __user * u, size_t size, loff_t * loff )
{

}

int chr_release( struct inode * node, struct file * f )
{
	flock_flag = 0;
	xsky_queue_reset( poll->r_queue );

	return 0;
}

unsigned int chr_poll( struct file * f, struct poll_table_struct * wait )
{
	struct xsky_poll * p;
	unsigned int mask = 0;
	p = f->private_data;

	poll_wait( f, &p->inq, wait );
	if ( p->r_queue->have )
		mask = POLLIN | POLLRDNORM;
	else
		mask = POLLRDNORM;

	return mask;
}


#if LINUX_VERSION_CODE>=KERNEL_VERSION(4,13,0)
int nf_register_hook( struct nf_hook_ops * reg )
{
	struct net * net, * last;
	int ret;

	rtnl_lock();
	for_each_net( net )
	{
		ret = nf_register_net_hook( net, reg );
		if ( ret && ret != -ENOENT )
			goto rollback;
	}
	rtnl_unlock();

	return 0;
rollback:
	last = net;
	for_each_net( net )
	{
		if ( net == last )
			break;
		nf_unregister_net_hook( net, reg );
	}
	rtnl_unlock();
	return ret;
}

static void nf_unregister_hook( struct nf_hook_ops * reg )
{
	struct net * net;

	rtnl_lock();
	for_each_net( net )
		nf_unregister_net_hook( net, reg );
	rtnl_unlock();
}
#endif

int xsky_init( void )
{
	int ret = create_dev();
	if ( 0 != ret )
		return ret;

	nf_register_hook( &nf_ops );
	nf_register_hook( &nf6_ops );

	poll = kmalloc( sizeof( struct xsky_poll ), GFP_ATOMIC );
	init_waitqueue_head( &poll->inq );

	poll->r_queue = xsky_queue_init( QUEUE_SIZE );

	return 0;
}

void xsky_exit( void )
{
	delete_dev();
	nf_unregister_hook( &nf_ops );
	nf_unregister_hook( &nf6_ops );
	xsky_queue_release( poll->r_queue );

	kfree( poll );
}

module_init( xsky_init );
module_exit( xsky_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "xsky" );
MODULE_DESCRIPTION( "the module for xsky" );
