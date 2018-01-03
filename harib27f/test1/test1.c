#include "apilib.h"
#include<string.h>
#include<stdio.h>
void HariMain(void)
{
        char* s;
        char a = 'a';
        sprintf(s,"The address of a is %x",&a);  
	api_putstr0(s);
        int i = 0;
        for(;1==1;i++){
           if(i%1000000==0)
             api_putchar(a);
        }

	  api_end();
	
}
