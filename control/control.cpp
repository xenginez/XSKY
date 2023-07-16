#include "control.h"

#include <iostream>

#define CMD_ROUTE_SKY  _IOW('d', 3, unsigned int)

control::control()
{
	open();
}

control::~control()
{
	close();
}

void control::open()
{
	close();

	fd = ::open( "/dev/sky_dev", O_RDWR );

	std::cout << "open fd: " << fd << std::endl;
}

void control::close()
{
	if ( fd != -1 )
	{
		std::cout << "close fd: " << fd << std::endl;

		::close( fd );
		fd = -1;
	}
}

void control::address( unsigned int addr )
{
	if ( fd != -1 )
	{
		ioctl( fd, CMD_ROUTE_SKY, addr );

		std::cout << "address fd: " << fd
			<< " ip: " << ( (unsigned char *)&addr )[0] << "." << ( (unsigned char *)&addr )[1] << "." << ( (unsigned char *)&addr )[2] << "." << ( (unsigned char *)&addr )[3] << std::endl;
	}
}
