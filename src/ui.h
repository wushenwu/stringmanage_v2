#ifndef UI_H_BF2C7655_C46A_4bdf_96DA_C45A738352FA

#define UI_H_BF2C7655_C46A_4bdf_96DA_C45A738352FA


/*
Function: menu options for user to select.
*/
void
showmenu(void);

/*
Function: show fit ways you can choose 
*/
void
showfit(void);

/*
Function: deal with user selection for menu
*/
void
getselect(int *select);


/*
Function: deal with user input,
          like string to add, del, query, or modify
*/
void
getinput(char *chinput, char *hitinfo);

/*
Function: show info for users
          encapsulation of printf
*/
void
showinfo(char *szhitinfo);

#endif