/**
 **	mymain.c
 **		: main function source
 **/

#include <stdio.h>

void lib_func_1(char *);
void lib_func_2(char *);

int main()
{
	printf("FUNFUN EXAMPLE \n");
	printf("---------------\n");

	lib_func_1("LIB 1 called");
	lib_func_2("LIB 2 called");

	return 0;
}
