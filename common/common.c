#include "common.h"

#define NAT_DATA_PAIR_SIZE 100
#define NAT_DATA_BUCKET_SIZE 100

typedef struct
{
	unsigned int key;
	void * value;
} pair;

typedef struct _bucket
{
	struct _bucket * next;

	unsigned long long used;
	pair pairs[NAT_DATA_PAIR_SIZE];
} bucket;

struct _hash_map
{
	bucket pages[NAT_DATA_BUCKET_SIZE];
};

unsigned int hash( void * key, int key_len )
{
	int i = 0;
	unsigned int _Val = 2166136261U;
	unsigned char * _First = (unsigned char *) key;

	for( i = 0; i < key_len; ++i )
	{
		_Val ^= (unsigned int) ( _First[i] );
		_Val *= 16777619U;
	}

	return _Val;
}


int config_info_length( const config_info * info )
{
	int i = 0;
	int size = sizeof( config_info ) + info->macs * sizeof( mac_info );

	domain_info * domain = (domain_info *) ( ( (unsigned char *) info ) + size );
	for( i = 0; i < info->domains; i++ )
	{
		size += sizeof( domain_info ) + domain->strlen;
		domain = (domain_info *) ( ( (unsigned char *) domain ) + sizeof( domain_info ) + domain->strlen );
	}

	return size;
}

int capture_info_length( const capture_info * info )
{
	return sizeof( capture_info );
}


hash_map create_hash_map( void )
{
	return (hash_map)vzalloc( sizeof( struct _hash_map ) );
}

void insert_hash_map( hash_map map, void * key, int key_len, void * value )
{
	int i = 0;
	unsigned int h = hash( key, key_len );

	bucket * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page->used == NAT_DATA_PAIR_SIZE )
	{
		if( page->next == 0 )
		{
			page->next = (bucket *)vzalloc( sizeof( bucket ) );
		}

		page = page->next;
	}

	for( i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
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

void * find_hash_map( hash_map map, void * key, int key_len )
{
	int i = 0;
	unsigned int h = hash( key, key_len );

	bucket * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page != 0 )
	{
		for( i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
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

void * remove_hash_map( hash_map map, void * key, int key_len )
{
	int i = 0;
	unsigned int h = hash( key, key_len );

	bucket * page = map->pages + ( h % NAT_DATA_BUCKET_SIZE );
	while( page != 0 )
	{
		for( i = 0; i < NAT_DATA_PAIR_SIZE; ++i )
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

void free_hash_map( hash_map map )
{
	int i = 0;
	bucket * page = 0, * tpage = 0;
	for( i = 0; i < NAT_DATA_BUCKET_SIZE; ++i )
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
