#include <stdio.h>
#include <string.h>
#include "memmanage.h"
#include "table.h"
#include "ui.h"

/*
0 1 2 3 ....i-1 i(the insert point) i+1 i+2....
*/
void
insertintoaddtable(struct tag_table *table, struct tag_record *record)
{
    int i = 0;
    int j = 0;

    //find the proper position
    for (i = table->nrecords; i >= 1; i--)
    {
        //i will be the insert point.
        if (table->records[i - 1].start <= record->start)
        {
            break;
        }
    }

    //now we need to move i, i+1, .. nrecords-1 to i+1, i+2,...nrecords
    for (j = table->nrecords; j >= i + 1; j--)
    {
        memcpy(&table->records[j], &table->records[j - 1], sizeof(struct tag_record));      
    }

    //now insert this record to i
    memcpy(&table->records[i], record, sizeof(struct tag_record));

    table->nrecords++;
}

/*
Function: 考虑g_del_table中，index记录与其前驱、后继是否为连续的空闲空间，进行整合，
           以获取更大的空闲空间，便于后续再分配
Param   : index

Return  : None

Effect  : 可能调用removefromtable，间接更新g_indexMAX_del

Caller: this function can only be called by insertintodeltable
*/
static void
reorganize(int index)
{
    struct tag_record record;
    int indexpre = -1;
    int sizepre  = -1;
    int startpre = -1;

    int indexaft = -1; 
    int sizeaft  = -1;
    int startaft = -1;

    int size     = -1;
    int start    = -1;
     
    size  = g_del_table.records[index].size;
    start = g_del_table.records[index].start;   
    
    //aft
    if ((index + 1) != g_del_table.nrecords)
    {
        indexaft = index + 1;
        sizeaft  = g_del_table.records[indexaft].size;
        startaft = g_del_table.records[indexaft].start;

        if (startaft == start + size)
        {
            record.size  = size + sizeaft;
            record.start = start;
            updatetable(&g_del_table, index, &record);
            removefromtable(&g_del_table, indexaft);            
            //showinfo("reorganize space aft happens\r\n");
        }
    }//end for aft
    
    //pre
    if (index != 0)
    {
        indexpre = index - 1;  
        sizepre  = g_del_table.records[indexpre].size;
        startpre = g_del_table.records[indexpre].start;
        
        if (start == startpre + sizepre)
        {
            //take care, may size already updated, we need to use g_del_table[index]
            record.size  = sizepre + g_del_table.records[index].size;
            record.start = startpre;
            updatetable(&g_del_table, indexpre, &record);
            removefromtable(&g_del_table, index);
            //showinfo("reorganize space pre happens\r\n");
        }   
    }//end for pre 
}


/*
    Yes, you may say this seems the same with the insertintoaddtable.
    Yes, it is, apart from the reorganize(index)
    U can make it more beautiful
*/
void
insertintodeltable(struct tag_table *table, struct tag_record *record)
{
    int i = 0;
    int j = 0;
    
    //find the proper position
    for (i = table->nrecords; i >= 1; i--)
    {
        if (table->records[i - 1].start <= record->start)
        {
            break;
        }
    }
    
     //now we need to move i, i+1, .. nrecords-1 to i+1, i+2,...nrecords
    for (j = table->nrecords; j >= i + 1; j--)
    {
        memcpy(&table->records[j], &table->records[j - 1], sizeof(struct tag_record));      
    }
    
    //now insert this record
    memcpy(&table->records[i], record, sizeof(struct tag_record));

    table->nrecords++;

    reorganize(i);
}

void
removefromtable(struct tag_table *table, int index)
{
    int nrecords = 0;
    int i        = 0;

    nrecords = table->nrecords;

    //move index+1, index+2, ..., nrecords-1 to index, index+1, ..., nrecords-2
    for (i = index; i < nrecords - 1; i++)
    {
        memcpy(&table->records[i], &table->records[i+1], sizeof(struct tag_record));
    }
   
    memset(&table->records[nrecords - 1], 0, sizeof(struct tag_record));

    table->nrecords--;
}


void
updatetable(struct tag_table *table, int index, struct tag_record *record)
{
    table->records[index].size  = record->size;
    table->records[index].start = record->start;
}

int
gettable(char *buf, int index, int *max, int index_add, int index_del)
{
    int size_add  = -1;
    int start_add = -1;
    int size_del  = -1;
    int start_del = -1;
    int table     = -1;

    if (index_add != g_add_table.nrecords)
    {
        size_add  = g_add_table.records[index_add].size;
        start_add = g_add_table.records[index_add].start;
    }
        
    if (index_del != g_del_table.nrecords)
    {
        size_del  = g_del_table.records[index_del].size;
        start_del = g_del_table.records[index_del].start;
    }
 
#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
    //the start and index are relative to the base.
    if ((index >= start_add) && (index <= start_add + size_add))        
#else
    //index is relative to buf, start is absolute.
    if (((int)buf + index >= start_add) && ((int)buf + index <= start_add + size_add))
#endif
    {
        *max = start_add + size_add;
        return ADD;
    }

#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
    if ((index >= start_del) && (index <= start_del + size_del))
#else
    if (((int)buf + index >= start_del) && ((int)buf + index <= start_del + size_del))
#endif
    {
        *max = start_del + size_del;
        return DEL;
    }

    //should not come to this point.
    return table;
}