#pragma once

#include <memory>
#include <common.h>

namespace xsky
{
	class apnic
	{
	private:
		struct private_p;

	public:
		apnic();

		~apnic();

	public:
		void init();

		void release();

	public:
		bool is_cn( ipv4_addr ip ) const;

		bool is_cn( ipv6_addr ip ) const;

	private:
		void sync();

	private:
		private_p * _p;
	};
}