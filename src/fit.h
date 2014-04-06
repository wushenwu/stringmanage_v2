#ifndef FIT_H_7D199A32_98B9_4858_A226_045DC71B5CF7

#define FIT_H_7D199A32_98B9_4858_A226_045DC71B5CF7

//different ways you can use to get suitable size
#define		FIT_EQUAL				0
#define		FIT_FIRST				1
#define 	FIT_ENOUGH			    2
#define		FIT_EQUAL_ENOUGH	    3
#define		FIT_EQUAL_FIRST		    4


/*
Function: ��g_del_table���ҵ�һ��(size, start)��¼��ʹ��size = requestsize
Param	: requestsize ��������Ŀռ��С�� 
����      index���ڽ��շ�������(size, start) ��g_del_table�е��±�
Return  :  NOT_EXIST ʧ�ܣ� SUCCESS �ɹ���������index.
*/
int 
fitequal(int requestsize, int *index);

/*
Function: ��g_del_table���ҵ�һ��(size, start)��¼��ʹ��size �պ�>= 
����       requestsize��
Param	: requestsize ��������Ŀռ��С�� 
          index���ڽ��շ�������(size, start) ��g_del_table�е��±�
Return  :  NOT_EXIST ʧ�ܣ� SUCCESS �ɹ���������index.
*/
int 
fitenough(int requestsize, int *index);

/*
Function: ��g_del_table���ҵ�һ��(size, start)��¼��ʹ��size �ǵ�һ��>= 
����        requestsize��
Param	: requestsize ��������Ŀռ��С�� 
          index���ڽ��շ�������(size, start) ��g_del_table�е��±�
Return  :  NOT_EXIST ʧ�ܣ� SUCCESS �ɹ���������index.
*/
int 
fitfirst(int requestsize, int *index);

/*
Function: ����g_fitWAY����g_del_table���ҵ�һ�����ʵ�(size, start)��¼��
����      �Ƕ�����fitxxx�����ķ�װ��
���� 
Param	: requestsize ��������Ŀռ��С�� 
          index���ڽ��շ�������(size, start) ��g_del_table�е��±�
Return  :  NOT_EXIST ʧ�ܣ� SUCCESS �ɹ���������index.
*/
int 
getsize(int requestsize, int *index);

/*
Function: set the default way according to your input
*/
void
setfit(int select);

#endif