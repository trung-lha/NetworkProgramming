#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define BACKLOG 5
#define MAX 8192
#define QUES_FILENAME "question.txt"
#define QUES_IN_LEVER 10
#define QUES_NUMBER 30
// account node

typedef struct node
{
	char username[MAX];
	char password[MAX];
	int status;
	int point;
	struct node *next;
} node_t;
typedef struct question
{
	int id;
	int level; //1 : easy, 2 : medium, 3 : hard
	char content[200];
	char choiceA[50];
	char choiceB[50];
	char choiceC[50];
	char choiceD[50];
	char answer;
} Question;
typedef struct message
{
	char content[8192];
	char answer[11];
} Message;
Question questionList[QUES_NUMBER];
int easyList[QUES_IN_LEVER];
int mediumList[QUES_IN_LEVER];
int hardList[QUES_IN_LEVER];
char* headQuestion[100] = {"Câu 1: ","Câu 2: ","Câu 3: ","Câu 4: ","Câu 5: ","Câu 6: ","Câu 7: ","Câu 8: ","Câu 9: ","Câu 10: ","Câu 11: ","Câu 12: ","Câu 13: ","Câu 14: ","Câu 15: "};
int easyIndex = 0, mediumIndex = 0, hardIndex = 0;
Message buff;

// load data from text file to linked list
node_t *load_data(char *filename)
{
	int status, point, count = 0; // number of accounts
	FILE *f;
	char username[MAX], password[MAX];
	node_t *head, *current;
	head = current = NULL;

	// open file data
	printf("Loading data from file...\n");

	if ((f = fopen(filename, "r")) == NULL)
	{
		printf("Cannot open file!\n");
		exit(0);
	}

	// load accounts to linked list
	while (fscanf(f, "%s %s %d %d\n", username, password, &status, &point) != EOF)
	{
		// create new node
		node_t *node = malloc(sizeof(node_t));
		strcpy(node->username, username);
		strcpy(node->password, password);
		node->status = status;
		node->point = point;
		// add node to list
		if (head == NULL)
			current = head = node;
		else
			current = current->next = node;
		count++;
	}

	fclose(f);
	printf("LOADED SUCCESSFULY %d ACCOUNT(S)\n", count);
	return head;
}

// find a node exist in linked list given username
node_t *find_node(node_t *head, char *username)
{
	node_t *current = head;
	while (current != NULL)
	{
		if (0 == strcmp(current->username, username))
			return current;
		current = current->next;
	}
	return NULL;
}

// ghi đè lại toàn bộ dữ liệu về người dùng, những thay đổi về thông tin người dùng khi chạy chương trình
void save_list(node_t *head, char *filename)
{
	FILE *f;
	f = fopen(filename, "w");
	node_t *current;
	for (current = head; current; current = current->next)
		fprintf(f, "%s %s %d %d\n", current->username, current->password, current->status, current->point);
	fclose(f);
}

// Lưu lại các câu hỏi khó, dễ, trung bình vào trong 3 mảng được khai báo phía trên
int addLeverList(int id, int lv)
{
	switch (lv)
	{
	case 1:
		easyList[easyIndex] = id;
		easyIndex++;
		break;
	case 2:
		mediumList[mediumIndex] = id;
		mediumIndex++;
		break;
	case 3:
		hardList[hardIndex] = id;
		hardIndex++;
		break;
	default:
		break;
	}
	return 1;
}

void lastReq(node_t *head,char* bufff);

// đọc toàn bộ dữ liệu câu hỏi ra lưu vào mảng questionList[] đã khai báo phía trên
void readQues()
{
	FILE *fptr;
	int id, lv;
	char content[1000], a[1000], b[1000], c[1000], d[1000];
	char answ;
	int i = 0;
	fptr = fopen(QUES_FILENAME, "r");
	if (fptr == NULL)
	{
		printf("Can't open file !\n");
		return;
	}

	while (fscanf(fptr, "%d | %d | %[^|] | %[^|] | %[^|] | %[^|] | %[^|] | %c", &id, &lv, content, a, b, c, d, &answ) != EOF)
	{
		questionList[i].id = id;
		questionList[i].level = lv;
		strcpy(questionList[i].content, content);
		strcpy(questionList[i].choiceA, a);
		strcpy(questionList[i].choiceB, b);
		strcpy(questionList[i].choiceC, c);
		strcpy(questionList[i].choiceD, d);
		questionList[i].answer = answ;
		addLeverList(id, lv);
		i++;
	}
	fclose(fptr);
}

int checkExitsInArray(int *arr,int check){
	int count = sizeof(arr)/sizeof(int);
	for (int i =0;i<count;i++){
		if(arr[i] == check) return 1;
	}
	return 0;
}

void randomId(int *arr, int lv)
{

	if (lv == 1)
	{
		arr[0] = rand() % 9;
		do
		{
			arr[1] = rand() % 9;
		} while (arr[1] == arr[0]);
		do
		{
			arr[2] = rand() % 9;
		} while (arr[2] == arr[0] || arr[2] == arr[1]);
		do
		{
			arr[3] = rand() % 9;
		} while (arr[3] == arr[0] || arr[3] == arr[1] || arr[3] == arr[2]);
		return;
	}
	if (lv == 2)
	{
		arr[0] = rand() % 9;
		do
		{
			arr[1] = rand() % 9;
		} while (arr[1] == arr[0]);
		do
		{
			arr[2] = rand() % 9;
		} while (arr[2] == arr[0] || arr[2] == arr[1]);
		do
		{
			arr[3] = rand() % 9;
		} while (arr[3] == arr[0] || arr[3] == arr[1] || arr[3] == arr[2]);
		return;
	}
	if (lv == 3)
	{
		arr[0] = rand() % 9;
		do
		{
			arr[1] = rand() % 9;
		} while (arr[1] == arr[0]);
		return;
	}
}

// Tạo ra bộ đề thi
Message makeQuesList()
{
	int easy[4];
	int medium[4];
	int hard[2];
	int i, n;
	int numberQuestion = 0;
	Message buff;
	srand((int)time(0));
	randomId(easy, 1);
	randomId(medium, 2);
	randomId(hard, 3);
	for (i = 0; i < 4; i++)
	{
		n = easyList[easy[i]];
		strcat(buff.content, headQuestion[numberQuestion]);
		strcat(buff.content, questionList[n].content);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceA);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceB);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceC);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceD);
		strcat(buff.content, "\n\n");
		buff.answer[i] = questionList[n].answer;
		numberQuestion+=1;
		// printf("%s\n",questionList[n].content);
	}
	for (i = 0; i < 4; i++)
	{
		n = mediumList[medium[i]];
		strcat(buff.content, headQuestion[numberQuestion]);
		strcat(buff.content, questionList[n].content);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceA);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceB);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceC);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceD);
		strcat(buff.content, "\n\n");
		numberQuestion+=1;
		buff.answer[i + 4] = questionList[n].answer;
		//printf("%s\n",questionList[n].content);
	}
	for (i = 0; i < 2; i++)
	{
		n = hardList[hard[i]];
		strcat(buff.content, headQuestion[numberQuestion]);
		strcat(buff.content, questionList[n].content);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceA);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceB);
		strcat(buff.content, "\n");
		strcat(buff.content, questionList[n].choiceC);
		strcat(buff.content, "\t\t");
		strcat(buff.content, questionList[n].choiceD);
		strcat(buff.content, "\n\n");
		numberQuestion+=1;
		//printf("%s\n",questionList[n].content);
		buff.answer[i + 8] = questionList[n].answer;
	}
	//printf("%s",buff.content);
	return buff;
}

int checkAnswer(char *req, char *local)
{
	int point = 0;
	for (int i = 0; i < 10; i++)
	{
		if (req[i] == local[i])
			point++;
	}
	return point;
}

int main(int argc, char const *argv[])
{
	char filename[] = "account.txt";
	readQues();
	//printf("%s",buff.content);
	// valid number of argument
	if (argc != 2)
	{
		printf("Usage: ./server PortNumber\n\n");
		return 0;
	}

	int listen_sock, conn_sock, n;
	char username[MAX], password[MAX], answer[11], point[3], *reply,bee[1024]={0};
	int bytes_sent, bytes_received;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t sin_size;
	node_t *found;
	int pid;

	// load file txt to linked list
	node_t *account_list = load_data(filename);

	// Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	// Bind address to socket
	memset(&server, '\0', sizeof server);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("\nError: port number has to great than 1024");
		return 0;
	}

	// Listen request from client
	if (listen(listen_sock, BACKLOG) == -1)
	{
		perror("\nError: ");
		return 0;
	}

	puts("Server up and running...\n");

	// Communicate with client
	while (1)
	{
		// accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
			perror("\nError: ");

		printf("\nYou got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */

		// start conversation on other fork
		pid = fork();
		if (pid < 0)
		{
			perror("Error");
			return 1;
		}

		// on child process
		if (pid == 0)
		{
			int count = 0; // count password repeatation
			while (1)
			{
				if (0 >= (bytes_received = recv(conn_sock, username, MAX - 1, 0)))
				{
					printf("\nConnection closed 1\n");
					break;
				}
				username[bytes_received] = '\0';

				// check username existence
				if ((found = find_node(account_list, username)))
				{
					if (found->status == 1)
						reply = "1"; // username found
					else
						reply = "2"; // username found but has been locked
				}
				else
					reply = "0"; // username not found
				// echo to client
				if (0 >= (bytes_sent = send(conn_sock, reply, strlen(reply), 0)))
				{
					printf("\nConnection closed 2\n");
					break;
				}

				

				while (1)
				{
					// receive password
					memset(password, '\0', MAX);
					if (0 >= (bytes_received = recv(conn_sock, password, MAX - 1, 0)))
					{
						printf("\nConnection closed 3\n");
						break;
					}
					password[bytes_received] = '\0';

					// validate password
					if (0 == strcmp(found->password, password))
					{
						memset(&buff, '\0', sizeof(Message));
						buff = makeQuesList();
						reply = buff.content;
						//printf("Đáp án:\n%s\n", buff.answer);
						if (0 >= (bytes_sent = send(conn_sock, reply, strlen(reply), 0)))
						{
							printf("\nConnection closed 4\n");
							break;
						}
						if (0 >= (bytes_received = recv(conn_sock, answer, 11, 0)))
						{
							printf("\nConnection closed 5\n");
							break;
						}
						n = checkAnswer(answer, buff.answer);
						if (found->point < n)
							found->point = n;
						save_list(account_list, filename);
						snprintf(point, 3, "%d", n);
						strcat(bee,point);
						strcat(bee,"/10");
						strcat(bee,"\n\n");
						lastReq(account_list,bee);
						// printf("%s\n",bee);
						if (0 >= (bytes_sent = send(conn_sock, bee,strlen(bee), 0)))
						{
							printf("\nConnection closed 6\n");
							break;
						}
					}
					else
					{
						count++;
						if (count == 3)
						{
							reply = "2";	   // wrong pass 3 times, reply 2
							found->status = 0; // then lock account
							
						}
						else
							reply = "0"; // wrong pass < 3 times, reply 0
						if (0 >= (bytes_sent = send(conn_sock, reply, strlen(reply), 0)))
						{
							printf("\nConnection closed 7\n");
							break;
						}
						break;
					}

					// echo to client
				}
			}
			// save linked list state
			save_list(account_list, filename);
			close(conn_sock);
		}
		else
		{
			// on parent process
			close(conn_sock);
		}
	}
	close(listen_sock);
	return 0;
}
void lastReq(node_t *head,char* bufff){
   node_t *current;
   int m1=0,m2=0,m3=0;
   char x1[300]={0},x2[300]={0},x3[300]={0},n[3];
   // lấy ra số điểm 3 người có số điểm lớn nhất được lưu trong file account.txt
	for (current = head; current; current = current->next)
		if(current->point > m1) m1 = current->point;
	for (current = head; current; current = current->next)
		if(current->point > m2 && current->point < m1) m2 = current->point;
	for (current = head; current; current = current->next)
		if(current->point > m3 && current->point < m2) m3 = current->point;
	strcat(x1,"\nBảng xếp hạng hiện tại\nHạng 1:\t");
	strcat(x2,"\nHạng 2:\t");
	strcat(x3,"\nHạng 3:\t");

	// Lấy ra tên của người dùng ứng với 3 số điểm cao nhất
	for (current = head; current; current = current->next){
		if(current->point == m1) {
		strcat(x1,current->username);
		strcat(x1," | ");	
		}else if(current->point == m2){
		 strcat(x2,current->username);	
		 strcat(x2," | ");
		} else if(current->point == m3){
		 strcat(x3,current->username);
		 strcat(x3," | ");
		}
	}

	strcat(x1," với số điểm: ");
	strcat(x2," với số điểm: ");
	strcat(x3," với số điểm: ");

    snprintf(n, 3, "%d", m1);
    strcat(x1,n);
    snprintf(n, 3, "%d", m2);
    strcat(x2,n);
    snprintf(n, 3, "%d", m3);
    strcat(x3,n);
    x1[strlen(x1)] ='\0';
    x2[strlen(x2)] ='\0';
    x3[strlen(x3)] ='\0';
    strcat(bufff,x1);
    strcat(bufff,x2);
    strcat(bufff,x3);
    // printf("%s\n",bufff);
    return ;

}