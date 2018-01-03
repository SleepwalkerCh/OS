#include "apilib.h"

void HariMain(void)
{
 char *hello = "hello, world\n";
       for(;;){
	api_putstr0(hello);
        }
	api_end();
}
