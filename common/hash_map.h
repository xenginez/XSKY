#ifndef __NAT_MAP_H__
#define __NAT_MAP_H__

struct hash_map;

struct hash_map * create_hash_map();

void insert_hash_map( struct hash_map * map, void * key, int key_len, void * value );

void * find_hash_map( struct hash_map * map, void * key, int key_len, );

void * remove_hash_map( struct hash_map * map, void * key, int key_len, );

void release_hash_map( struct hash_map * map );

#endif // !__NAT_MAP_H__
