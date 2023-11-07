/*!
 * \file	controller.h
 *
 * \author	ZhengYuanQing
 * \date	2023/11/04
 * \email	zhengyuanqing.95@gmail.com
 *
*/
#ifndef CONTROLLER_H__126675C1_8703_432D_B00C_2D70A0358376
#define CONTROLLER_H__126675C1_8703_432D_B00C_2D70A0358376

#include <memory>

namespace xsky
{
	class controller
	{
	private:
		struct private_p;

	private:
		controller();

		~controller();

	public:
		static controller * instance();

	public:
		void init();

		void release();

	private:
		void accept();

		void config();

	private:
		void create( std::uint8_t * data, std::uint32_t size );

	private:
		private_p * _p;
	};
}

#endif//CONTROLLER_H__126675C1_8703_432D_B00C_2D70A0358376