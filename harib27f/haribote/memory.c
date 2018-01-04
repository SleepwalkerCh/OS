/* �������֌W */

#include "bootpack.h"

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

unsigned int memtest(unsigned int start, unsigned int end)//��鷶Χ���ڴ���ô�С 
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* ȷ��CPU��386����486*/
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* ������386��ʹ�趨AC=1��AC���ǻ��Զ����� */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /*��ֹ���� */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* ������ */
		store_cr0(cr0);
	}

	return i;
}

void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			/* ������Ϣ��Ŀ */
	man->maxfrees = 0;		/* ���ڹ۲����״�� */
	man->lostsize = 0;		/* �ͷ�ʧ�ܵ��ڴ��С�ܺ� */
	man->losts = 0;			/* �ͷ�ʧ�ܵĴ��� */
	return;
}

unsigned int memman_total(struct MEMMAN *man)
/* ��������ڴ��С�ܺ� */
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
/* ���� */
{
	unsigned int i, a;
	for (i = man->frees - 1; i >= 0; i--) {
		if (man->free[i].size >= size) {
			/* �ҵ��㹻����ڴ� */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i].size���0�ͼ���һ��������Ϣ */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* ����ṹ�� */
				}
			}
			return a;
		}
	}
	return 0; /* �������Ȃ� */
}


int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
/* �ͷ��ڴ� */
{
	int i, j;
	/* Ϊ���ڹ����ڴ棬��free[i]����addr˳������ */
	/* ���ԣ��Ⱦ���Ӧ�÷������� */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* ǰ���п����ڴ� */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* ��ǰ����ڴ���ɵ�һ�� */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* ����Ҳ�� */
				if (addr + size == man->free[i].addr) {
					/* �����Ŀ����ڴ���ɵ�һ�� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]ɾ�� */
					/* free[i]���0����ɵ�ǰ��ȥ */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* �ṹ�帳ֵ */
					}
				}
			}
			return 0; /* �ɹ���� */
		}
	}
	/* ������ǰ��Ŀ��ÿռ���ɵ�һ�� */
	if (i < man->frees) {
		/* ���滹�� */
		if (addr + size == man->free[i].addr) {
			/* �ͺ���Ŀռ���ɵ�һ�� */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* �ɹ���� */
		}
	}
	/* �Ȳ�����ǰ��Ҳ���������ϲ���һ�� */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]֮��Ŀռ�����ƶ����ڳ�һ�����ÿռ� */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* �������ֵ */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* �ɹ���� */
	}
	/* ���пռ��Ƴ����²��������ƶ� */
	man->losts++;
	man->lostsize += size;
	return -1; /* ʧ�� */
}

unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size)
{//��0x1000��4k���ֽ�Ϊ��λ�����ڴ���� 
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size)
{//��0x1000��4k���ֽ�Ϊ��λ�����ڴ��ͷ�
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
