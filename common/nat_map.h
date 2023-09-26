#ifndef __NAT_MAP_H__
#define __NAT_MAP_H__

typedef struct
{
	__u16 port;
	__u32 addr;
} nat_v4_data;
typedef struct
{
	__u16 port;
	union
	{
		__u16 ip[8];
		__u32 wip[4];
	};
} nat_v6_data;
typedef union
{
	struct nat_v4_data v4;
	struct nat_v6_data v6;
}nat_data;
typedef struct
{
	nat_data key;
	nat_data value;
} nat_pair;

struct nat_map;

struct nat_map * create_nat_map();

void insert_nat_map( struct nat_map * map, nat_pair * nat );

nat_data * find_nat_map( nat_data * key );

void remove_nat_map( struct nat_map * map, nat_pair * nat );

void release_nat_map( struct nat_map * map );

#endif // !__NAT_MAP_H__
