#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memmanage.h"
#include "fit.h"
#include "table.h"
#include "ui.h"

/*storage for your input. can only be dealed within file*/
#if defined(NO_USE_MALLOC) 
    static char g_chBuf[MAX_SPACE];
    static int g_indexMAX_buf;  //how many strings already stored into this buf. 
#endif

#if defined(USE_MALLOC_V1) 
    static char *g_chBuf = NULL;
    static int g_indexMAX_buf;
#endif
 

/*used for maintain*/
struct tag_table g_add_table;
struct tag_table g_del_table;

/*char statistics ( those printable but not space) can only be dealed within file*/
static int g_char_table[MAX_CHAR];
static int g_totalchar;


int 
string_query(char *szinput, int *index)
{
    int i;
    int size;
    int result;
    int nrecords;

    result   = NOT_EXIST;
    size     = strlen(szinput) + 1;
    nrecords = g_add_table.nrecords;

    for (i = 0; i < nrecords; i++)
    {
        if (size != g_add_table.records[i].size)
        {
            continue;
        }

    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        //start is relative
        if (0 == strcmp(&g_chBuf[g_add_table.records[i].start], szinput))
    #else
        //start is absolute            
        if (0 == strcmp((char *)(g_add_table.records[i].start), szinput))
    #endif
        {
            *index = i;
            result = SUCCESS;
            break;
        }
    }

    return result;
}

int
string_query_about(char *szinput, int *indexary, int *num)
{
    int i;
    int size;
    int result;
    int nrecords;
    
    result = NOT_EXIST;    
    size = strlen(szinput) + 1;
    *num = 0;   //not a good idea to use the param
    
    nrecords = g_add_table.nrecords;

    for (i = 0; i < nrecords; i++)
    {
        if (size > g_add_table.records[i].size)
        {
            continue;
        }
 
    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        //start is relative
        if (NULL != strstr(&g_chBuf[g_add_table.records[i].start], szinput))
    #else
        //start is absolute
        if (NULL != strstr((char *)(g_add_table.records[i].start), szinput))
    #endif
        {
            *(indexary + *num) = i;
            *num += 1;
         }
    }

    if (0 != *num)
    {
        result = SUCCESS;
    }
    
    return result;
}

int
store_by_malloc(char *szinput, char **ppszstart)
{
    char *pszstart = NULL;
    size_t len     = 0;
    int result     = SUCCESS;

    len = strlen(szinput);
    
    pszstart = (char *)malloc(len + 1);
    if (NULL == pszstart)
    {
        result = NO_SPACE;
        return result;
    }
    else
    {
        strcpy(pszstart, szinput);
        *(pszstart + len) = '\0';

        *ppszstart = pszstart;

        return result;
    }
}

#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
int
store(char *chBuf, int start, char *szinput, int requestsize)
{
    int result = SUCCESS;

    //make sure g_indexMAX_buf updated
    if (start == g_indexMAX_buf)
    {
        if (MAX_SPACE == g_indexMAX_buf)
        {
            result = NO_SPACE;
            return result;            
        }
        g_indexMAX_buf += requestsize;
    }

    strcpy(&chBuf[start], szinput);
    chBuf[start + requestsize - 1] = '\0'; 

    return result;
} 
#endif

int
string_add(char *szinput)
{
    struct tag_record record;
    char *pszstart  = NULL;
    int requestsize = 0;
    int result      = NO_SPACE;
    int index       = -1;  //index of (size, start) within g_del_table
    int start       = -1;
    int size        = -1;
       
    //we also store the '\0'
    requestsize = strlen(szinput) + 1;

#if defined(USE_MALLOC_V3) 
    //USE_MALLOC_V3, 不使用g_del_table,不对已删除空间再利用，直接malloc
    result = NOT_EXIST;    

#else
    //NO_USE_MALLOC, USE_MALLOC_V1, USE_MALLOC_V2  
    //均使用g_del_table, 对已删除空间再利用，所以优先查询g_del_table
    result = getsize(requestsize, &index);  

#endif  

    if (NOT_EXIST == result)
    {
    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        //NO_USE_MALLOC and USE_MALLOC_V1 store into g_chBuf
        result = store(g_chBuf, g_indexMAX_buf, szinput, requestsize);
        if (NO_SPACE == result)
        {
            //failed, no space
            return result;
        }
        
        //bkz g_indexMAX_buf already updated
        start = g_indexMAX_buf - requestsize;
        
        record.size  = requestsize;
        record.start = start;        
        updatetable(&g_add_table, g_add_table.nrecords, &record);        
        g_add_table.nrecords++;
    #else
        //USE_MALLOC_V2 USE_MALLOC_V3 store into the malloc space
        result = store_by_malloc(szinput, &pszstart);

        record.size  = requestsize;
        record.start = (int)pszstart;
        insertintoaddtable(&g_add_table, &record);
    #endif
    }//end for if(NOT_EXIST == result)
    else
    {
        size  = g_del_table.records[index].size;
        start = g_del_table.records[index].start;

    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)        
        //both the NO_USE_MALLOC and USE_MALLOC_V1 use the g_chBuf
        result = store(g_chBuf, start, szinput, requestsize);
    #endif
        
    #if defined(USE_MALLOC_V2)
        //directly copy into the heap stack indicated by start
        strcpy((char *)start, szinput);
        
        //will success always, bkz we didn't free the space
        result = SUCCESS;       
    #endif

    #if defined(USE_MALLOC_V3)
        //USE_MALLOC_V3 don't use g_del_table, no freed space to use.        
        //should never come to this point.
    #endif

        if (NO_SPACE == result)
        {
            return result;
        }

        if (requestsize == size)
        {
            record.size  = size;
            record.start = start;
            removefromtable(&g_del_table, index);
            insertintoaddtable(&g_add_table, &record);
        }
        else
        {
            record.size  = size - requestsize;
            record.start = start + requestsize;
            updatetable(&g_del_table, index, &record);

            record.size  = requestsize;
            record.start = start;
            insertintoaddtable(&g_add_table, &record);
        }
    }//end for NOT_EXIST != result

    //This is not a good idea using a var to represent more than one meaning.
    result = record.start;
    return result;
}

int
string_del(char *szinput)
{
    struct tag_record record;
    int index = 0;
    int size  = 0;
    int start = 0;
    int result;

    result = string_query(szinput, &index);
    if (NOT_EXIST == result)
    {
        return result;
    }
    else
    {
        size  = g_add_table.records[index].size;
        start = g_add_table.records[index].start;

    #if defined(USE_MALLOC_V3)
        //USE_MALLOC_V3不使用g_del_table
        free((void *)start);
    #else   
        record.size  = size;
        record.start = start;
        insertintodeltable(&g_del_table, &record);
    #endif

        removefromtable(&g_add_table, index); 
    }//end for result != NOT_EXIST

    return result;
}

int
string_modify(char *origin, char *newinput)
{
    struct tag_record record;
    int oldsize = 0;
    int newsize = 0;
    int index   = 0;
    int start   = 0;
    int result;

    result = string_query(origin, &index);

    if (NOT_EXIST == result)
    {
        //not exist
        return result;
    }
    else    
    {
        oldsize = strlen(origin) + 1;
        newsize = strlen(newinput) + 1;

        start = g_add_table.records[index].start;
       
        // this will generate unused space, we need to update g_add_table and g_del_table;
        if (newsize < oldsize)
        {
        #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            result = store(g_chBuf, start, newinput, newsize);
        #else
            //directly copy into the heap stack indicated by start
            strcpy((char *)start, newinput);
            result = SUCCESS;
        #endif    
            
            if (NO_SPACE == result)
            {
                return result;
            }

            record.size  = newsize;
            record.start = start;
            updatetable(&g_add_table, index, &record);

            record.size  = oldsize - newsize;
            record.start = start + newsize;
            insertintodeltable(&g_del_table, &record);
        }//end for if (newsize < oldsize)
        else if (newsize == oldsize)
        {
        #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            //just replace directly
            result = store(g_chBuf, start, newinput, newsize);
        #else   
            //directly copy into the heap stack indicated by start
            strcpy((char *)start, newinput);
            result = SUCCESS;
        #endif

            if (NO_SPACE == result)
            {
                return result;
            }
        }//end for if (newsize == oldsize)
        else
        {
            string_del(origin);
            result = string_add(newinput);
        }//end for if (newsize > oldsize)
    }//end for result != NOT_EXIST

    return result;
}

void
display(void)
{
    char szhitinfo[MAX_INPUT] = {0};
    int nrecords = 0;
    int size     = 0;
    int start    = 0;
    int i;

    nrecords = g_add_table.nrecords;    
    
    for (i = 0; i < nrecords; i++)
    {
        size  = g_add_table.records[i].size;
        start = g_add_table.records[i].start;
    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        sprintf(szhitinfo, "%s  start: %d, size: %d\r\n", &g_chBuf[start], start, size);       
    #else
        sprintf(szhitinfo, "%s  start: %p, size: %d\r\n", (char *)start, start, size);
    #endif
        showinfo(szhitinfo);
    }
}

/*
Function: 显示buf[index, index+1, .. index+len-1]间的字符
*/
void
showbuf(char *buf, int index, int len)
{
    char szhitinfo[8] = {0};
    int i;

    //ugly
    if (index < 0)
    {
        index = 0;
    }

    for (i = index; i < index + len; i++)
    {
        if ('\0' == buf[i])
        {
            showinfo("\\0 ");
        }
        else
        {
            sprintf(szhitinfo, "%c ", buf[i]);            
            showinfo(szhitinfo);
        }
    }
}

/*
 g_add_table, g_del_table 已经各自有序
 对于其中相邻的(size1, start1), (size2, start2)
 如果start2 == start1 + size1, 说明它们是相邻的。
 这在USE_MALLOC_V2中是对删除空间再利用的情况，所以不能对start2进行free.
*/
void
free_space(void)
{
    int nrecords_add = 0;
    int index_add = 0;
    int size_add = 0;
    int start_add = 0;
    
    int nrecords_del = 0;
    int index_del = 0;
    int size_del = 0;
    int start_del = 0;

    int start_next = -1;    //用于判断那些为malloc所得空间的一部分的空间，不能再释放。
    
    nrecords_add = g_add_table.nrecords;
    nrecords_del = g_del_table.nrecords; 
    
    for ( ; index_add < nrecords_add && index_del < nrecords_del; )
    {
        size_add  = g_add_table.records[index_add].size;
        start_add = g_add_table.records[index_add].start;
        if (start_add == start_next)
        {
            //该start_add所指示的空间，为malloc所得空间的一部分，已经在之前释放。
            index_add++;
            continue;
        }
        
        size_del  = g_del_table.records[index_del].size;
        start_del = g_del_table.records[index_del].start;
        if (start_del == start_next)
        {
            index_del++;
            continue;
        }
        
        //先释放较小地址空间，避免释放的错误。
        if (start_add < start_del)
        {
            free((void *)start_add);
            start_next = start_add + size_add;  //
            index_add++;      
        }
        else
        {
            free((void *)start_del);
            start_next = start_del + size_del;  //
            index_del++;
        }
    }
    
    //perhaps some add records remains
    for ( ; index_add < nrecords_add; index_add++ )
    {
        size_add  = g_add_table.records[index_add].size;
        start_add = g_add_table.records[index_add].start;
        if (start_add == start_next)
        {
            start_next = start_add + size_add;
            continue;
        }
        else
        {
            free((void *)start_add);
        }
    }
    
    //perhaps some del records remains
    for ( ; index_del < nrecords_del; index_del++)
    {
        size_del  = g_del_table.records[index_del].size;
        start_del = g_del_table.records[index_del].start;        
        if (start_del == start_next)
        {
            start_next = start_del + size_del;
            continue;
        }
        else
        {
            free((void *)start_del);
        }           
    }
}

void
space_stat_for_malloc(void)
{
    int nrecords_add = 0;
    int index_add = 0;
    int size_add = 0;
    int start_add = 0;

    int nrecords_del = 0;
    int index_del = 0;
    int size_del = 0;
    int start_del = 0;    

    nrecords_add = g_add_table.nrecords;
    nrecords_del = g_del_table.nrecords; 
    
    for ( ; index_add < nrecords_add && index_del < nrecords_del; )
    {
        size_add  = g_add_table.records[index_add].size;
        start_add = g_add_table.records[index_add].start;

        size_del  = g_del_table.records[index_del].size;
        start_del = g_del_table.records[index_del].start;

        if (start_add < start_del)
        {
            //the smaller add first
            space_stat((char *)start_add, size_add, index_add, index_del);
            index_add++;      
        }
        else
        {
            space_stat((char *)start_del, size_del, index_add, index_del);
            index_del++;
        }
    }

    //perhaps some add records remains
    for ( ; index_add < nrecords_add; index_add++ )
    {
        size_add  = g_add_table.records[index_add].size;
        start_add = g_add_table.records[index_add].start;

        space_stat((char *)start_add, size_add, index_add, index_del);
    }

    //perhaps some del records remains
    for ( ; index_del < nrecords_del; index_del++)
    {
        size_del  = g_del_table.records[index_del].size;
        start_del = g_del_table.records[index_del].start;

        space_stat((char *)start_del, size_del, index_add, index_del);     
    }
}

void
space_stat(char *buf, int size, int index_add, int index_del)
{
    //ugly, very
    char szhitinfo[32] = {0};
    int index_buf = 0;
    int col       = 0;  
    int max       = 0;
    int table     = 0;
    int start     = 0;
    int len       = 0;
    int remain    = 0;
    int i         = 0;
    char flag     = '\0'; //F or U

    table     = ADD;
    max       = -1;

    showinfo("\r\n");

    for ( ; index_buf < size; index_buf++)
    {
        //this index belong to which one? g_add_table, g_del_table ?
        if (index_buf >= max )
        {
            table = gettable(buf, index_buf, &max, index_add, index_del);  
            
            if (ADD == table)
            {
                //not a good habit to reuse the param
                index_add++;
            }
            else
            {
                index_del++;
            }
        }//end for if (index_buf >= max)
        else
        {
            //flag keeps the same until max
            //so no need to gettable again.
        }        

        if (ADD == table)       //used
        {
            flag = 'U';
        }
        else if (DEL == table)  //free
        {
            flag = 'F';
        }
        else
        {
            //sth wrong?
        }

        //输出开头的偏移
        if (0 == col % (SPLIT))
        {
        #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            sprintf(szhitinfo, "%d: ", col);
        #else
            sprintf(szhitinfo, "%p: ", col + (int)buf); //其实可以全部采用这个
        #endif
            showinfo(szhitinfo);
        }
       
        //输出各空间的状态标志
        sprintf(szhitinfo, "%c ", flag);
        showinfo(szhitinfo);
  
        //输出各空间相应的字符
        if ((col != 0)
            &&(0 == (col + 1) % (SPLIT)) 
            )
        {
            showinfo("    ");

            start = col + 1 - (SPLIT);
            len   = SPLIT;
            showbuf(buf, start, len);

            showinfo("\r\n");
        }
        col += 1;
    } //end for ( ; index_buf < size; index_buf++)
    
    //对最后不足一个SPLIT的处理
    remain = (SPLIT) - col % (SPLIT);
    for (i = 0; i < remain; i++)
    {
        showinfo("  ");
    }
    showinfo("  ");
    
    len   = (SPLIT) - remain;
    start = col - len;
    
    showbuf(buf, start, len);    
}

/*
Function: used for debug, show detail info about g_add_table, g_del_table
*/
void
showtable(void)
{
    char szhitinfo[64] = {0};
    int nrecords = 0;
    int i;
    
    showinfo("\r\n[ Debug ]showing g_add_table, g_del_table  (size, start) ......\r\n");    
    showinfo("g_add_table:\r\n");

    nrecords = g_add_table.nrecords;
    for (i = 0; i < nrecords; i++)
    {
    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        //the start is relative to the same base
        sprintf(szhitinfo, "(%d %d) ", g_add_table.records[i].size, 
                                       g_add_table.records[i].start);
    #else
        //the start is absolute address by malloc or part of it.s
        sprintf(szhitinfo, "(%d %p) ", g_add_table.records[i].size, 
                                       g_add_table.records[i].start);        
    #endif
        showinfo(szhitinfo);
    }
    showinfo("\r\n\r\n");
    
    showinfo("g_del_table:\r\n");
    for (i = 0; i < g_del_table.nrecords; i++)
    {
    #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
        sprintf(szhitinfo, "(%d %d) ", g_del_table.records[i].size, 
                                       g_del_table.records[i].start);
    #else
        sprintf(szhitinfo, "(%d %p) ", g_del_table.records[i].size, 
                                       g_del_table.records[i].start);
    #endif
        showinfo(szhitinfo);
    }
    showinfo("\r\n\r\n"); 
}

#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
void
defrag(void)
{
    int size_del, start_del;
    int size_add, start_add;    
    int nrecords_add = 0;
    int i, j, k;
     
    nrecords_add = g_add_table.nrecords;
    i            = g_del_table.nrecords; 
    
    //walk through del_table
    for ( ;i >= 1; i--)
    {
        size_del  = g_del_table.records[i - 1].size;
        start_del = g_del_table.records[i - 1].start;        

        //find the first proper index (size, start) in add_table which just follows the del record
        for (j = nrecords_add ; j >= 1; j--)
        {
            size_add  = g_add_table.records[j - 1].size;
            start_add = g_add_table.records[j - 1].start;
            
            if (start_add == start_del + size_del)
            {
                //we've already gotten the index
                break;
            }
        }

        //remove (size_del, start_del) from del_table, update g_indexMAX_del
        g_del_table.records[i - 1].size  = 0; //size
        g_del_table.records[i - 1].start = 0; //start
        g_del_table.nrecords--;

        //update (size, start) in add_table to (size, start - size_del)
        for (k = nrecords_add; (j >= 1) && (k >= j); k--)
        {
            g_add_table.records[k - 1].start = g_add_table.records[k - 1].start - size_del;            
        }

        //move char in g_chBuf[start_add,.. ,g_indexMAX_buf] forward
        // step is size_del
        for (k = start_add; (j >= 1) && (k < g_indexMAX_buf); k++)
        {
            g_chBuf[k - size_del] = g_chBuf[k];
        }

        //update g_indexMAX_buf
        g_indexMAX_buf -= size_del;                
    }
}
#endif

void
char_stat(void)
{
    double percent = 0.0;
    char ch;
    int  num;
    int i;
    char szhitinfo[64] = {0};

    showinfo("Show char info......\r\n");

    sprintf(szhitinfo, "Total: %d\r\n", g_totalchar);
    showinfo(szhitinfo);

    for (i = 0; i < (MAX_CHAR - MIN_CHAR + 1) ; i++ )
    {
        if (0 != g_char_table[i])       
        {
            //0x21 is the first printable(exclude space) within ascii
            ch  = i + 0x21;
            num = g_char_table[i];
            percent = num * 1.0 / g_totalchar;

            sprintf(szhitinfo, "%c %d %f\r\n", ch, num, percent);
            showinfo(szhitinfo);
        }
    }
}

static void
updatechar(char *szText, int action)
{
    int index;
    int contribute;
    size_t i;
    size_t length;

    length = strlen(szText);

    if (ADD == action)
    {
        contribute = 1;
    }
    else if (DEL == action)
    {
        contribute = -1;
    }
    else
    {
        //sth wrong??
    }

    for (i = 0; i < length; i++ )
    {
        index = szText[i] - MIN_CHAR;
        g_char_table[index] += contribute;
        g_totalchar += contribute;
    }
}

void
process_add(void)
{
    char szhitinfo[MAX_INPUT] = {0};    //输出的提示性信息
    char szinput[MAX_INPUT]   = {0};
    int result                = -1;     //ugly

    //Add
    getinput(szinput, "Add what ? ");
    
    result = string_add(szinput);
    if (NO_SPACE == result)
    {
        showinfo("Add failed, for No More Space\r\n");
    }
    else
    {
        //not a good habit to reuse result for different meanning.
        sprintf(szhitinfo, "ADD: store into %p\r\n", result);
        showinfo(szhitinfo);
        
        //for char_stat()
        updatechar(szinput, ADD);
    }
}

void
process_query(void)
{
    char szhitinfo[MAX_INPUT] = {0};    //输出的提示性信息
    char szinput[MAX_INPUT]   = {0};
    char szqueryed[MAX_INPUT] = {0};    //模糊查询匹配的字符串
    int indexary[MAX_INPUT]   = {0};    //用于接收模糊查询的结果
    int num                   = 0;      //模糊查收所得结果的数目
    int index                 = 0;    
    int result                = 0;
    int i                     = 0;

    //string_query
    getinput(szinput, "Query what ? ");
    
#if defined(ACCURATE_QUERY)
    result = string_query(szinput, &index);
#else
    result = string_query_about(szinput, indexary, &num);
#endif    
    
    if (NOT_EXIST == result)
    {
        showinfo("Query: not exist\r\n");
    }
    else
    {
#if defined(ACCURATE_QUERY)
        sprintf(szhitinfo, "Query: at %p\r\n", g_add_table.records[index].start);
        showinfo(szhitinfo);
#else
        //模糊查询结果输出
        for (i = 0; i < num; i++)
        {
            //查询到的字符串的(size, start)记录在g_add_table中的下标
            index = indexary[i];  
        #if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            strcpy(szqueryed, &g_chBuf[g_add_table.records[index].start]);
        #else
            strcpy(szqueryed, (char *)(g_add_table.records[index].start));
        #endif  
            sprintf(szhitinfo, "Query: %s at %p\r\n", 
                szqueryed,                             //查询到的字符串
                g_add_table.records[index].start       //查询到的字符串在g_chBuf中的下标
                );    
            
            showinfo(szhitinfo);
        }
#endif
    }
}

void
process_del(void)
{
    char szhitinfo[MAX_INPUT] = {0};    //输出的提示性信息
    char szinput[MAX_INPUT]   = {0};    
    int result                = 0;

    getinput(szinput, "Del what ? ");
    
    result = string_del(szinput);    
    if (NOT_EXIST == result)
    {
        sprintf(szhitinfo, "%s does not exist\r\n", szinput);
        showinfo(szhitinfo);
    }
    else
    {
        showinfo("Del ok\r\n");
        
        //
        updatechar(szinput, DEL);
    }
}

void
process_modify(void)
{
    char szhitinfo[MAX_INPUT] = {0};    //输出的提示性信息
    char szinput[MAX_INPUT]   = {0};
    char newinput[MAX_INPUT]  = {0};
    int result                = 0;
    
    //string_modify
    getinput(szinput, "Modify, origin ? ");    
    getinput(newinput, "newinput ? ");
    
    result = string_modify(szinput, newinput);
    if (NOT_EXIST == result)
    {
        sprintf(szhitinfo, "Modify: %s not exist\r\n", szinput);
        showinfo(szhitinfo);
    }
    else if (NO_SPACE == result)
    {
        showinfo("Modify: no more space to store newinput\r\n");
    }
    else
    {
        showinfo("Modify ok\r\n");
        
        //
        updatechar(szinput, DEL);
        updatechar(newinput, ADD);
    }
}

void
process_fit(void)
{
    int nselect = -1;

    //set fit
    showinfo("Please choose the fit way\r\n");    
    showfit();    
    getselect(&nselect);
    
    if (nselect < FIT_EQUAL || nselect > FIT_EQUAL_FIRST )  //change accordingly
    {
        showinfo("Please make the right decision ^_^\r\n");
    }
    else
    {
        setfit(nselect);
    }
}

void
dispatch(int nselect)
{
    switch (nselect)
    {
    default:
        {
            //should not come to this point. whenever here, something wrong in the main.
            showinfo("Please make the right decision ^_^\r\n");
            break;
        }

    case 1:
        {
            process_add();
            break;
        }
  
    case 2:
        {
            process_query();
            break;
        }

    case 3:
        {
            process_del();
            break;
        }

    case 4:
        {
            process_modify();
            break;
        }

    case 5:
        {
            //display:
            showinfo("Displaying......\r\n");
            
            display();            
            break;
        }

    case 6:
        {
            //statistics
            showinfo("Statisticsing......U for Used, F for Free\r\n\r\n");

#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            space_stat(g_chBuf, g_indexMAX_buf, 0, 0);
#else
            space_stat_for_malloc();
#endif
            break;
        }

    case 7:
        {
            process_fit();
            break;
        }

    case 8:
        {
#if defined(NO_USE_MALLOC) || defined(USE_MALLOC_V1)
            defrag();
#else
            showinfo("defrag is not supported in this USE_MALLOC_V2, USE_MALLOC_V3\r\n");
#endif
            break;
        }

    case 9:
        {
            char_stat();
            break;
        }
    }//end for switch
}

int
main(void)
{
    int nselect = -1;

#if defined(USE_MALLOC_V1) 
    g_chBuf = (char *)malloc(MAX_SPACE);
    if (NULL == g_chBuf)
    {
        showinfo("No enough memory to continue\r\n");
        return -1;
    }
#endif

    while (1)
    {
        showmenu();

        getselect(&nselect);
        
        if (0 == nselect)
        {
            break;
        }
        else if(nselect < 0 || nselect >= 10) //change this according to the menu
        {
            showinfo("Please make the right decision ^_^\r\n");
            continue;
        }

        dispatch(nselect);

        //just for debug, can be removed
        showtable();
    }

#if defined(USE_MALLOC_V1) 
    if (g_chBuf)
    {
        free(g_chBuf);
        g_chBuf = NULL;
    }
#endif

#if defined(USE_MALLOC_V2) || defined(USE_MALLOC_V3)
    free_space();
#endif
    return 0;
}