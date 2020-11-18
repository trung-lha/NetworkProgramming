#include <stdio.h>
#include <stdlib.h>
#include "domain.h"

int main(int argc, char *argv[]){
	if(argc==1){
		printf("Please input IP or host name\n");
		return 0;
	}
	else{
		char *input = argv[1];
     if (ip_valid(input) == 1) {
			get_info_ip_address(input);
			return 0;
		}
		get_ip(input);
	}
    printf("\n");
	return 0;

}