#include "apilib.h"

void HariMain(void)
{
	api_consumer();  //从用户态启动consumer
	api_end();
}