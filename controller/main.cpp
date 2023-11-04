#include <stdio.h>

#include "controller.h"

int main( int argc, char * argv[] )
{
	xsky::controller::instance()->init();

	while ( char c = getchar() )
	{
		if ( c == 'q' )
			break;
	}

	xsky::controller::instance()->release();

	return 0;
}
