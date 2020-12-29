#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "linkedList.h"

#define BACKLOG 5
#define BUFF_SIZE 1024
#define SUCCESS "Success"
#define FAILURE "Failure"
#define BLOCKING "Blocking"

void getDatatoList(node **list);
node *checkUsername(node *root, char *username);
void writeFile(FILE *fin, node *root);
void printNode(node *root);

int main(int argc, char const *argv[])
{
	// valid number of argument
	if (argc != 2)
	{
		printf("Usage: ./server PortNumber\n\n");
		return 0;
	}

  FILE *fp;
	fp = fopen("account.txt","r");
	if (fp == NULL){
		printf("No file input\n");
		return 0;
	}

  node *root = NULL;
  node *acc = NULL;

	int listen_sockfd, conn_sockfd;
	int recvBytes, sendBytes;

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;

	socklen_t sin_size;

  char *data = malloc(BUFF_SIZE*sizeof(char));
  char *confirm = malloc(BUFF_SIZE*sizeof(char));

	
	// Step1: Construct a TCP socket to listen connection request
	if ((listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("\nError: ");
		return 0;
	}
	
	// Step2: Bind address to socket
	memset(&servaddr, '\0', sizeof servaddr);
	servaddr.sin_family = AF_INET;         
	servaddr.sin_port = htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listen_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){
		perror("\nError: ");
		return 0;
	}     
	
	// Step3:  Listen request from client
	if(listen(listen_sockfd, BACKLOG) == -1){
		perror("\nError: ");
		return 0;
	}
	
	printf("Server started!\n");

	// Communicate with client
	while(1) 
	{
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sockfd = accept(listen_sockfd,( struct sockaddr *)&cliaddr, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("Connecting Clinet ID: %s\n", inet_ntoa(cliaddr.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1)
		{
      getDatatoList(&root);
      // printNode(root);
			//receives message from client
      free(data);
      data = malloc(BUFF_SIZE*sizeof(char));
			recvBytes = recv(conn_sockfd, data, BUFF_SIZE-1, 0);
			if (recvBytes <= 0)
			{
				printf("\nConnection closed");
				break;
			}
      printf("Server going to validate %s\n", data);

      acc = NULL;
      if((acc = checkUsername(root, data)) != NULL){
        strcpy(confirm, SUCCESS);
				send(conn_sockfd, confirm, strlen(confirm), 0);
        free(data);
        data = malloc(BUFF_SIZE*sizeof(char));
        recv(conn_sockfd, data, sizeof(data), 0);
        printf("Check pass: %s\n", data );
        if(strcmp(acc->element.password, data) == 0){
          strcpy(confirm, SUCCESS);
					send(conn_sockfd, confirm, strlen(confirm), 0);
        }else{
          strcpy(confirm, FAILURE);
          send(conn_sockfd, confirm, strlen(confirm), 0);
          recv(conn_sockfd, data, sizeof(data), 0);
          printf("Check pass: %s\n", data );
          if(strcmp(acc->element.password, data) == 0){
            strcpy(confirm, SUCCESS);
            send(conn_sockfd, confirm, strlen(confirm), 0);
          }else{
            strcpy(confirm, FAILURE);
						send(conn_sockfd, confirm, strlen(confirm), 0);
            acc->element.status = 0;
            fp = fopen("account.txt", "w");
            writeFile(fp, root);
            fclose(fp);
          }
        }
      }else{
        strcpy(confirm, FAILURE);
        send(conn_sockfd, confirm, strlen(confirm), 0);
      }
			//end conversation
		} 
		close(conn_sockfd);	
	}
	close(listen_sockfd);
	return 0;
}

void getDatatoList(node **list){

  (*list) = freeList(*list);
  elementtype element;
  FILE *fin;
	fin = fopen("account.txt","r");
  while(fscanf(fin, "%s%s%d", element.user_name, element.password, &(element.status)) != EOF){
    insertAtEnd(list, element);
  }
	fclose(fin);
}

node *checkUsername(node *root, char *username){
  node *temp = root;
  while(temp != NULL){
    // printf("***%s", e.user_name);
    // printf("*%s\n", temp->element.user_name);
    if(strcmp(temp->element.user_name, username) == 0)
      return temp;
    temp = temp->next;
  }
  return NULL;
}

void writeFile(FILE *fin, node *root){
  node *temp = root;
  while(temp != NULL){
    fprintf(fin, "%s %s %d\n", temp->element.user_name, temp->element.password, temp->element.status);
    temp = temp->next;
  }
}

void printNode(node *root){
  node *temp = root;
  while(temp != NULL){
    printf("%s-%s-%d\n", temp->element.user_name, temp->element.password, temp->element.status);
    temp = temp->next;
  }
}