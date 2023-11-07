#ifndef XSKY_QUEUE_H
#define XSKY_QUEUE_H

#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/spinlock.h>

#define MALLOC(size) vmalloc(size)
#define FREE(p) vfree(p)
#define XSKY_MTU 1501

struct xsky_queue_data
{
    struct xsky_queue_data * next;
    struct xsky_queue_data * previous;

    ssize_t size;
    char data[XSKY_MTU];
};

struct xsky_queue
{
    struct xsky_queue_data * begin;
    struct xsky_queue_data * end;
    struct xsky_queue_data * list;

    size_t have;
    size_t size;
    spinlock_t lock;
};

struct xsky_queue * xsky_queue_init( size_t qsize )
{
    struct xsky_queue * queue = MALLOC( sizeof( struct xsky_queue ) );
    struct xsky_queue_data * tmp_pre = NULL, * tmp = NULL;

    memset( queue, 0, sizeof( struct xsky_queue ) );

    for ( int n = 0; n < qsize; n++ )
    {
        tmp = MALLOC( sizeof( struct xsky_queue_data ) );
        memset( tmp, 0, sizeof( struct xsky_queue_data ) );
        if ( 0 == n )
        {
            queue->list = tmp;
        }
        else
        {
            tmp_pre->next = tmp;
            tmp->previous = tmp_pre;
        }

        tmp_pre = tmp;
    }

    queue->list->previous = tmp;
    tmp->next = queue->list;
    queue->size = qsize;

    spin_lock_init( &queue->lock );

    return queue;
}

int xsky_queue_push( struct xsky_queue * queue, char * data, size_t size )
{
    struct xsky_queue_data * tmp;

    if ( size > XSKY_MTU )
        return -1;

    if ( queue->size == queue->have )
        return -2;

    spin_lock( &queue->lock );

    if ( 0 == queue->have )
    {
        tmp = queue->list;
        tmp->size = size;
        memcpy( tmp->data, data, size );
        queue->begin = tmp;
    }
    else
    {
        tmp = queue->end;
        tmp = tmp->next;
        tmp->size = size;
        memcpy( tmp->data, data, size );
    }

    queue->end = tmp;
    queue->have += 1;

    spin_unlock( &queue->lock );

    return 0;
}

struct xsky_queue_data * xsky_queue_pop( struct xsky_queue * queue )
{
    struct xsky_queue_data * tmp = NULL;

    if ( 0 == queue->have )
        return tmp;

    spin_lock( &queue->lock );

    tmp = queue->begin;
    queue->begin = tmp->next;
    queue->have -= 1;

    spin_unlock( &queue->lock );

    return tmp;
}

void xsky_queue_reset( struct xsky_queue * queue )
{
    queue->have = 0;
}

void xsky_queue_release( struct xsky_queue * queue )
{
    struct xsky_queue_data * tmp_a = NULL, * tmp_b = NULL;
    size_t qsize = queue->size;

    for ( int n = 0; n < qsize; n++ )
    {
        if ( 0 == n )
            tmp_a = queue->list;

        tmp_b = tmp_a->next;
        FREE( tmp_a );

        tmp_a = tmp_b;
    }

    FREE( queue );
}

#endif //!XSKY_QUEUE_H
