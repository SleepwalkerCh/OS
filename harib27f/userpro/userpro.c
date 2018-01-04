#include "apilib.h"

void HariMain(void)
{
	api_producer();  //从用户态启动producer
	api_end();
}