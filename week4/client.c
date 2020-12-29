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
#define EXISTEDFILE "Error: File da ton tai "

int menu(void);
void checkName(char *fileName);
void toNameOfFile(char *fileName, char* name );
char** str_split(char* a_str, const char a_delim);


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
	char buff[BUFF_SIZE];   // Send message to server
  int select;   // select option in menu

  // Variable of case 2
  FILE *fileptr;

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
        printf("\nInsert string to send (ENTER to quit): ");
        fgets(buff, BUFF_SIZE, stdin);

        // exit if user only input ENTER
        if (buff[0] == '\n') break;

        // replace \n with \0
        buff[strcspn(buff, "\n")] = '\0';

        sendBytes = send(client_sockfd, buff, strlen(buff), 0);
        if(sendBytes <= 0){
          printf("\nConnection closed!\n");
          break;
        }
        
        // receive echo reply
        recvBytes = recv(client_sockfd, buff, BUFF_SIZE-1, 0);
        if(recvBytes <= 0){
          printf("\nError!Cannot receive data from sever!\n");
          break;
        }
        
        // print reply
        buff[recvBytes] = '\0';
        puts("Reply from server:");
        puts(buff);
        break;
      case 2:
        getchar();
				//send message
        printf("Nhap ten file: ");
        bzero(buff,BUFF_SIZE);
        fgets(buff, BUFF_SIZE, stdin);    // Enter file path
        if(buff == NULL || strcmp(buff, "\n") == 0){
            printf("%s\n", "Good bye !");
            return 0;
        }
        // Split path file --> file name send to server
        char fileName[50];
        // Convert filename\n --> filename
        checkName(buff); //check if null
        printf("fileName: %s\n", buff); //print file name
        if ((fileptr = fopen(buff, "rb")) == NULL){
            printf("Error: File not found\n");
            fclose(fileptr);
            continue;
        }
        // Split buff --> filename
        toNameOfFile(buff, fileName);

        // get length of file
        long filelen;
        fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
        filelen = ftell(fileptr);             // Get the current byte offset in the file       
        rewind(fileptr);    // pointer to start of file
        
        // send filename to server
        sendBytes = send(client_sockfd, fileName, strlen(fileName), 0);
        if(sendBytes <= 0){ // check if false
          printf("\nConnection closed!\n");
          break;
        }

        // Recv status of filename
        recvBytes = recv(client_sockfd, buff, BUFF_SIZE, 0); // recv result of check file  name
        if(recvBytes <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        
        buff[recvBytes] = '\0';
        if(strcmp(buff, EXISTEDFILE) == 0) { // if file is existed
            printf("%s\n", buff);
            continue;
        }

        sendBytes = send(client_sockfd, &filelen, 20, 0); //send file size
        if(sendBytes <= 0){
            printf("\nConnection closed!\n");
            break;
        }

        int sum = 0; //count size byte send
        printf("\n Bat dau doc file !\n");
        while(1) {
            int byteNum = BUFF_SIZE;
            if((sum + BUFF_SIZE) > filelen) {// if over file size
                byteNum = filelen - sum; 
            }
            char* buffer = (char *) malloc((byteNum) * sizeof(char));
            fread(buffer, byteNum, 1, fileptr); // read buffer with size 
            sum += byteNum; //increase byte send
            sendBytes = send(client_sockfd, buffer, byteNum, 0);
            if(sendBytes <= 0){
                printf("\nConnection closed!\n");
                break;
            }
            free(buffer);
            if(sum >= filelen) {
              break;
            }
        }
        printf("\n Ket thuc doc file !\n");
        fclose(fileptr); //close file

        recvBytes = recv(client_sockfd, buff, BUFF_SIZE, 0); // recv result
        if(recvBytes <= 0){
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        printf("%s \n", buff); //print result
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
  puts("1. Split string");
  puts("2. Transfer file");
  puts("Other. out");
  printf("Enter selection: ");
  scanf("%d", &select);
  return select;
}

void checkName(char *fileName) {
    int i;
    for(i = 0; fileName[i] != '\0'; i++) {
        if(fileName[i] == '\n') {
            fileName[i] = '\0';
        }
    }
}
void toNameOfFile(char *fileName, char* name ) {
    char** tokens = str_split(fileName, '/');
    int i;
    for (i = 0; *(tokens + i); i++)
    {
        // count number elements in array
    }
    strcpy(name, *(tokens + i -1));
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}