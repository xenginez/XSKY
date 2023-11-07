#include "device.h"

#include <concurrentqueue.h>

struct xsky::device::private_p
{
	std::function<void( std::uint8_t *, std::uint32_t )> callback;
};

xsky::device::device()
	: _p( new private_p )
{
}

xsky::device::~device()
{
	delete _p;
}

void xsky::device::open( const std::function<void( std::uint8_t *, std::uint32_t )> & callback )
{
	_p->callback = callback;
}

void xsky::device::close()
{

	_p->callback = nullptr;
}

void xsky::device::ioctl( unsigned long cmd, unsigned long arg )
{
}

void xsky::device::write( std::uint64_t id, std::uint8_t cmd, std::uint8_t * data, std::uint32_t size )
{
}
