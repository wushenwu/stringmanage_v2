#include <stdio.h>
#include "ui.h"


void
showmenu(void)
{
    showinfo("This is the menu for memory management:\r\n"
        "---------------------------------------\r\n"
        "1. Add      : store your input         \r\n"
        "2. Query    : whether your input exists\r\n"
        "3. Del      : remove your input        \r\n"
        "4. Modify   : change origin to new     \r\n"
        "5. Display  : show all stored inputs   \r\n"
        "6. Space Statistics: show how space are used \r\n"
        "7. Choose Fit:  default is fitequal    \r\n"
        "8. Defrag    :  reorganize free space  \r\n"
        "9. Char  Statistics: show char info  \r\n"
        "0. exit                                \r\n"
        "---------------------------------------\r\n"
        "Please enter one number to select:     \r\n");
}

void
showfit(void)
{
    showinfo("This is the fit way you can choose:\r\n"
            "---------------------------------------\r\n"
            "0. FIT_EQUAL       : find deled space whose size just equal to your input\r\n"
            "1. FIT_FIRST       : find the first \r\n"
            "2. FIT_ENOUGH      : find the smallest that >= \r\n"
            "3. FIT_EQUAL_ENOUGH: if no equal than enough \r\n"
            "4. FIT_EQUAL_FIRST : if no equal than the first\r\n"
            );
}

void
getselect(int *select)
{
    //take care of the input
    scanf("%d", select);
    fflush(stdin);
}

void
getinput(char *chinput, char *szhitinfo)
{
    showinfo(szhitinfo);

    //255 is for the MAX_INPUT 256
    scanf("%255s", chinput);    
    fflush(stdin);
}

void
showinfo(char *szhitinfo)
{
    //can be changed according to your own situation
    printf("%s", szhitinfo);
}
