#include "nat_map.h"

#define NAT_DATA_PAIR_SIZE 100
#define NAT_DATA_BUCKET_SIZE 1000

struct nat_page
{
	struct nat_page * next;

	unsigned long long used;
	struct nat_pair pairs[NAT_DATA_PAIR_SIZE];
};

struct nat_map
{
	struct nat_page * pages;
};

unsigned int hash_route_sky( nat_data * data )
{
	int _Idx = 0;
	unsigned int _Val = 2166136261U;
	unsigned char * _First = (unsigned char *) data;

	for( ; _Idx < sizeof( nat_data ); ++_Idx )
	{
		_Val ^= (unsigned int) ( _First[_Idx] );
		_Val *= 16777619U;
	}

	return _Val;
}

struct nat_map * create_nat_map()
{
	return kzalloc( sizeof( struct nat_map ), GFP_KERNEL );
}

void insert_nat_map( struct nat_map * map, nat_pair * nat )
{
}

nat_data * find_nat_map( nat_data * key )
{
	return nullptr;
}

void remove_nat_map( struct nat_map * map, nat_pair * nat )
{
}

void release_nat_map( struct nat_map * map )
{
}
