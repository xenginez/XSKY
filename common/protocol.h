/*!
 * \file protocol.h
 *
 * \author ZhengYuanQing
 * \date 2023/10/06
 * \email zhengyuanqing.95@gmail.com
 *
*/
#ifndef PROTOCOL_H__869E7B00_67AB_446B_B145_B196865FAA26
#define PROTOCOL_H__869E7B00_67AB_446B_B145_B196865FAA26

#define ETH_ALEN 6
#define ETH_TYPE_ARP 0x0806
#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_IPV6 0x86DD

#define IP_VERSION_4 0x0100
#define IP_VERSION_6 0x0110

#define ARP_HARDWARE_NETROM 0  /* from KA9Q: NET/ROM pseudo */
#define ARP_HARDWARE_ETHER  1  /* Ethernet 10Mbps  */
#define ARP_HARDWARE_EETHER 2  /* Experimental Ethernet */
#define ARP_HARDWARE_AX25 3  /* AX.25 Level 2  */
#define ARP_HARDWARE_PRONET 4  /* PROnet token ring  */
#define ARP_HARDWARE_CHAOS 5  /* Chaosnet   */
#define ARP_HARDWARE_IEEE802 6  /* IEEE 802.2 Ethernet/TR/TB */
#define ARP_HARDWARE_ARCNET 7  /* ARCnet   */
#define ARP_HARDWARE_APPLETLK 8  /* APPLEtalk   */
#define ARP_HARDWARE_DLCI 15  /* Frame Relay DLCI  */
#define ARP_HARDWARE_ATM 19  /* ATM     */
#define ARP_HARDWARE_METRICOM 23  /* Metricom STRIP (new IANA id) */
#define ARP_HARDWARE_IEEE1394 24  /* IEEE 1394 IPv4 - RFC 2734 */
#define ARP_HARDWARE_EUI64 27  /* EUI-64                       */
#define ARP_HARDWARE_INFINIBAND 32  /* InfiniBand   */
#define ARP_HARDWARE_SLIP 256
#define ARP_HARDWARE_CSLIP 257
#define ARP_HARDWARE_SLIP6 258
#define ARP_HARDWARE_CSLIP6 259
#define ARP_HARDWARE_RSRVD 260  /* Notional KISS type   */
#define ARP_HARDWARE_ADAPT 264
#define ARP_HARDWARE_ROSE 270
#define ARP_HARDWARE_X25 271  /* CCITT X.25   */
#define ARP_HARDWARE_HWX25 272  /* Boards with X.25 in firmware */
#define ARP_HARDWARE_CAN 280  /* Controller Area Network      */
#define ARP_HARDWARE_MCTP 290
#define ARP_HARDWARE_PPP 512
#define ARP_HARDWARE_CISCO 513  /* Cisco HDLC    */
#define ARP_HARDWARE_LAPB 516  /* LAPB    */
#define ARP_HARDWARE_DDCMP 517  /* Digital's DDCMP protocol     */
#define ARP_HARDWARE_RAWHDLC 518  /* Raw HDLC   */
#define ARP_HARDWARE_RAWIP 519  /* Raw IP                       */
#define ARP_HARDWARE_TUNNEL 768  /* IPIP tunnel   */
#define ARP_HARDWARE_TUNNEL6 769  /* IP6IP6 tunnel         */
#define ARP_HARDWARE_FRAD 770             /* Frame Relay Access Device    */
#define ARP_HARDWARE_SKIP 771  /* SKIP vif   */
#define ARP_HARDWARE_LOOPBACK 772  /* Loopback device  */
#define ARP_HARDWARE_LOCALTLK 773  /* Localtalk device  */
#define ARP_HARDWARE_FDDI 774  /* Fiber Distributed Data Interface */
#define ARP_HARDWARE_BIF 775             /* AP1000 BIF                   */
#define ARP_HARDWARE_SIT 776  /* sit0 device - IPv6-in-IPv4 */
#define ARP_HARDWARE_IPDDP 777  /* IP over DDP tunneller */
#define ARP_HARDWARE_IPGRE 778  /* GRE over IP   */
#define ARP_HARDWARE_PIMREG 779  /* PIMSM register interface */
#define ARP_HARDWARE_HIPPI 780  /* High Performance Parallel Interface */
#define ARP_HARDWARE_ASH 781  /* Nexus 64Mbps Ash  */
#define ARP_HARDWARE_ECONET 782  /* Acorn Econet   */
#define ARP_HARDWARE_IRDA 783  /* Linux-IrDA   */
#define ARP_HARDWARE_FCPP 784  /* Point to point fibrechannel */
#define ARP_HARDWARE_FCAL 785  /* Fibrechannel arbitrated loop */
#define ARP_HARDWARE_FCPL 786  /* Fibrechannel public loop */
#define ARP_HARDWARE_FCFABRIC 787  /* Fibrechannel fabric  */
#define ARP_HARDWARE_IEEE802_TR 800  /* Magic type ident for TR */
#define ARP_HARDWARE_IEEE80211 801  /* IEEE 802.11   */
#define ARP_HARDWARE_IEEE80211_PRISM 802 /* IEEE 802.11 + Prism2 header  */
#define ARP_HARDWARE_IEEE80211_RADIOTAP 803 /* IEEE 802.11 + radiotap header */
#define ARP_HARDWARE_IEEE802154 804
#define ARP_HARDWARE_IEEE802154_MONITOR 805 /* IEEE 802.15.4 network monitor */
#define ARP_HARDWARE_PHONET 820  /* PhoNet media type  */
#define ARP_HARDWARE_PHONET_PIPE 821  /* PhoNet pipe header  */
#define ARP_HARDWARE_CAIF 822  /* CAIF media type  */
#define ARP_HARDWARE_IP6GRE 823  /* GRE over IPv6  */
#define ARP_HARDWARE_NETLINK 824  /* Netlink header  */
#define ARP_HARDWARE_6LOWPAN 825  /* IPv6 over LoWPAN             */
#define ARP_HARDWARE_VSOCKMON 826  /* Vsock monitor header  */
#define ARP_HARDWARE_VOID 0xFFFF /* Void type, nothing is known */
#define ARP_HARDWARE_NONE 0xFFFE /* zero header length */

#define ARP_OPCODE_REQUEST 1  /* ARP request   */
#define ARP_OPCODE_REPLY 2  /* ARP reply   */
#define ARP_OPCODE_RREQUEST 3  /* RARP request   */
#define ARP_OPCODE_RREPLY 4  /* RARP reply   */
#define ARP_OPCODE_InREQUEST 8  /* InARP request  */
#define ARP_OPCODE_InREPLY 9  /* InARP reply   */
#define ARP_OPCODE_NAK 10  /* (ATM)ARP NAK   */

#define ICMP_TYPE_ECHOREPLY 0    /* Echo Reply            */
#define ICMP_TYPE_DEST_UNREACH 3    /* Destination Unreachable    */
#define ICMP_TYPE_SOURCE_QUENCH 4    /* Source Quench        */
#define ICMP_TYPE_REDIRECT 5    /* Redirect (change route)    */
#define ICMP_TYPE_ECHO 8    /* Echo Request            */
#define ICMP_TYPE_TIME_EXCEEDED 11    /* Time Exceeded        */
#define ICMP_TYPE_PARAMETERPROB 12    /* Parameter Problem        */
#define ICMP_TYPE_TIMESTAMP 13    /* Timestamp Request        */
#define ICMP_TYPE_TIMESTAMPREPLY 14    /* Timestamp Reply        */
#define ICMP_TYPE_INFO_REQUEST 15    /* Information Request        */
#define ICMP_TYPE_INFO_REPLY 16    /* Information Reply        */
#define ICMP_TYPE_ADDRESS 17    /* Address Mask Request        */
#define ICMP_TYPE_ADDRESSREPLY 18    /* Address Mask Reply        */

#define ICMP_CODE_NET_UNREACH 0    /* Network Unreachable        */
#define ICMP_CODE_HOST_UNREACH 1    /* Host Unreachable        */
#define ICMP_CODE_PROT_UNREACH 2    /* Protocol Unreachable        */
#define ICMP_CODE_PORT_UNREACH 3    /* Port Unreachable        */
#define ICMP_CODE_FRAG_NEEDED 4    /* Fragmentation Needed/DF set    */
#define ICMP_CODE_SR_FAILED 5    /* Source Route failed        */
#define ICMP_CODE_NET_UNKNOWN 6
#define ICMP_CODE_HOST_UNKNOWN 7
#define ICMP_CODE_HOST_ISOLATED 8
#define ICMP_CODE_NET_ANO 9
#define ICMP_CODE_HOST_ANO 10
#define ICMP_CODE_NET_UNR_TOS 11
#define ICMP_CODE_HOST_UNR_TOS 12
#define ICMP_CODE_PKT_FILTERED 13    /* Packet filtered */
#define ICMP_CODE_PREC_VIOLATION 14    /* Precedence violation */
#define ICMP_CODE_PREC_CUTOFF 15    /* Precedence cut off */

#define DNS_TYPE_A 1
#define DNS_TYPE_AAAA 28
#define DNS_CLASS_IN 1

union ipv6_addr
{
    unsigned char addr8[16];
    unsigned short addr16[8];
    unsigned int addr32[4];
};

struct protocol_eth
{
 unsigned char dst[ETH_ALEN];
 unsigned char src[ETH_ALEN];
 unsigned short type;
};

struct protocol_arp
{
    unsigned short int hardware;
    unsigned short int protocol;
    unsigned char hardware_len;
    unsigned char protocol_len;
    unsigned short int opcode;
};

struct protocol_ipv4
{
 unsigned int ihl : 4;
 unsigned int version : 4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
};

struct protocol_ipv6
{
    unsigned int version : 4;
    unsigned int priority : 8;
    unsigned int flow_lbl : 20;
    unsigned short payload_len;
    unsigned char nexthdr;
    unsigned char hop_limit;
    ipv6_addr saddr;
    ipv6_addr daddr;
};

struct protocol_icmp
{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    union
    {
        struct
        {
            unsigned short id;
            unsigned short sequence;
        } echo;
        unsigned int gateway;
        struct
        {
            unsigned short __unused;
            unsigned short mtu;
        } frag;
    } un;
};

struct protocol_tcp
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
};

struct protocol_udp
{
    unsigned short source;
    unsigned short dest;
    unsigned short len;
    unsigned short check;
};

struct protocol_dns
{
    unsigned short tid;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

#endif//PROTOCOL_H__869E7B00_67AB_446B_B145_B196865FAA26