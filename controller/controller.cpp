#include "controller.h"

#include <mutex>
#include <thread>
#include <condition_variable>

#include <common.h>
#include <asio.hpp>
#include <concurrentqueue.h>

#include "task.h"
#include "apnic.h"
#include "device.h"
#include "tasks/tasks.h"

struct task_pool
{
	task_pool()
		: exit( false )
	{
		for ( size_t i = 0; i < std::thread::hardware_concurrency(); i++ )
		{
			pool.emplace_back( [this]()
			{
				execute();
			} );
		}
	}

	~task_pool()
	{
		exit = true;

		cond.notify_all();

		for ( auto & it : pool )
		{
			if ( it.joinable() )
				it.join();
		}
	}

	void push( const std::shared_ptr<xsky::task> & task )
	{
		queue.enqueue( task );

		cond.notify_one();
	}

	void execute()
	{
		while ( !exit )
		{
			std::shared_ptr<xsky::task> task;

			if ( !queue.try_dequeue( task ) )
			{
				std::unique_lock<std::mutex> lock( mutex );
				cond.wait( lock );
			}

			if ( task != nullptr )
			{
				task->execute();
				task->close();
				task = nullptr;
			}
		}
	}

	bool exit;
	std::mutex mutex;
	std::condition_variable cond;
	std::vector<std::thread> pool;
	moodycamel::ConcurrentQueue<std::shared_ptr<xsky::task>> queue;
};

struct xsky::controller::private_p
{
	xsky::apnic apn;
	std::thread thread;
	asio::io_context io;
	asio::streambuf buf;
	std::shared_ptr<task_pool> pool;
	std::shared_ptr<xsky::device> dev;
	std::shared_ptr<asio::io_context::work> work;
	std::shared_ptr<asio::ip::tcp::socket> socket;
	std::shared_ptr<asio::ip::tcp::acceptor> accept;
};

xsky::controller::controller()
	: _p( new private_p )
{
}

xsky::controller::~controller()
{
	delete _p;
}

xsky::controller * xsky::controller::instance()
{
	static controller ctl;
	return &ctl;
}

void xsky::controller::init()
{
	_p->apn.init();
	_p->dev = std::make_shared<device>();
	_p->pool = std::make_shared<task_pool>();
	_p->dev->open( [this]( std::uint8_t * data, std::uint32_t size )
	{
		create( data, size );
	} );
	_p->work = std::make_shared<asio::io_context::work>( _p->io );
	_p->thread = std::thread( [this]()
	{
		_p->io.run();
	} );
	_p->accept = std::make_shared<asio::ip::tcp::acceptor>( _p->io, asio::ip::tcp::endpoint( asio::ip::address::from_string( "127.0.0.1" ), 48888 ) );

	accept();
}

void xsky::controller::release()
{
	_p->dev->close();
	_p->dev = nullptr;
	_p->pool = nullptr;

	_p->apn.release();

	_p->accept = nullptr;
	_p->work = nullptr;

	if ( _p->thread.joinable() )
		_p->thread.join();
}

void xsky::controller::accept()
{
	_p->socket = std::make_shared<asio::ip::tcp::socket>( _p->io );
	_p->accept->async_accept( *_p->socket, [this]( auto ec )
	{
		if ( !ec )
		{
			config();
		}
	} );
}

void xsky::controller::config()
{
	//_p->socket->async_receive( _p->buf,  );
}

void xsky::controller::create( std::uint8_t * data, std::uint32_t size )
{
	if ( size > sizeof( protocol_ipv4 * ) )
	{
		std::uint64_t id = *( (std::uint64_t *)data );
		data += sizeof( std::uint64_t );

		std::shared_ptr<task> task;
		protocol_tcp * tcp = nullptr;
		protocol_udp * udp = nullptr;

		if ( ( (protocol_ipv4 *)data )->version == IP_VERSION_4 )
		{
			if ( ( (protocol_ipv4 *)data )->protocol == IP_PROTO_TCP )
				tcp = (protocol_tcp *)( data + sizeof( protocol_ipv4 ) );
			else if ( ( (protocol_ipv4 *)data )->protocol == IP_PROTO_UDP )
				udp = (protocol_udp *)( data + sizeof( protocol_ipv4 ) );
		}
		else
		{
			if ( ( (protocol_ipv6 *)data )->nexthdr == IP_PROTO_TCP )
				tcp = (protocol_tcp *)( data + sizeof( protocol_ipv4 ) );
			else if ( ( (protocol_ipv6 *)data )->nexthdr == IP_PROTO_UDP )
				udp = (protocol_udp *)( data + sizeof( protocol_ipv4 ) );
			else if ( ( (protocol_ipv6 *)data )->nexthdr == IPV6_NEXTHDR_UDP )
				udp = (protocol_udp *)( data + sizeof( protocol_ipv4 ) );
		}

		if ( tcp != nullptr )
		{
			if ( ::ntohs( tcp->dest ) == HTTP_FIXED_PORT )
				task = std::make_shared<http_task>();
			else if ( ::ntohs( tcp->dest ) == HTTPS_FIXED_PORT )
				task = std::make_shared<https_task>();
			else
				task = std::make_shared<tcp_task>();
		}
		else if ( udp != nullptr )
		{
			if ( ::ntohs( udp->dest ) == DNS_FIXED_PORT )
				task = std::make_shared<dns_task>();
			else if ( ::ntohs( udp->dest ) == HTTPS_FIXED_PORT )
				task = std::make_shared<https3_task>();
			else
				task = std::make_shared<udp_task>();
		}

		if ( task )
		{
			task->init( _p->dev, id, data, size );

			_p->pool->push( task );
		}
	}
}
