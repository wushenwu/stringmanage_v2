#ifndef MEMMANAGE_H_CF771D01_FB96_4a42_BA47_C7B9C76D8B18    //防止头文件被重复引用

#define MEMMANAGE_H_CF771D01_FB96_4a42_BA47_C7B9C76D8B18

//
#define 	MAX_SPACE	            10000
#define 	MAX_INPUT		        256

//which table: g_add_table, or g_del_table
#define		ADD		                1
#define		DEL			            -1

//error process
#define     NOT_EXIST               -1
#define     NO_SPACE                -2
#define     SUCCESS                 0                


//for memory space statistics
#define     SPLIT                   8

//for char statistics(printable but not space)
#define     MAX_CHAR                0x7E
#define     MIN_CHAR                0x21

//used to choose between ARRY and POINTER VERSION
#define     ARY_VERSION


/*
  改用malloc和free进行处理， 可选的变更有如下几种：
 1)USE_MALLOC_V1 
   将原来的大数组g_chBuf[10000]用malloc(10000) 一次性获取, 后续的字符串的添加、删除，仍然采用现有方案。 在进程结束前，将该大块空间free掉。
  
    自评：
    该方案使得变更最小，功能没有丧失。(分配表add_table, del_table等仍然需要，可用）
    缺点——基本没有使用malloc, free,失去变更的意义
    
      
 2)USE_MALLOC_V2
   抛弃原来的大数组g_chBuf,  继续保留add_table, del_table, 对删除的空间进行再利用，每次添加时，视del_table的情况，可能需要malloc或者使用原删除空间。 每次释放均不进行free. 在进程退出前，对add_table，del_table中的进行free.
        
   自评：
   这样，add_table, del_table中的记录(size, start)就真的变成了(size, start_address), 某些功能丧失，比如碎片整理。
   这样对删除的空间不是直接free而是放入到del_table中考虑再利用，减小了malloc.
   但是这可能导致碎片化比较严重（跟fit的方式有关）， 也无法进行碎片整理。
          
            
 3)USE_MALLOC_V3
    抛弃原来的大数组g_chBuf, 仅保留add_table, 不对删除的空间进行再利用，每次添加均malloc, 每次释放均free.
              
    自评：
    这样，全权的交给系统进行malloc, free,仅利用add_table记录各字符串的(size, start_address)， 同2）一样，有功能丧失。 也不再对删除的空间再利用。
    避免了考虑碎片等问题。

*/
//#define NO_USE_MALLOC

//#define USE_MALLOC_V1   

//#define USE_MALLOC_V2

#define USE_MALLOC_V3

struct tag_record
{
    //a pair of (size, start) makes one record
    int size;       //size of the string
    int start;      //the start location within the buf.
};

struct tag_table
{
    int nrecords;                           //number of records
    struct tag_record records[MAX_SPACE];   //ary of records
};


/*不提倡使用全局变量， 尽量不要在头文件中使用extern int value 这类声明*/

/*used for maintain*/
extern struct tag_table g_add_table;
extern struct tag_table g_del_table;

/*
Function: 精确查询输入的字符串是否存在于g_chBuf中。 
　　
Param	: chInput 临时存放用户输入的字符串 
           index 用于接收该字符串对应的(size, start)在g_add_table中的下标
           (结果唯一)
           
Return  :  NOT_EXIST不存在， SUCCESS存在，并设置index.
*/
int 
string_query(char *chInput, int *index);

/*
Function: 模糊查询输入的字符串是否存在于g_chBuf中

Param   : chInput 临时存放用户输入的字符串
          indexary用于接收查询到的字符串对应的(size, start)在g_add_table中的下标
          (可能有多个，所以传参时，应该为数组名).

           num 用于接收查询到的总个数

Return  : NOT_EXIST 不存在， SUCCESS存在，并设置indexary.
          
*/
int
string_query_about(char *chInput, int *indexary, int *num);


/*
Function: 将用户输入chInput[]存储至g_chBuf[]中。
　　  
Param	: chInput[]为用户输入的临时存储
　　        
Return  :  NO_SPACE 添加失败， 其他表示添加的位置.

Effect  : 更新g_add_table, g_indexMAX_add, 根据情况更新
          g_del_table,g_indexMAX_buf


  string_add(string):
  　　sizereal = len(string)
  　　如果没有从g_del_table中查询到合适（ 在后面阐述）的(size, start)记录，则根据charBuf[10000]数组中已分配的最大下标indexMax, 从indexMax处填入string, 并在g_add_table中追加(size, indexMax)记录。
  　　
  　　若从g_del_table中查询到一个合适的（在后面阐述) size的记录(size, start)，将string拷贝至charBuf[start]开始的位置。根据sizereal和size的关系，有以下几种情况：
  　　sizereal = size, 则将(size, start)从g_del_table中删除，同时将(size, start)有序的插入到g_add_table中
      sizereal < size, 则将g_del_table中的该(size, start)改为(size - sizereal, start + sizereal), 同时，将(sizereal, start)有序的插入到g_add_table中。

*/
int
string_add(char *chInput);

/*
Function: 试图从g_chBuf[]中删除用户输入chInput[]。
　　  
Param	: chInput[]为用户输入的临时存储
　　        
Return  :  NOT_EXIST for failed， SUCCESS 成功.

Effect  : 更新g_add_table, 
           更新g_del_table， 根据情况对空闲空间进行整合，影响g_del_table

string_del(string):
　　index = string_query(string)
　　(size, start) = (g_add_table[index], g_add_table[index+1])
　　
　　如果index == -1 查询失败，则返回-1表示删除失败。
　　
　　如果index != -1, 则将(size, start)从g_add_table中删除，同时考虑更新有序表g_del_table. 考虑到可能的连续空闲空间的整合，分为以下情况：
　　如果(size, start)有合适的前驱(size1, start1) 满足 start1 + size1 = size, 表明该两块空闲空间连续，则应将(size1, start1)更新为(size1+size, start1)
　　如果(size,start)有合适的后继(size2, start2)满足 start + size = start2, 则(size2, start2)应更新为(size+size2, start)
*/
int
string_del(char *chInput);

/*
Function: 试图从g_chBuf[]中将原有输入origin更改为新输入内容new。
　　  
Param	: origin指向待修改内容
           new指向新内容
　　        
Return  :  NOT_EXIST for not exist, NO_SPACE for no space, SUCCESS for success

Effect  : 更新g_add_table, 
           更新g_del_table， 根据情况对空闲空间进行整合，影响g_del_table

string_modify(origin, new):
　　index = string_query(origin)
　　如果index == -1 表明不存在origin, 修改失败。
　　oldsize = len(origin)
　　newsize = len(new)
　　如果index != -1, 它表示origin的(size, start)在g_add_table中的下标，取得start.
　　根据oldsize和newsize的大小，分为以下情况：
　　如果 newsize < oldsize, 则更改g_add_table中的(size, start) 为(newsize, start), 同时在g_del_table中有序的插入(oldsize - newsize, start + newsize)
　　如果 newsize = oldsize, 则不发生变更
　　如果newsize > oldsize, 则此时可以简单调用的string_del(origin)和string_add(string), 完成空间的释放和再整合，以及再分配。
（当然，也可能出现该start前驱或者后继本来已经为空闲的情况，那么此时进行空闲的整合是最好的，但是我们还是将其纳入一般的流程进行处理）。
*/
int
string_modify(char *origin, char *newinput);


/*
Function: 将chInput[]存储至chBuf[start]
　　  
Param	 : chBuf, start, chInput, requestsize          
　　        
Return  :  NO_SPACE for failed, SUCESS for success

Effect   : 更新chBuf[],根据需要更新g_indexMAX_buf
*/
int
store(char *chBuf, int start, char *chInput, int requestsize);

/*
Function: for USE_MALLOC_V2, USE_MALLOC_V3, store szinput into space malloced.

Param   : ppszstart 用于接收malloc的地址。
*/
int
store_by_malloc(char *szinput, char **ppszstart);

/*
Function: 遍历g_add_table, 结合g_chBuf, 输出所有存储的字符串输入。
　　  
Param	 : None         
　　        
Return  : None.

Effect  : None
*/
void
display(void);


/*
Function: 统计g_chBuf[] 0 ～g_indexMAX_buf 各字节的使用情况,

更改    ：由于NO_USE_MALLOC, USE_MALLOC_V1, USE_MALLOC_V2, USE_MALLOC_V3多种情况的考虑
          将此更改为统计char *buf ~ char *(buf+size) 中各空间的使用情况
　　  
Param	 : index_add, index_del 为目前所遍历到的g_add_table, g_del_table中的下标。
            NO_USE_MALLOC 和USE_MALLOC_V1中恒为(0,0)
            USE_MALLOC_V2 和USE_MALLOC_V3中由space_stat_for_malloc设置
　　        
Return  : None
Effect   : None
*/
void
space_stat(char *buf, int size, int index_add, int index_del);

/*
Function: 对USE_MALLOC_V2, USE_MALLOC_V3中空间使用情况的统计
          会调用space_stat;
*/
void
space_stat_for_malloc();

/*
Function: 进行碎片整理
*/
void
defrag(void);

/*
Function: 统计各字符出现的次数及频率
*/
void
char_stat(void);

/*
Function: 针对szText[]及action, 更新g_char_table中各字符出现的次数

        called whenever string_add, string_del, string_modify success.
*/
static void
updatechar(char *szText, int action);

/*
Function: 对USE_MALLOC_V2，USE_MALLOC_V3中的g_add_table, g_del_table所维护的空间进行释放。
*/
void
free_space(void);
#endif