#include "task.h"

#include <map>

xsky::task::task()
	:_id( 0 ), _size( 0 ), _data( nullptr )
{

}

xsky::task::~task()
{

}

void xsky::task::init( const std::shared_ptr<xsky::device> & dev, std::uint64_t id, std::uint8_t * data, std::uint32_t size )
{
	_id = id;
	_dev = dev;
	_size = size;
	_data = data;
}

void xsky::task::close()
{

}

std::uint64_t xsky::task::id() const
{
	return _id;
}

std::uint64_t xsky::task::size() const
{
	return _size;
}

std::uint8_t * xsky::task::data() const
{
	return _data;
}

const std::shared_ptr<xsky::device> & xsky::task::device() const
{
	return _dev;
}
