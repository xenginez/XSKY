#include <asio.hpp>

#include "control.h"

int main( int argc, char * argv[] )
{
	control ctl;

#ifdef CONFIG_ROUTE
	ctl.address( inet_addr( "119.28.88.21" ) );
#endif // CONFIG_ROUTE


	getchar();

	return 0;
}