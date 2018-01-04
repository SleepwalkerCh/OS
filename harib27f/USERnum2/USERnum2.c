#include "apilib.h"
#include <stdio.h>
#include <string.h>

void HariMain(void)
{   
	char s[60];
	int rowNo;
	int i,h;
	int row_num=api_createrownum();
	rowNo=row_num;
	int MaxRow=api_createmaxrow();
	//api_product2();

	row_num=( row_num + 1 ) % MaxRow; 
	do{
		api_TASin(rowNo);
		api_product2();
		int compete_num=api_createcompete2();
		//ÁÙ½çÇø
		for(i=0;i<=30000;i++){
			sprintf(s,"%d",compete_num);
			api_putstr0(s);
		}
		//
		api_TASout(rowNo);
	} while (1==1);
	
}