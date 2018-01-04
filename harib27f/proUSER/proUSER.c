#include "apilib.h"
#include <stdio.h>
#include <string.h>

void HariMain(void)
{
	int i,j;
	char s[10];
	int buffersize = 5;
	int *addrempty = api_getempty();
	int *addrmutex = api_getmutex();
	//int *addrlist = api_getlist();
	int *addrfull = api_getfull();
	int count = api_getcount();
	int x,y;

	do{

		api_wait(addrempty);
		api_wait(addrmutex);

		sprintf(s,"producing\n");
		api_putstr0(s);

		//向数组list头部写入一个�?
		for (i = 0; i < buffersize; i++)
		{
			if (api_readlist(i)==0) break;
		}
		j=i;
		for (i=j;i>0;i--)
		{
			x = api_readlist(i-1);
			api_writelist(i, x);
			//addrlist[i]=addrlist[i-1];
		}
		//api_readlist[0]=count++;
		api_writelist(0, count++);

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
		api_signal(addrfull);
		for (i = 0; i < 30000000; ++i);
	}while(1==1);
}
