#include "apilib.h"

void HariMain(void)
{
	api_add1();  //从用户态启动add1
	api_end();
}