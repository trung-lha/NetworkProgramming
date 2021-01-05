#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
   char *ans=(char*)malloc(sizeof(char));
//    getchar();
   scanf("%s",ans);
   printf("ban vua nhap: %s\n",ans);
   return(0);
}