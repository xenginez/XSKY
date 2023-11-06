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

struct task_pool
{
	task_pool()
		: exit( false )
	{
		for ( size_t i = 0; i < std::thread::hardware_concurrency(); i++ )
		{
			pool.emplace_back( [this]()
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
						task = nullptr;
					}
				}
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

	bool exit;
	std::mutex mutex;
	std::condition_variable cond;
	std::vector<std::thread> pool;
	moodycamel::ConcurrentQueue<std::shared_ptr<xsky::task>> queue;
};

struct xsky::controller::private_p
{
	xsky::apnic apn;
	xsky::device dev;
	std::thread thread;
	asio::io_context io;
	asio::streambuf buf;
	std::shared_ptr<task_pool> pool;
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
	_p->pool = std::make_shared<task_pool>();
	_p->dev.open( create );
	_p->apn.init();
	_p->work = std::make_shared<asio::io_context::work>( _p->io );
	_p->thread = std::thread( [this](){ _p->io.run(); } );
	_p->accept = std::make_shared<asio::ip::tcp::acceptor>( _p->io, asio::ip::tcp::endpoint( asio::ip::address::from_string( "127.0.0.1" ), 48888 ) );

	accept();
}

void xsky::controller::release()
{
	_p->pool = nullptr;
	_p->dev.close();

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

void xsky::controller::create( std::uint8_t * data, std::size_t size )
{
	if ( size > sizeof( protocol_ipv4 * ) )
	{
		auto ip = (protocol_ipv4 *)data;

	}
}
