#include "apnic.h"

#include <thread>

#include <asio.hpp>

#include "mmap.h"

namespace
{
	static constexpr const char ftp_apnic_net_url[] = "ftp.apnic.net";
	static constexpr const char ftp_apnic_net_http[] =
		"GET /apnic/stats/apnic/delegated-apnic-latest HTTP/1.1\r\n"
		"Host: ftp.apnic.net\r\n"
		"Connection: keep-alive\r\n"
		"Cache-Control: max-age=0\r\n"
		"Upgrade-Insecure-Requests: 1\r\n"
		"DNT: 1\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36 Edg/119.0.0.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n\r\n";
}

struct xsky::apnic::private_p
{
	mmap _ipv4;
	mmap _ipv6;

	bool _exit = false;
	std::thread _thread;
	std::shared_ptr<asio::io_context> _io;
	std::shared_ptr<asio::ip::tcp::socket> _socket;
};

xsky::apnic::apnic()
	: _p( new private_p )
{
}

xsky::apnic::~apnic()
{
	delete _p;
}

void xsky::apnic::init()
{
	_p->_exit = false;
	_p->_ipv4.open( std::filesystem::current_path() / "apnic_ipv4", (size_t)1024 * 1024 * 64 );
	_p->_ipv6.open( std::filesystem::current_path() / "apnic_ipv6", (size_t)1024 * 1024 * 1024 * 4 );
	sync();
}

void xsky::apnic::release()
{
	_p->_ipv4.close();
	_p->_ipv6.close();

	_p->_exit = true;
	if ( _p->_thread.joinable() )
		_p->_thread.join();
}

bool xsky::apnic::is_cn( ipv4_addr ip ) const
{
	return _p->_ipv4.read( ( ip & 0xFFFFFF ) >> 8 );
}

bool xsky::apnic::is_cn( ipv6_addr ip ) const
{
	return _p->_ipv6.read( ip.addr32[0] );
}

void xsky::apnic::sync()
{
	_p->_io = std::make_shared<asio::io_context>();
	_p->_socket = std::make_shared < asio::ip::tcp::socket >( *_p->_io );

	_p->_thread = std::thread( [this]()
	{
		std::error_code ec;
		asio::ip::tcp::resolver resolver( *_p->_io );
		auto result = resolver.resolve( asio::ip::tcp::v4(), ftp_apnic_net_url, "http" );
		for ( auto it : result )
		{
			_p->_socket->connect( it.endpoint(), ec );
			if ( !ec )
				break;
		}

		if ( _p->_socket->is_open() )
		{
			_p->_socket->write_some( asio::buffer( ftp_apnic_net_http ), ec );
			if ( !ec )
			{
				asio::streambuf buf;

				while ( !_p->_exit )
				{
					auto size = _p->_socket->read_some( asio::buffer( buf.prepare( 512 ) ), ec );
					if ( ec )
					{
						return;
					}

					buf.commit( size );

					std::string line, ip;
					std::istream is( &buf );
					while ( 1 )
					{
						ip.clear();
						line.clear();
						std::getline( is, line );

						if ( line.empty() )
							break;

						auto pos = line.find( "apnic|CN|ipv" );
						if ( pos != std::string::npos )
						{
							pos += 12;
							auto beg = line.begin() + pos + 2;
							auto end = line.begin() + line.find( '|', pos + 2 );
							ip.assign( beg, end );

							if ( line[pos] == '4' )
							{
								_p->_ipv4.write( ( asio::ip::address::from_string( ip ).to_v4().to_uint() & 0xFFFFFF ) >> 8, true );
							}
							else if ( line[pos] == '6' )
							{
								auto bytes = asio::ip::address::from_string( ip ).to_v6().to_bytes();
								_p->_ipv6.write( *( (std::uint32_t *)bytes.data() ), true );
							}
						}
					}
				}
			}
		}
	} );
}
