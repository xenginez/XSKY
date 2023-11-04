#ifndef XSKY_DEV_CTL_H
#define XSKY_DEV_CTL_H

#define XSKY_DEV_NAME "xsky_dgram"
#define XSKY_IOC_MAGIC 0xad

// 设置全局UDP代理的局域网机器
#define XSKY_IOC_SET_UDP_PROXY_SUBNET _IOW(XSKY_IOC_MAGIC,1,int)
// 设置隧道IP
#define XSKY_IOC_SET_TUNNEL_IP _IOW(XSKY_IOC_MAGIC,2,int)

struct xsky_subnet
{
    char address[16];
    unsigned char prefix;
    char is_ipv6;
};

struct xsky_address
{
    char address[16];
    char is_ipv6;
};

#endif //!XSKY_DEV_CTL_H
