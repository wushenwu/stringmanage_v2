#ifndef  TABLE_H_6AEDF8D6_CF88_44ce_9E06_CCF5E6129889
#define  TABLE_H_6AEDF8D6_CF88_44ce_9E06_CCF5E6129889

/*
Function: 将record代表的(size, start)记录有序的插入到g_add_table中
　　  
Param	:  table, record
　　        
　　        
Return  :  None.

Effect   : 更新g_add_table
*/
void
insertintoaddtable(struct tag_table *table, struct tag_record *record);

/*
Function: 将record代表的(size, start)记录有序的插入到g_del_table中, 并对g_del_table指示连续空闲空间进行整合
　　  
Param	:  table[*max]
　　        size, start
　　        
Return  :  None.

Effect   : 更新g_del_table, 对g_del_table指示的连续空闲空间进行整合，进行再分配
*/
void
insertintodeltable(struct tag_table *table, struct tag_record *record);

/*
Function: 将index 所标识的(size, start)记录从table中删除, 并保证table的有序性。
　　  
Param	: table, index
　　        
Return  :  None.

Effect  : 更新table;
*/
void
removefromtable(struct tag_table *table, int index);

/*
Function: 将table中相应index所标识的(size, start)记录更新为新传入的record。
　　  
Param	: 
　　        
Return  :  None.

Effect  :  更新table
*/
void
updatetable(struct tag_table *table, int index, struct tag_record *record);


/*
Function: get the table that index belongs to, and set the max value of this range.
           用于space_stat 中设置使用标志U, F

Param   : buf is the storage buf.
          index_add, index_del 为space_stat所传进的参数，
          代表目前对g_add_del, g_del_table所遍历到的下标。
          NO_USE_MALLOC, USE_MALLOC_V1中恒为(0,0)

*/
int
gettable(char *buf, int index, int *max, int index_add, int index_del);


#endif