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
		virtual void init( const std::shared_ptr<xsky::device> & dev, std::uint64_t id, std::uint8_t * data, std::uint32_t size );

		virtual void execute() = 0;

		virtual void close();

	public:
		std::uint64_t id() const;

		std::uint64_t size() const;

		std::uint8_t * data() const;

		const std::shared_ptr<device> & device() const;

	private:
		std::uint64_t _id;
		std::uint32_t _size;
		std::uint8_t * _data;
		std::shared_ptr<xsky::device> _dev;
	};
}

#endif//TASK_H__2B530015_471E_4D88_9246_4B050C5007E5