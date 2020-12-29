#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>

#define BUFF_SIZE 1024

int STATUS = 0;
#define SUCCESS "Success"
#define FAILURE "Failure"
#define BLOCKING "Blocking"

int menu(void);
void saveAccount(char tmp_username[30], char tmp_password[30]);

int main (int argc, char const *argv[])
{
	// valid number of argument
	if (argc != 3)
	{
		printf("Usage: ./client IPAddress PortNumber\n\n");
		return 0;
	}

  // Variable of global
	int client_sockfd;    // Status of socket
	int recvBytes, sendBytes;   // Status of send and recv
	struct sockaddr_in serv_addr; // Server's address
  char *buff = malloc(BUFF_SIZE * sizeof(char));  // Send message to server
  char *result = malloc(BUFF_SIZE * sizeof(char));
  int select;   // select option in menu

	// Step1: Construct socket
	client_sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	// Step2: Specify server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	// Step3: Request to connect server
	if(connect(client_sockfd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	// Step4: Communicate with server			
	while(1){
    select = menu();
    switch(select){
      case 1:
        getchar();
        //send message
        if (STATUS == 1 ){
					printf("You have already log in\n");
				}
				if(STATUS == 2){
					printf("This window can not log in more.\n");
				}
        if(STATUS == 0){
          free(buff);
          buff = malloc(BUFF_SIZE * sizeof(char));
          printf("\nUsername: ");
          scanf("%s", buff);

          // printf("**%s\n", buff);

          // exit if user only input ENTER
          // if (buff[0] == '\n') break;

          // replace \n with \0
          buff[strcspn(buff, "\n")] = '\0';

          sendBytes = send(client_sockfd, buff, strlen(buff), 0);
          if(sendBytes <= 0){
            printf("\nConnection closed!\n");
            break;
          }
          // receive echo reply
          recvBytes = recv(client_sockfd, result, BUFF_SIZE-1, 0);
          if(recvBytes <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
          }
          // printf("***%s\n", result);
          if (strcmp(result,SUCCESS)==0) { 
            getchar();
						printf("Password: ");
						scanf("%s", buff);
						send(client_sockfd, buff, strlen(buff), 0);
						recv(client_sockfd, result, sizeof(result), 0);
						if (strcmp(result, SUCCESS)==0){
							printf("Log in successfully\n");
							STATUS = 1;
						}
						if (strcmp(result, FAILURE)==0)
						{
              getchar();
							printf("Password incorrect please try again\nPassword: ");
							scanf("%s", buff);
							send(client_sockfd, buff, strlen(buff), 0);
							recv(client_sockfd, result, sizeof(result), 0);
							if (strcmp(result, SUCCESS)==0){
								printf("Log in successfully\n");
                STATUS = 1;
							}
							if (strcmp(result, FAILURE)==0){
								while ((getchar()) != '\n');
								printf("Password incorrect please try again\nPassword: ");
								scanf("%s", buff);
								send(client_sockfd, buff, strlen(buff), 0);
								recv(client_sockfd, result, sizeof(result), 0);
								if (strcmp(result, SUCCESS)==0){
									printf("Log in successfully\n");
									// saveAccount(tmp_username,tmp_password);
                  STATUS = 1;
								}
								if (strcmp(result, FAILURE)==0){
									printf("Password in correct 3 times. Block account\n");
								}
							}
						}
					}else if(strcmp(result,FAILURE)==0){
						printf("Account is not existed\n");
					}else if (strcmp(result,BLOCKING)==0)	{
						printf("Account is blocked, can't log in\n");
					}
        }
        break;
      case 2:
        if (STATUS == 0){
					printf("Haven't sign in please log in first\n");
				}
				if (STATUS == 2){
					printf("Have already log out\n");
				}
				if (STATUS == 1){
					printf("Sign out username successfully\n");
					STATUS = 0;
				}
        break;
      default:
        // Close socket
        close(client_sockfd);
        return 0;
    }
	}
	return 0;
}

int menu(void){
  int select;
  puts("MENU");
  puts("-------------------------");
  puts("1. Login");
  puts("2. Logout");
  puts("Other. out");
  printf("Enter selection: ");
  scanf("%d", &select);
  return select;
}