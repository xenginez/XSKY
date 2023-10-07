#include "nat_map.h"

#define NAT_DATA_PAIR_SIZE 100
#define NAT_DATA_BUCKET_SIZE 100

typedef struct
{
	unsigned int key;
	void * value;
} nat_pair;

typedef struct _nat_page
{
	struct _nat_page * next;

	unsigned long long used;
	nat_pair pairs[NAT_DATA_PAIR_SIZE];
} nat_page;

struct nat_map
{
	nat_page pages[NAT_DATA_BUCKET_SIZE];
};

unsigned int hash( void * key, int key_len )
{
	int _Idx = 0;
	unsigned int _Val = 2166136261U;
	unsigned char * _First = (unsigned char *) key;

	for( ; _Idx < key_len; ++_Idx )
	{
		_Val ^= (unsigned int) ( _First[_Idx] );
		_Val *= 16777619U;
	}

	return _Val;
}


struct nat_map * create_nat_map()
{
	return (struct nat_map *)vzalloc( sizeof( struct nat_map ) );
}

void insert_nat_map( struct nat_map * map, void * key, int key_len, void * value )
{
	unsigned int h = hash( key, key_len );

	nat_page * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page->used == NAT_DATA_PAIR_SIZE )
	{
		if( page->next == 0 )
		{
			page->next = (nat_page *)vzalloc( sizeof( nat_page ) );
		}

		page = page->next;
	}

	for( int i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
	{
		if( page->pairs[i].key == 0 )
		{
			page->pairs[i].key = h;
			page->pairs[i].value = value;
			page->used += 1;
			
			break;
		}
	}
}

void * find_nat_map( struct nat_map * map, void * key, int key_len, )
{
	unsigned int h = hash( key, key_len );

	nat_page * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page != 0 )
	{
		for( int i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
		{
			if( page->pairs[i].key == h )
			{
				return page->pairs[i].value;
			}
		}

		if( page->next == 0 )
		{
			return 0;
		}

		page = page->next;
	}

	return 0;
}

void * remove_nat_map( struct nat_map * map, void * key, int key_len, )
{
	unsigned int h = hash( key, key_len );

	nat_page * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page != 0 )
	{
		for( int i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
		{
			if( page->pairs[i].key == h )
			{
				void * result = page->pairs[i].value;
				
				page->pairs[i].key = 0;
				page->pairs[i].value = 0;
				page->used -= 1;

				return result;
			}
		}

		if( page->next == 0 )
		{
			return 0;
		}

		page = page->next;
	}

	return 0;
}

void release_nat_map( struct nat_map * map )
{
	int i = 0;
	nat_page * page = 0, * tpage = 0;
	for( ; i < NAT_DATA_BUCKET_SIZE; ++i )
	{
		if( map->pages[i].next != 0 )
		{
			page = map->pages[i].next;

			while( page != 0 )
			{
				tpage = page;
				page = page->next;
				vfree( tpage );
			}
		}
	}
	vfree( map );
}
