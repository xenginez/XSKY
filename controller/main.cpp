#include <stdio.h>

#include "controller.h"

int main( int argc, char * argv[] )
{
	controller::instance()->init();

	while ( char c = getchar() )
	{
		if ( c == 'q' )
			break;
	}

	controller::instance()->release();

	return 0;
}
