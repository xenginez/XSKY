/*!
 * \file	device.h
 *
 * \author	ZhengYuanQing
 * \date	2023/11/04
 * \email	zhengyuanqing.95@gmail.com
 *
*/
#ifndef DEVICE_H__E6DDB8F2_24ED_4439_A9B5_4A11EA72F4D7
#define DEVICE_H__E6DDB8F2_24ED_4439_A9B5_4A11EA72F4D7

#include <memory>
#include <functional>

namespace xsky
{
	class device
	{
	private:
		struct private_p;

	public:
		device();

		~device();

	public:
		void open( const std::function<void( std::uint8_t *, std::uint32_t )> & callback );

		void close();

		void ioctl( unsigned long cmd, unsigned long arg = 0 );

		void write( std::uint64_t id, std::uint8_t cmd, std::uint8_t * data, std::uint32_t size );

	private:
		private_p * _p;
	};
}

#endif//DEVICE_H__E6DDB8F2_24ED_4439_A9B5_4A11EA72F4D7