/*!
 * \file	task.h
 *
 * \author	ZhengYuanQing
 * \date	2023/11/04
 * \email	zhengyuanqing.95@gmail.com
 *
*/
#ifndef TASK_H__2B530015_471E_4D88_9246_4B050C5007E5
#define TASK_H__2B530015_471E_4D88_9246_4B050C5007E5

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

#include "device.h"

namespace xsky
{
	class task : public std::enable_shared_from_this<task>
	{
	public:
		task();

		virtual ~task();

	public:
		virtual void init( const std::shared_ptr<xsky::device> & dev, std::uint64_t id, std::istream & is, std::size_t size );

		virtual void execute() = 0;

		virtual void close();

	public:
		std::uint64_t id() const;

		const std::shared_ptr<device> & device() const;

	public:
		template< typename T > struct registerd
		{
			registerd( const std::string & type )
			{
				task::register_task( type, []()
				{
					return std::make_shared<T>();
				} );
			}
		};
		static std::shared_ptr<task> create_task( const std::string & type );
		static void register_task( const std::string & type, const std::function<std::shared_ptr<task>()> & callback );

	private:
		std::uint64_t _id;
		std::vector<std::uint8_t> _data;
		std::shared_ptr<xsky::device> _dev;
	};
}

#endif//TASK_H__2B530015_471E_4D88_9246_4B050C5007E5