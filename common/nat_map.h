#ifndef __NAT_MAP_H__
#define __NAT_MAP_H__

struct nat_map;

struct nat_map * create_nat_map();

void insert_nat_map( struct nat_map * map, void * key, int key_len, void * value );

void * find_nat_map( struct nat_map * map, void * key, int key_len, );

void * remove_nat_map( struct nat_map * map, void * key, int key_len, );

void release_nat_map( struct nat_map * map );

#endif // !__NAT_MAP_H__
