#include "device.h"

xsky::device::device()
{
}

xsky::device::~device()
{
}

void xsky::device::open( const std::function<void( std::uint8_t *, std::size_t )> & callback )
{
	_callback = callback;
}

void xsky::device::close()
{

	_callback = nullptr;
}

void xsky::device::ioctl( unsigned long cmd, unsigned long arg )
{
}
