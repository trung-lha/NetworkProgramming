#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>
#include <string.h>

int validateAnswer(char *ans){
	char *vali[8] = {"a","b"};
	if(strlen(ans) != 1){
		return 0;
	} else {
		for(int i=0;i<2;i++){
			if(strcmp(ans,vali[i]) == 0)
			return 1;
		}
		return 0;
	}
}
int main () {
   char *ans = (char*)malloc(sizeof(char));
   scanf("%s",ans);

   printf("%d\n",validateAnswer(ans));
   return 0;
}