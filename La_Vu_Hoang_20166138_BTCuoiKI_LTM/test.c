#include <stdio.h> 
#include <string.h> 
  
int main() 
{ 
    // declare and initialize string 
    char* headQuestion[100] = {"Câu 1","Câu 2","Câu 3","Câu 4","Câu 5","Câu 6","Câu 7","Câu 8","Câu 9","Câu 10","Câu 11","Câu 12","Câu 13","Câu 14","Câu 15"};
    for(int i=0;i<15;i++)
        printf("Appended String: %s\n", headQuestion[i]); 
  
    return 0; 
} 