#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netdb.h>
#include <errno.h>
#define MAX 1024

// Check string from client has "."?
int check_input(char *buff);
// Check number between 0 and 256
int check_number(char *ip_part);
// Check ip address 
int ip_valid(char *ip);
char* ipToHostName(char *ipAddress);
// Input hostname, print out IP, alias IP
char* hostNameToIp(char * hostname) ;
char *seperate(char* buff);

int main(int argc, char const *argv[])
{
	int server_sock;
	socklen_t sin_size;
	char buff[MAX+1];
	int bytes_sent, bytes_received;
	struct sockaddr_in server;
	struct sockaddr_in client;

	if (argc != 2)
	{
		printf("Enter true form: ./server PortNumber\n\n");
		return 0;
	}

	// construct UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("\nError: ");
		exit(0);
	}

	// bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;
	memset(server.sin_zero, '\0', sizeof server.sin_zero);

	if (bind(server_sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("\nError: ");
		exit(0);
	}

	// communicate with client
	while(1)
	{
		// get string from client
		sin_size = sizeof(struct sockaddr_in);
		bytes_received = recvfrom(server_sock, buff, MAX-1, 0, (struct sockaddr*)&client, &sin_size);

		if (bytes_received < 0)
			perror("\nError: ");
		else
		{
			// handle string from client
			buff[bytes_received] = '\0';

			char *reply;
			if(check_input(buff) == 1){
				reply = seperate(buff);
			}else{
				if(ip_valid(buff) == 1)
					reply = ipToHostName(buff);
				else
					reply = hostNameToIp(buff);
			}

			// if string contain symbol return Error
			if (reply == NULL)
				reply = "Error";

			// return result to client
			bytes_sent = sendto(server_sock, reply, strlen(reply), 0, (struct sockaddr*)&client, sin_size);
			if (bytes_sent < 0)
				perror("\nError: ");
		}
	}

	close(server_sock);
	return 0;
}

int check_input(char *buff){
	for(int i = 0; i < strlen(buff); i++){
		if(buff[i] == '.')
			return 0;
	}
	return 1;
}

int check_input(char *buff);

// Check number between 0 and 256
int check_number(char *ip_part) {
    int number = atoi(ip_part);
    if(number < 0 || number > 256)
        return 0;
    return 1;
}

// Check ip address 
// return 1 for true ip address
// return 0 for not
int ip_valid(char *ip) {
    char str[40];
    strcpy(str, ip);

    int count = 0;
    // int number = 0;

    char *token = strtok(str, "."); 

    while (token != NULL) 
    { 
        if(!check_number(token))
            return 0;

        token = strtok(NULL, "."); 
        count++;
    }

    if(count != 4) 
        return 0;

    return 1;
}

// Input hostname, print out IP, alias IP

char* hostNameToIp(char * hostname) 
{   
	char *reply = malloc(sizeof(char)*200);
    struct hostent *he;
    struct in_addr **addr_list;     
    int i;     
    if ( (he = gethostbyname( hostname ) ) == NULL)     
    {
		reply = "Not found information\n";        
    }
    else{  
        // Print out all alias
        addr_list = (struct in_addr **) he->h_addr_list;
		sprintf(reply, "Official : %s \n", inet_ntoa(*addr_list[0]));
		strcat(reply, "Alias : ");
        if(addr_list[1] == NULL){
			strcat(reply, "NULL \n");
        }
        for(i = 1; addr_list[i] != NULL; i++) {
			strcat(reply, inet_ntoa(*addr_list[i]));
			strcat(reply, "\n");
        }
    }
	return reply;
}

// Input IP, print out hostname and alias


char* ipToHostName(char *ipAddress) {
    struct in_addr addr;
	char *reply = malloc(500*sizeof(char));
    inet_aton(ipAddress, &addr);
    struct hostent *host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if(host == NULL) {
      reply = "Not found information\n";
    }
    else{

			sprintf(reply, "Official name: %s\n",host->h_name);
			strcat(reply, "Alias: \n");
			if (host->h_aliases[0] == NULL){
				strcat(reply, "NULL\n");
			}
			else{
				// char *tmp;
				for(int i = 0; host->h_aliases[i] != NULL; i++) {
					strcat(reply, "\t");
					strcat(reply, host->h_aliases[i]);
					strcat(reply, "\n");
				}
			}
    }  
	return reply;
}

char *seperate(char* buff)
{
	// return NULL if input string is empty
	if (strlen(buff) == 0)
		return NULL;
	
	char numbers[MAX], letters[MAX], *result; 
	int i, i_number = 0, i_letter = 0;
	memset(numbers, '\0', MAX);
	memset(letters, '\0', MAX);
	result = malloc(MAX+1);

	// seperate numbers and letters
	for (i = 0; buff[i] != '\0'; i++)
	{
		if (isdigit(buff[i]))
			numbers[i_number++] = buff[i];
		else if (isalpha(buff[i]))
			letters[i_letter++] = buff[i];
		else
			return NULL;
	}
	sprintf(result, "%s\n%s", numbers, letters);
	return result;
}