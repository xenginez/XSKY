#pragma once

#include <filesystem>

namespace xsky
{
	class mmap
	{
	private:
		struct private_p;

	public:
		mmap();
		~mmap();

	public:
		bool open( const std::filesystem::path & path, std::size_t size );
		void close();

	public:
		bool read( std::size_t index ) const;
		void write( std::size_t index, bool value );

	private:
		private_p * _p;
	};
}