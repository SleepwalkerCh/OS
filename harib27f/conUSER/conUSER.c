#include "apilib.h"
#include <stdio.h>
#include <string.h>

void HariMain(void)
{
	char s[10];
	int i;
	int buffersize = 5;
	int *addrfull = api_getfull();
	int *addrmutex = api_getmutex();
	int *addrempty = api_getempty();
	//int *addrlist = api_getlist();
	do{

		api_wait(addrfull);
		api_wait(addrmutex);

		sprintf(s,"consuming\n");
		api_putstr0(s);

		//删除数组最后一个内�?
		for (i = buffersize-1; i >=0; i--)
		{
			if(api_readlist(i)!=0){
				int h = 0;
				api_writelist(i, h);
				break;
			}
		}
		

		for (i = 0; i < 300000000; ++i);

		//print输出数组内容
		for (i = 0; i < buffersize; i++)
		{
			sprintf(s,"%d",api_readlist(i));
			api_putstr0(s);
		}
		sprintf(s,"\n");
		api_putstr0(s);

		sprintf(s,"finish\n");
		api_putstr0(s);

		api_signal(addrmutex);
		api_signal(addrempty);	
		for (i = 0; i < 30000000; ++i);
	}while(1==1);
}
