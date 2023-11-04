#include "task.h"

#include <map>

namespace
{
	std::map<std::string, std::function<std::shared_ptr<xsky::task>()>> & task_map()
	{
		static std::map<std::string, std::function<std::shared_ptr<xsky::task>()>> map;
		return map;
	}
}

xsky::task::task()
	:_id( 0 )
{

}

xsky::task::~task()
{

}

void xsky::task::init( const std::shared_ptr<xsky::device> & dev, std::uint64_t id, std::istream & is, std::size_t size )
{
	_id = id;
	_dev = dev;
	_data.resize( size );
	is.read( (char *)_data.data(), size );
}

void xsky::task::close()
{

}

std::uint64_t xsky::task::id() const
{
	return _id;
}

const std::shared_ptr<xsky::device> & xsky::task::device() const
{
	return _dev;
}

std::shared_ptr<xsky::task> xsky::task::create_task( const std::string & type )
{
	return task_map()[type]();
}

void xsky::task::register_task( const std::string & type, const std::function<std::shared_ptr<xsky::task>()> & callback )
{
	task_map()[type] = callback;
}
