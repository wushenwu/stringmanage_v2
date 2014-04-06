#ifndef  TABLE_H_6AEDF8D6_CF88_44ce_9E06_CCF5E6129889
#define  TABLE_H_6AEDF8D6_CF88_44ce_9E06_CCF5E6129889

/*
Function: ��record�����(size, start)��¼����Ĳ��뵽g_add_table��
����  
Param	:  table, record
����        
����        
Return  :  None.

Effect   : ����g_add_table
*/
void
insertintoaddtable(struct tag_table *table, struct tag_record *record);

/*
Function: ��record�����(size, start)��¼����Ĳ��뵽g_del_table��, ����g_del_tableָʾ�������пռ��������
����  
Param	:  table[*max]
����        size, start
����        
Return  :  None.

Effect   : ����g_del_table, ��g_del_tableָʾ���������пռ�������ϣ������ٷ���
*/
void
insertintodeltable(struct tag_table *table, struct tag_record *record);

/*
Function: ��index ����ʶ��(size, start)��¼��table��ɾ��, ����֤table�������ԡ�
����  
Param	: table, index
����        
Return  :  None.

Effect  : ����table;
*/
void
removefromtable(struct tag_table *table, int index);

/*
Function: ��table����Ӧindex����ʶ��(size, start)��¼����Ϊ�´����record��
����  
Param	: 
����        
Return  :  None.

Effect  :  ����table
*/
void
updatetable(struct tag_table *table, int index, struct tag_record *record);


/*
Function: get the table that index belongs to, and set the max value of this range.
           ����space_stat ������ʹ�ñ�־U, F

Param   : buf is the storage buf.
          index_add, index_del Ϊspace_stat�������Ĳ�����
          ����Ŀǰ��g_add_del, g_del_table�����������±ꡣ
          NO_USE_MALLOC, USE_MALLOC_V1�к�Ϊ(0,0)

*/
int
gettable(char *buf, int index, int *max, int index_add, int index_del);


#endif