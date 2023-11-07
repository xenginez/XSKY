#include "mmap.h"

#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

struct xsky::mmap::private_p
{
#ifdef _WIN32
	void * _mapping = nullptr;
#endif

	void * _file = nullptr;
	std::size_t _size = 0;
	std::uint8_t * _address = nullptr;
};

xsky::mmap::mmap()
	: _p( new private_p )
{
}

xsky::mmap::~mmap()
{
	delete _p;
}
#ifdef _WIN32
bool xsky::mmap::open( const std::filesystem::path & path, std::size_t size )
{
	close();

	if ( !std::filesystem::exists( path ) )
	{
		std::ofstream ofs( path, std::ios::out | std::ios::binary );
		ofs.close();
	}

	auto file_ = ::CreateFileA( path.string().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	auto mapping_ = ::CreateFileMappingA( file_, NULL, PAGE_READWRITE, DWORD( size & 0xFFFFFFFF00000000 ), DWORD( size & 0x00000000FFFFFFFF ), NULL );
	if ( mapping_ != INVALID_HANDLE_VALUE && mapping_ != 0 )
	{
		_p->_size = size;
		_p->_file = file_;
		_p->_mapping = mapping_;
		_p->_address = static_cast<std::uint8_t *>( ::MapViewOfFile( mapping_, ( FILE_MAP_READ | FILE_MAP_WRITE ), 0, 0, 0 ) );

		return true;
	}

	return false;
}
void xsky::mmap::close()
{
	if ( _p->_address )
	{
		::UnmapViewOfFile( _p->_address );
		_p->_address = nullptr;
	}

	if ( _p->_mapping )
	{
		::CloseHandle( _p->_mapping );
		_p->_mapping = nullptr;
	}

	if ( _p->_file )
	{
		::CloseHandle( _p->_file );
		_p->_file = nullptr;
	}

	_p->_size = 0;
}

#else
bool xsky::mmap::open( const std::filesystem::path & path, std::size_t size )
{
	close();

	if ( !std::filesystem::exists( path ) )
	{
		std::ofstream ofs( path, std::ios::out | std::ios::binary );
		ofs.close();
	}

	auto file_ = open( path.string().c_str(), O_RDONLY );
	if ( file_ != -1 )
	{
		_p->_size = size;
		_p->_file = reinterpret_cast<void *>( file_ );
		_p->_address = static_cast<XE::uint8 *>( mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file_, 0 ) );

		return true;
	}

	return false;
}
void xsky::mmap::close()
{
	if ( _p->_address != MAP_FAILED )
	{
		munmap( _p->_address, _p->_size );
		_p->_address = nullptr;
	}

	int file_ = reinterpret_cast<int>( _p->_file );
	if ( file_ != -1 || file_ != 0 )
	{
		close( file_ );
		_p->_file = nullptr;
	}

	_p->_size = 0;
}

#endif // _WIN32

bool xsky::mmap::read( std::size_t index ) const
{
	return _p->_address[index / 8] & ( 1 << ( index % 8 ) );
}

void xsky::mmap::write( std::size_t index, bool value )
{
	if( value )
		_p->_address[index / 8] |= ( 1 << ( index % 8 ) );
	else
		_p->_address[index / 8] &= ~( 1 << ( index % 8 ) );
}
