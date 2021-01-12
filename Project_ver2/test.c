#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
int main () { 
   time_t start_t, end_t; 
   double diff_t; 
   char *ans;
   printf("Bắt đầu chương trình...\n"); 
   time(&start_t);
   for(int i=0;i<5;i++){
      ans = (char*)malloc(sizeof(char));
      scanf("%s",ans);
   }

   time(&end_t);
   diff_t = (difftime(end_t, start_t)); 
   int phut = diff_t/60;
   printf("Thời gian thực thi là %d phút %.0f giây\n",phut, (diff_t - phut*60)); 
   printf("Thoát chương trình...\n"); 
   return(0); 
}