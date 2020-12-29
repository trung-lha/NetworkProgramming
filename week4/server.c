#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BACKLOG 5
#define BUFF_SIZE 1024
#define STORAGE "./Storage/" //default save file place
#define EXISTEDFILE "Error: File da ton tai "

int check_valid_file(char* file_name);
// seperate a string to 2 line: numbers and letters
char *split(char* buff);

int main(int argc, char const *argv[])
{
	// valid number of argument
	if (argc != 2)
	{
		printf("Usage: ./server PortNumber\n\n");
		return 0;
	}

	int listen_sockfd, conn_sockfd;
	int recvBytes, sendBytes;

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	
	char data[BUFF_SIZE];
  FILE *fileptr;
  long filelen;
	
	socklen_t sin_size;
	
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

  struct stat st = {0};
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
			//receives message from client
			recvBytes = recv(conn_sockfd, data, BUFF_SIZE-1, 0); //blocking
			if (recvBytes <= 0)
			{
				printf("\nConnection closed");
				break;
			}
      if (stat(STORAGE, &st) == -1) { //create storage if it not exist
        mkdir(STORAGE, 0755);
      }
      if (check_valid_file(data) == 1){
        char number[BUFF_SIZE];
        char character[BUFF_SIZE];
        data[recvBytes] = '\0';
        // Path of file in Storage folder
        char name[100];
				strcpy(name, STORAGE); // save file in storage
				strcat(name, data); // file name
				printf("Ten file : %s\n", name);
				if((fileptr = fopen(name, "rb")) != NULL) { // check if file exist
					printf("%s\n", EXISTEDFILE);
					sendBytes = send(conn_sockfd, EXISTEDFILE, strlen(EXISTEDFILE), 0);
					if (sendBytes <= 0){
						printf("\nConnection closed");
						break;
					}
					fclose(fileptr);
					continue;
				}else{
          // fclose(fileptr);
					sendBytes = send(conn_sockfd, "OK", 20, 0); // if file Name valid
					if (sendBytes <= 0){
						printf("\nConnection closed");
						break;
					}
					recvBytes = recv(conn_sockfd, &filelen, 20, 0);
					if (recvBytes <= 0){
						printf("\nConnection closed");
						break;
					}
					printf("Uploaded file name: %s.\n\n", data);

					int sumByte = 0;
          char *ndFile;
					fileptr = fopen(name, "wb");
					ndFile = (char*) malloc(BUFF_SIZE * sizeof(char));
					while(1) {
						recvBytes = recv(conn_sockfd, ndFile, BUFF_SIZE, 0);
						if(recvBytes == 0) {
							printf("Error: Gui File that bai \n");
						}
						sumByte += recvBytes;
						fwrite(ndFile, recvBytes, 1, fileptr);
						free(ndFile);
						ndFile = (char*) malloc(BUFF_SIZE * sizeof(char));
						if(sumByte >= filelen) {
							break;
						}
					}  // file content
					if (recvBytes <= 0){
						printf("\nConnection closed");
						break;
					}
					
					sendBytes = send(conn_sockfd, "Upload success \n", 30, 0);
					if (sendBytes <= 0){
						printf("\nConnection closed");
						break;
					}

					fclose(fileptr);
					free(ndFile);
        }
      }else{
        // handle received data
        data[recvBytes] = '\0';
        printf("%s\n", data);
        char *reply = split(data);
      
        // if string contain symbol return Error
        if (reply == NULL)
          reply = "Error";

        //echo to client
        sendBytes = send(conn_sockfd, reply, strlen(reply), 0);
        if (sendBytes <= 0)
        {
          printf("\nConnection closed");
          break;
        }
      }
			//end conversation
		} 
		close(conn_sockfd);	
	}
	close(listen_sockfd);
	return 0;
}

int check_valid_file(char* file_name){
  int count = 0;
  for(int i = 0; i < strlen(file_name); i++){
    if(file_name[i] == '.')
      count++;
  }
  if(count == 1)
    return 1;
	return 0;
}

char *split(char* buff)
{
	// return NULL if input string is empty
	if (strlen(buff) == 0)
		return NULL;
	
	char numbers[BUFF_SIZE], letters[BUFF_SIZE], *result; 
	int i, count_number = 0, count_letter = 0;
	memset(numbers, '\0', BUFF_SIZE);
	memset(letters, '\0', BUFF_SIZE);
	result = malloc(BUFF_SIZE+1);

	// seperate numbers and letters
	for (i = 0; buff[i] != '\0'; i++)
	{
		if (isdigit(buff[i]))
			numbers[count_number++] = buff[i];
		else if (isalpha(buff[i]))
			letters[count_letter++] = buff[i];
		else
			return NULL;
	}
	sprintf(result, "%s\n%s", numbers, letters);
	return result;
}