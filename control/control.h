/*!
 * \file	control.h
 *
 * \author	ZhengYuanQing
 * \date	2023/03/20
 * \email	zhengyuanqing.95@gmail.com
 *
 */
#ifndef CONTROL_H__BC3A96D7_6AD2_4306_B74D_BE6240EFFB19
#define CONTROL_H__BC3A96D7_6AD2_4306_B74D_BE6240EFFB19

#include <asio.hpp>

class control
{
public:
	control();

	~control();

public:
	void open();

	void close();

	void address( unsigned int addr );

private:
	int fd = -1;
};

#endif//CONTROL_H__BC3A96D7_6AD2_4306_B74D_BE6240EFFB19
