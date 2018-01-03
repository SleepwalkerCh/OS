/* ?R???\?[????W */

#include "bootpack.h"
#include <stdio.h>
#include <string.h>

int compete_num=1;
int waiting[10];
int lock=0;
int row_num=0;
int buffersize=5;//total buffer
int listsize=0;//当前大小
int block=0;//lock
int empty=5;
int full=0;
int mutex=1;
int count=1;
int list[5] = {0, 0, 0, 0, 0};

void console_task(struct SHEET *sheet, int memtotal)
{
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, *fat = (int *) memman_alloc_4k(memman, 4 * 2880);/*FAT?????19.3???????????????????????*/
	struct CONSOLE cons;
	struct FILEHANDLE fhandle[8];
	char cmdline[30];
	unsigned char *nihongo = (char *) *((int *) 0x0fe8);

	cons.sht = sheet;
	cons.cur_x =  8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	task->cons = &cons;
	task->cmdline = cmdline;

	if (cons.sht != 0) {
		cons.timer = timer_alloc();
		timer_init(cons.timer, &task->fifo, 1);
		timer_settime(cons.timer, 50);
	}
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));//??????????§Ö?FAT?????
	for (i = 0; i < 8; i++) {
		fhandle[i].buf = 0;	/* ???g?p?}?[?N */
	}
	task->fhandle = fhandle;
	task->fat = fat;
	if (nihongo[4096] != 0xff) {	/* ???{??t?H???g?t?@?C?????????????H */
		task->langmode = 1;
	} else {
		task->langmode = 0;
	}
	task->langbyte1 = 0;

	/* ???????? */
	cons_putchar(&cons, '>', 1);

	for (;;) {
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons.sht != 0) { /* ????????? */
				if (i != 0) {
					timer_init(cons.timer, &task->fifo, 0); /* ??????? */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					timer_init(cons.timer, &task->fifo, 1); /* ?????? */
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_000000;
					}
				}
				timer_settime(cons.timer, 50);
			}
			if (i == 2) {	/* ???ON */
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* ???OFF */
				if (cons.sht != 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000,
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				cons.cur_c = -1;
			}
			if (i == 4) {	/* ?????? */
				cmd_exit(&cons, fat);
			}
			if (256 <= i && i <= 511) { /* ???????????????A?? */
				if (i == 8 + 256) {
					/* ???? */
					if (cons.cur_x > 16) {
						/* ?????????????????¦Ë */
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {
					/* Enter */
					/* ?????????? */
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);	/* ???????? */
					if (cons.sht == 0) {
						cmd_exit(&cons, fat);
					}
					/* ?v?????v?g?\?? */
					cons_putchar(&cons, '>', 1);
				} else {
					/* ?????? */
					if (cons.cur_x < 240) {
						/* ?????????????????¦Ë */
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			/* ?????????? */
			if (cons.sht != 0) {
				if (cons.cur_c >= 0) {
					boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, 
						cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				}
				sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
			}
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{//????????? 
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09) {	/* ?^?u */
		for (;;) {
			if (cons->sht != 0) {
				putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			}
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				break;	/* 32?????????break */
			}
		}
	} else if (s[0] == 0x0a) {	/* ???s */
		cons_newline(cons);
	} else if (s[0] == 0x0d) {	/* ???A */
		/* ??³¦??????????? */
	} else {	/* ???????? */
		if (cons->sht != 0) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		}
		if (move != 0) {
			/* move??0??????J?[?\????i???? */
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{//???? 
	int x, y;
	struct SHEET *sheet = cons->sht;
	struct TASK *task = task_now();
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16; /* ????s?? */
	} else {
		/* ?X?N???[?? */
		if (sheet != 0) {
			for (y = 28; y < 28 + 112; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
				}
			}
			for (y = 28 + 112; y < 28 + 128; y++) {
				for (x = 8; x < 8 + 240; x++) {
					sheet->buf[x + y * sheet->bxsize] = COL8_000000;
				}
			}
			sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
		}
	}
	cons->cur_x = 8;
	if (task->langmode == 1 && task->langbyte1 != 0) {
		cons->cur_x = 16;
	}
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{//???????????????????? 
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{//????????????????? 
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_runcmd(char *cmdline/*??????????*/, struct CONSOLE *cons, int *fat, int memtotal)
{//?????????? 
	if (strcmp(cmdline, "mem") == 0 && cons->sht != 0) {
		cmd_mem(cons, memtotal);//?????????(total,free)
	} else if (strcmp(cmdline, "cls") == 0 && cons->sht != 0) {
		cmd_cls(cons);//????
	} else if (strcmp(cmdline, "dir") == 0 && cons->sht != 0) {
		cmd_dir(cons);//????
	} else if (strcmp(cmdline, "exit") == 0) {
		cmd_exit(cons, fat);//?????????
	} else if (strncmp(cmdline, "start ", 6) == 0) {
		cmd_start(cons, cmdline, memtotal);//???cmd
	} else if (strncmp(cmdline, "ncst ", 5) == 0) {
		cmd_ncst(cons, cmdline, memtotal);//
	} else if (strncmp(cmdline, "langmode ", 9) == 0) {
		cmd_langmode(cons, cmdline);
	} else if (strcmp(cmdline,"add1") == 0){
		cmd_add1(cons);//?§à??? ??
	} else if (strcmp(cmdline,"min1") == 0){
		cmd_min1(cons);//?§à??? ??
	} else if (strcmp(cmdline,"num1") == 0){
		cmd_num1(cons);//????????? ??
	} else if (strcmp(cmdline,"num2") == 0){
		cmd_num2(cons);//????????? ??
	} else if (strcmp(cmdline,"num3") == 0){
		cmd_num3(cons);//????????? ??
	} else if (strcmp(cmdline,"producer") == 0){
		producer(cons);//????????? ??
	} else if (strcmp(cmdline,"consumer") == 0){
		consumer(cons);//????????? ??
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			/* ???????????????§Ô??????????? */
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}
int TestAndSet(int *target)
{
	int rv=*target;
	*target=1;
	return rv;
}
void cmd_add1(struct CONSOLE *cons)
{   
	char s[60];
	int waste_time;
	int i;
	sprintf(s,"num is: %d\n",compete_num);
	cons_putstr0(cons, s);
	//waiting flag<=0 ;
	
	compete_num+=1;
	for(i=0;i<=900000000;i++){
		waste_time+=1;
	}

	//signal
	sprintf(s,"num+1 is: %d\n",compete_num);
	cons_putstr0(cons, s);
}

void cmd_min1(struct CONSOLE *cons)
{   
	char s[60];
	int waste_time;
	int i;
	sprintf(s,"num is: %d\n",compete_num);
	cons_putstr0(cons, s);
	//waiting flag<=0;
	
	compete_num-=1;
	for(i=0;i<=900000000;i++){
		waste_time+=1;
	} 

	//signal
	sprintf(s,"num-1 is: %d\n",compete_num);
	cons_putstr0(cons, s);
}
void TASin(int No)
{
	int key;
	waiting[No]=1;
	key=1;
	while (waiting[No] && key)
		key=TestAndSet(&lock);
	waiting[No]=0;
}
void TASout(int No)
{
	int j;
	j=(No+1)%MaxRow;
	while ((j!=No) && !waiting[j])
		j=(j+1)%MaxRow;
	if (j==No)
		lock=0;
	else
		waiting[j]=0;	
}
void cmd_num1(struct CONSOLE *cons)
{   
	char s[60];
	//int waste_time;
	int rowNo;
	int i;
	//int j;
	//int key;
	rowNo=row_num;
	row_num=( row_num + 1 ) % MaxRow; 
	do{
		TASin(rowNo);

		//ÁÙ½çÇø
		compete_num=1;
		for(i=0;i<=30000;i++){
			sprintf(s,"%d",compete_num);
			cons_putstr0(cons, s);
		}
		//
		TASout(rowNo);
	} while (1==1);
	
}

void cmd_num2(struct CONSOLE *cons)
{   
	char s[60];
	//int waste_time;
	int rowNo;
	int i;
	//int j;
	//int key;
	rowNo=row_num;
	row_num=( row_num + 1 ) % MaxRow;
	do{
		TASin(rowNo);

		//ÁÙ½çÇø
		compete_num=2;
		for(i=0;i<=30000;i++){
			sprintf(s,"%d",compete_num);
			cons_putstr0(cons, s);
		}
		//
		TASout(rowNo);
	} while (1==1);
	
}
 
void cmd_num3(struct CONSOLE *cons)
{   
	char s[60];
	//int waste_time;
	int rowNo;
	int i;
	//int j;
	//int key;
	rowNo=row_num;
	row_num=( row_num + 1 ) % MaxRow; 
	do{

	
		TASin(rowNo);

		//ÁÙ½çÇø
		compete_num=3;
		for(i=0;i<=30000;i++){
			sprintf(s,"%d",compete_num);
			cons_putstr0(cons, s);
		}
		//
		TASout(rowNo);
	} while (1==1);
	
}
//////////////////////////////////////////////////
void wait(int *sem){
	char s[20];
	while (*sem<=0){
		sprintf(s,"%d",sem);
	}
		
	
	*sem-=1;
}
void signal(int *sem){
	*sem+=1;
}

void producer(struct CONSOLE *cons){
	int i,j;
	char s[10];
	do{
		wait(&empty);

		wait(&mutex);

		sprintf(s,"producing\n");
		cons_putstr0(cons, s);

		//向数组list头部写入一个值
		for (i = 0; i < buffersize;i++)
		{
			if (list[i]==0) break;
		}
		j=i;
		for (i=j;i>0;i--)
		{
			list[i]=list[i-1];
		}
		list[0]=count++;

		for (i = 0; i < 300000000; ++i);
	
		//print输出数组内容
		for (i = 0; i < buffersize; i++)
		{
			sprintf(s,"%d",list[i]);
			cons_putstr0(cons, s);
		}
		sprintf(s,"\n");
		cons_putstr0(cons, s);

		sprintf(s,"finish\n");
		cons_putstr0(cons, s);

		
		

		signal(&mutex);
		signal(&full);
		for (i = 0; i < 30000000; ++i);
	}while(1==1);
}

void consumer(struct CONSOLE *cons){
	char s[10];
	int i;
	do{
		wait(&full);
		wait(&mutex);

		sprintf(s,"consuming\n");
		cons_putstr0(cons, s);

		//删除数组最后一个内容
		for (i = buffersize-1; i >=0; i--)
		{
			if(list[i]!=0){
				list[i]=0;
				break;
			}
		}
		

		for (i = 0; i < 300000000; ++i);

		//print输出数组内容
		for (i = 0; i < buffersize; i++)
		{
			sprintf(s,"%d",list[i]);
			cons_putstr0(cons, s);
		}
		sprintf(s,"\n");
		cons_putstr0(cons, s);

		sprintf(s,"finish\n");
		cons_putstr0(cons, s);

		signal(&mutex);
		signal(&empty);	
		for (i = 0; i < 30000000; ++i);
	}while(1==1);
}

////////////////////////////////////////////////

void cmd_mem(struct CONSOLE *cons, int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_exit(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *) 0x0fec);
	if (cons->sht != 0) {
		timer_cancel(cons->timer);
	}
	memman_free_4k(memman, (int) fat, 4 * 2880);
	io_cli();
	if (cons->sht != 0) {
		fifo32_put(fifo, cons->sht - shtctl->sheets0 + 768);	/* 768?`1023 */
	} else {
		fifo32_put(fifo, task - taskctl->tasks0 + 1024);	/* 1024?`2023 */
	}
	io_sti();
	for (;;) {
		task_sleep(task);
	}
}

void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	/* ????????????????????????????????§Õ????? */
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct TASK *task = open_constask(0, memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	/* ????????????????????????????????§Õ????? */
	for (i = 5; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	/* Enter */
	cons_newline(cons);
	return;
}

void cmd_langmode(struct CONSOLE *cons, char *cmdline)
{
	struct TASK *task = task_now();
	unsigned char mode = cmdline[9] - '0';
	if (mode <= 2) {
		task->langmode = mode;
	} else {
		cons_putstr0(cons, "mode number error.\n");
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	char name[18], *p, *q,*address;
	struct TASK *task = task_now();
	int i, segsiz, datsiz, esp, dathrb, appsiz;
	struct SHTCTL *shtctl;
	struct SHEET *sht;
    
	/* ??????????????????? */
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; /* ?????????????????? */

	/* ?????? */
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		/* ?????????????????????????.hrb */
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
		/*???????*/ 
	}

	if (finfo != 0) {
		/* ?????? */
		appsiz = finfo->size; 
		p = file_loadfile2(finfo->clustno, &appsiz, fat);//??????????????? 
		if (appsiz >= 36 && strncmp(p + 4, "Hari", 4) == 0 && *p == 0x00) {/*22.5*/
			//???????bin2hrb?????hrb???????4~7????????"Hari" 
			segsiz = *((int *) (p + 0x0000));//????????????¨®????????????¦Ä?§³ 
			esp    = *((int *) (p + 0x000c));//ESP??????????????????????? 
			datsiz = *((int *) (p + 0x0010));//hrb??????????????§³ 
			dathrb = *((int *) (p + 0x0014));//hrb???????????????? 
			q = (char *) memman_alloc_4k(memman, segsiz);
			task->ds_base = (int) q;
			sprintf(address,"The ds_base of q is %x",q);
			cons_putstr0(cons, address);
			set_segmdesc(task->ldt + 0, appsiz - 1, (int) p, AR_CODE32_ER + 0x60);
			set_segmdesc(task->ldt + 1, segsiz - 1, (int) q, AR_DATA32_RW + 0x60);
			for (i = 0; i < datsiz; i++) {
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
			
			shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
			for (i = 0; i < MAX_SHEETS; i++) {
				sht = &(shtctl->sheets0[i]);
				if ((sht->flags & 0x11) == 0x11 && sht->task == task) {
					/* ???????¨®??????????? */
					sheet_free(sht);	/* ??? */
				}
			}
			for (i = 0; i < 8; i++) {	/* ?N???[?Y???????t?@?C????N???[?Y */
				if (task->fhandle[i].buf != 0) {
					memman_free_4k(memman, (int) task->fhandle[i].buf, task->fhandle[i].size);
					task->fhandle[i].buf = 0;
				}
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman, (int) q, segsiz);
			task->langbyte1 = 0;
		} else {
			cons_putstr0(cons, ".hrb file format error.\n");
		}
		memman_free_4k(memman, (int) p, appsiz);
		cons_newline(cons);
		return 1;
	}
	/* ?t?@?C??????????????????? */
	return 0;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct TASK *task = task_now();
	int ds_base = task->ds_base;
	struct CONSOLE *cons = task->cons;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht;
	struct FIFO32 *sys_fifo = (struct FIFO32 *) *((int *) 0x0fec);
	int *reg = &eax + 1;	/* eax??????n */
		/* ?????????PUSHAD?????????????? */
		/* reg[0] : EDI,   reg[1] : ESI,   reg[2] : EBP,   reg[3] : ESP */
		/* reg[4] : EBX,   reg[5] : EDX,   reg[6] : ECX,   reg[7] : EAX */
	int i;
	struct FILEINFO *finfo;
	struct FILEHANDLE *fh;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + ds_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + ds_base, ecx);
	} else if (edx == 4) {
		return &(task->tss.esp0);
	} else if (edx == 5) {
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x10;
		sheet_setbuf(sht, (char *) ebx + ds_base, esi, edi, eax);
		make_window8((char *) ebx + ds_base, esi, edi, (char *) ecx + ds_base, 0);
		sheet_slide(sht, ((shtctl->xsize - esi) / 2) & ~3, (shtctl->ysize - edi) / 2);
		sheet_updown(sht, shtctl->top); /* ????}?E?X?????????????•??w??F ?}?E?X????????? */
		reg[7] = (int) sht;
	} else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + ds_base);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		memman_init((struct MEMMAN *) (ebx + ds_base));
		ecx &= 0xfffffff0;	/* 16?o?C?g?P??? */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16?o?C?g?P?????¥° */
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; /* 16?o?C?g?P?????¥° */
		memman_free((struct MEMMAN *) (ebx + ds_base), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	} else if (edx == 13) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		hrb_api_linewin(sht, eax, ecx, esi, edi, ebp);
		if ((ebx & 1) == 0) {
			if (eax > esi) {
				i = eax;
				eax = esi;
				esi = i;
			}
			if (ecx > edi) {
				i = ecx;
				ecx = edi;
				edi = i;
			}
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 14) {
		sheet_free((struct SHEET *) ebx);
	} else if (edx == 15) {
		for (;;) {
			io_cli();
			if (fifo32_status(&task->fifo) == 0) {
				if (eax != 0) {
					task_sleep(task);	/* FIFO???????Q???? */
				} else {
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1 && cons->sht != 0) { /* ?J?[?\???p?^?C?} */
				/* ?A?v?????s????J?[?\?????o??????A???????\???p??1?????????? */
				timer_init(cons->timer, &task->fifo, 1); /* ????1?? */
				timer_settime(cons->timer, 50);
			}
			if (i == 2) {	/* ?J?[?\??ON */
				cons->cur_c = COL8_FFFFFF;
			}
			if (i == 3) {	/* ?J?[?\??OFF */
				cons->cur_c = -1;
			}
			if (i == 4) {	/* ?R???\?[???????????? */
				timer_cancel(cons->timer);
				io_cli();
				fifo32_put(sys_fifo, cons->sht - shtctl->sheets0 + 2024);	/* 2024?`2279 */
				cons->sht = 0;
				io_sti();
			}
			if (i >= 256) { /* ?L?[?{?[?h?f?[?^?i?^?X?NA?o?R?j??? */
				reg[7] = i - 256;
				return 0;
			}
		}
	} else if (edx == 16) {
		reg[7] = (int) timer_alloc();
		((struct TIMER *) reg[7])->flags2 = 1;	/* ?????L?????Z???L?? */
	} else if (edx == 17) {
		timer_init((struct TIMER *) ebx, &task->fifo, eax + 256);
	} else if (edx == 18) {
		timer_settime((struct TIMER *) ebx, eax);
	} else if (edx == 19) {
		timer_free((struct TIMER *) ebx);
	} else if (edx == 20) {
		if (eax == 0) {
			i = io_in8(0x61);
			io_out8(0x61, i & 0x0d);
		} else {
			i = 1193180000 / eax;
			io_out8(0x43, 0xb6);
			io_out8(0x42, i & 0xff);
			io_out8(0x42, i >> 8);
			i = io_in8(0x61);
			io_out8(0x61, (i | 0x03) & 0x0f);
		}
	} else if (edx == 21) {
		for (i = 0; i < 8; i++) {
			if (task->fhandle[i].buf == 0) {
				break;
			}
		}
		fh = &task->fhandle[i];
		reg[7] = 0;
		if (i < 8) {
			finfo = file_search((char *) ebx + ds_base,
					(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
			if (finfo != 0) {
				reg[7] = (int) fh;
				fh->size = finfo->size;
				fh->pos = 0;
				fh->buf = file_loadfile2(finfo->clustno, &fh->size, task->fat);
			}
		}
	} else if (edx == 22) {
		fh = (struct FILEHANDLE *) eax;
		memman_free_4k(memman, (int) fh->buf, fh->size);
		fh->buf = 0;
	} else if (edx == 23) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			fh->pos = ebx;
		} else if (ecx == 1) {
			fh->pos += ebx;
		} else if (ecx == 2) {
			fh->pos = fh->size + ebx;
		}
		if (fh->pos < 0) {
			fh->pos = 0;
		}
		if (fh->pos > fh->size) {
			fh->pos = fh->size;
		}
	} else if (edx == 24) {
		fh = (struct FILEHANDLE *) eax;
		if (ecx == 0) {
			reg[7] = fh->size;
		} else if (ecx == 1) {
			reg[7] = fh->pos;
		} else if (ecx == 2) {
			reg[7] = fh->pos - fh->size;
		}
	} else if (edx == 25) {
		fh = (struct FILEHANDLE *) eax;
		for (i = 0; i < ecx; i++) {
			if (fh->pos == fh->size) {
				break;
			}
			*((char *) ebx + ds_base + i) = fh->buf[fh->pos];
			fh->pos++;
		}
		reg[7] = i;
	} else if (edx == 26) {
		i = 0;
		for (;;) {
			*((char *) ebx + ds_base + i) =  task->cmdline[i];
			if (task->cmdline[i] == 0) {
				break;
			}
			if (i >= ecx) {
				break;
			}
			i++;
		}
		reg[7] = i;
	} else if (edx == 27) {
		reg[7] = task->langmode;
	} else if (edx==123456789) {
		cons_putstr0(cons,"xixixix");
		*((char *)0x000643f7) = 'b';
	}
	  
	return 0;
}

int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0C :\n Stack Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* ???I???????? */
}

int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	char s[30];
	cons_putstr0(cons, "\nINT 0D :\n General Protected Exception.\n");
	sprintf(s, "EIP = %08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0);	/* ???I???????? */
}

void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col)
{
	int i, x, y, len, dx, dy;

	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx < 0) {
		dx = - dx;
	}
	if (dy < 0) {
		dy = - dy;
	}
	if (dx >= dy) {
		len = dx + 1;
		if (x0 > x1) {
			dx = -1024;
		} else {
			dx =  1024;
		}
		if (y0 <= y1) {
			dy = ((y1 - y0 + 1) << 10) / len;
		} else {
			dy = ((y1 - y0 - 1) << 10) / len;
		}
	} else {
		len = dy + 1;
		if (y0 > y1) {
			dy = -1024;
		} else {
			dy =  1024;
		}
		if (x0 <= x1) {
			dx = ((x1 - x0 + 1) << 10) / len;
		} else {
			dx = ((x1 - x0 - 1) << 10) / len;
		}
	}

	for (i = 0; i < len; i++) {
		sht->buf[(y >> 10) * sht->bxsize + (x >> 10)] = col;
		x += dx;
		y += dy;
	}

	return;
}
