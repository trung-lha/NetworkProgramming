#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX 1024
void Up(char *s)
{
	for (int i = 0; i < strlen(s); i++)
	{
		s[i] = s[i] - 32;
	}
}
int main(int argc, char const *argv[])
{
	// valid number of argument
	time_t start, end;
	double diff_t;
	char answer[11];
	if (argc != 3)
	{
		printf("Usage: ./client IPAddress PortNumber\n\n");
		return 0;
	}

	int client_sock;
	char username[MAX], password[MAX], buff[8192];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received;

	// Construct socket
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Request to connect server
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
	{
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	// Communicate with server
	while (1)
	{
		// get username
		bzero(username,MAX);
		puts("\nHãy đăng nhập vào hệ thống!!\n");
		printf("Tên người dùng: ");
		scanf("%[^\n]%*c", username);
		//puts(username);
		username[strcspn(username, "\n")] = '\0';

		// send username to server
		if (0 >= (bytes_sent = send(client_sock, username, strlen(username), 0)))
		{
			printf("\nConnection closed!\n");
			return 0;
		}

		// receive server reply
		if (0 >= (bytes_received = recv(client_sock, buff, 8192, 0)))
		{
			printf("\nError!Cannot receive data from sever!\n");
			return 0;
		}

		// exit if user not found on server
		buff[bytes_received] = '\0';
		if (0 == strcmp(buff, "0"))
		{
			puts("Không tìm thấy tên tài khoản\n");
			continue;
		}
		else if (0 == strcmp(buff, "2"))
		{
			puts("Tài khoản đã bị khóa,đang đóng...\n");
			continue;
		}

		// get password
		printf("Mã bảo mật: ");
		fgets(password, MAX, stdin);
		password[strcspn(password, "\n")] = '\0';

		// send password to server
		if (0 >= (bytes_sent = send(client_sock, password, strlen(password), 0)))
		{
			printf("\nConnection closed!\n");
			return 0;
		}

		// receive server reply
		memset(buff, '\0', MAX);
		if (0 >= (bytes_received = recv(client_sock, buff, 8192, 0)))
		{
			printf("\nError!Cannot receive data from sever!\n");
			return 0;
		}
		buff[bytes_received] = '\0';

		// analyze server reply
		if (0 == strcmp(buff, "0"))
		{ // if pass is wrong < 3 times
			puts("\nMã bảo mật nhập sai,Xin mời nhập lại\n");
			continue;
		}
		else if (0 == strcmp(buff, "2"))
		{ // if pass is wrong 3 times
			puts("\nNhập sai quá 3 lần mã bảo mật,tài khoản của bạn đã bị khóa\n");
			return 0;
		}
		else
		{ // if pass is right
			fflush(stdin);
			int index = 0;
			char *listQuestions[20];
			char *toSpilit = "$";
			char *token = strtok(buff,toSpilit);
			
			while(token!=NULL){
				listQuestions[index] = token;
				token = strtok(NULL,toSpilit);
				index++;
			}
			time(&start);
			printf("\n\nĐăng nhập thành công\n=====================================\n******* Mời bạn trả lời các câu hỏi sau ******\n\n");
			for(int i=0;i<index-1;i++){
				char *ans=(char*)malloc(sizeof(char));
				printf("%s\n",listQuestions[i]);
				printf("Đáp án của bạn là: "); scanf("%s", ans);
				// printf("%s.\n",ans);
				strcat(answer,ans);
			}
			time(&end);
			diff_t = (difftime(end, start));
			int phut = diff_t/60;
			printf("\n===============================================\nChúc mừng bạn đã hoàn thành bài thi\n===============================================\n");
   			printf("Thời gian làm bài của bạn là: %d phút %.0f giây\n",phut, (diff_t - phut*60));
			// printf("%s",answer);
			// scanf("%[^\n]%*c", answer);
			Up(answer);
			if (0 >= (bytes_sent = send(client_sock, answer, strlen(answer), 0)))
			{
				printf("\nConnection closed!\n");
				return 0;
			}
			memset(buff, 0, 8192);
			if (0 >= (bytes_received = recv(client_sock, buff, 8192, 0)))
			{
				printf("\nError!Cannot receive data from sever!\n");
				return 0;
			}
			printf("\nBạn trả lời đúng: %s", buff);
			break;
		}
	}
	// Close socket
	close(client_sock);
	return 0;
}
