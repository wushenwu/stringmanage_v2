#include <stdio.h>
#include "memmanage.h"
#include "fit.h"

//the way you can get suitable size, can only be dealed within this file
static int g_fitWAY = FIT_EQUAL;

int 
fitequal(int requestsize, int *index)
{
    int result = NOT_EXIST;
    int i      = 0;

    for (i = 0; i < g_del_table.nrecords; i++)
    {
        if (requestsize == g_del_table.records[i].size)
        {
            *index = i;
            result = SUCCESS;

            break;
        }
    }

    return result;
}

int 
fitenough(int requestsize, int *index)
{
    int result = NOT_EXIST;
    int min    = -1;
    int i      = 0;

    for (i = 0, min = -1; i < g_del_table.nrecords; i++)
    {
        if (requestsize <= g_del_table.records[i].size)
        {
            if ((-1 == min) 
                || (g_del_table.records[i].size < g_del_table.records[min].size)
                )
            {
                min = i;
            }
        }
    }

    if (-1 != min)
    {
        *index = min;
        result = SUCCESS;
    }

    return result;
}

int 
fitfirst(int requestsize, int *index)
{
    int result = NOT_EXIST;
    int i      = 0;

    for (i = 0; i < g_del_table.nrecords; i++)
    {
        if (requestsize <= g_del_table.records[i].size)
        {
            *index = i;
            result = SUCCESS;

            break;
        }
    }

    return result;
}

int 
getsize(int requestsize, int *index)
{
    int result = NOT_EXIST;

    switch (g_fitWAY)
    {
    case FIT_EQUAL:
        {
            result = fitequal(requestsize, index);
            break;
        }       

    case FIT_ENOUGH:
        {
            result = fitenough(requestsize, index);
            break;
        }        

    case FIT_FIRST:
        {
            result = fitfirst(requestsize, index);
            break;
        }        

    case FIT_EQUAL_ENOUGH:
        {
            result = fitequal(requestsize, index);
            if (NOT_EXIST == result)
            {
                result = fitenough(requestsize, index);
            }
            break;
        }        

    case FIT_EQUAL_FIRST:
        {
            result = fitequal(requestsize, index);
            if (NOT_EXIST == result)
            {
                result = fitfirst(requestsize, index);
            }
            break;
        }        

    default:
        {
            //sth wrong ?
            break;
        }        
    }

    return result;
}

void
setfit(int nselect)
{
   g_fitWAY = nselect;
}