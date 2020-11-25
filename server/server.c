#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
extern int errno;
#define PORT 8080
typedef struct sockaddr_in Sin;
typedef struct sockaddr S;
int connfd;


void send_file()
{
	while(1)
	{
		char buff[66000];
		char str[100];
		int retv;
		retv=read(connfd, buff, 65000);
		buff[retv]='\0';
		// printf("%s\n",buff );
		if(strcmp(buff,"exit") == 0)
		{
			write(connfd,"done",4);
			printf("Connection with the client is closed\n");
			fflush(NULL);
			break;
		}
		printf("File name recieved is %s\n", buff);
		fflush(NULL);
		int fd = open(buff, O_RDONLY);
		if(fd == -1)
		{
			perror("Error in opening a file");
			write(connfd,"error",5);
			retv=read(connfd,buff,65000);
			buff[retv]='\0';
			// printf("e/o:%s\n",buff);
			fflush(NULL);
			continue;
		}
		else
		{
			write(connfd,"open",4);
			retv=read(connfd,buff,65000);
			buff[retv]='\0';
			// printf("e/o:%s\n",buff);
			fflush(NULL);
			//bzero(buff,66000-1);
			int bytes_per_read,n,m;
			char num[20];
			//bzero(num, 20-1);
			int length = lseek(fd,0,SEEK_END);
			sprintf(num,"%d",length);
			// printf("%d\n", length);
			write(connfd, num, strlen(num));
			retv=read(connfd, num,19);
			num[retv]='\0';

			if(length<65000)
			{
				bytes_per_read=1;
				n=length/bytes_per_read;
				m=length%bytes_per_read;
			}
			else
			{
				n=length/65000;
				m=length%65000;
				bytes_per_read=65000;
			}

			//	INITIALISING POINTER
			lseek(fd, 0, SEEK_SET);

			while(n--)
			{
				//bzero(buff, 65005);
				// printf("%s\n",buff);
				read(fd,buff,bytes_per_read);
				buff[bytes_per_read] = '\0';
				// printf("%s\n",buff);
				write(connfd,buff,bytes_per_read);
				//	printf("c:%s\n",buff);
				retv=read(connfd,buff,bytes_per_read);
				buff[retv]='\0';
				//printf("read is %s\n",buff);
			}

			if(m!=0)
			{
				//bzero(buff, 65005);
				bytes_per_read = m;
				read(fd,buff,bytes_per_read);
				buff[bytes_per_read] = '\0';
				//printf("1: %s\n",buff);
				write(connfd,buff,bytes_per_read);
				retv=read(connfd,buff,bytes_per_read);
				buff[retv]='\0';
			}
			write(connfd,buff,bytes_per_read);
			printf("File sent to Client\n");
			close(fd);
		}
	}
}

int main()
{
	// CREATING SOCKET
	int serverfd;
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd == -1)
	{
		perror("Error in socket");
		exit(0);
	}
	else
		printf("Socket is successfully created\n");

	// CHECKING AVAILABILITY OF SOCKET AND REUSING PORT
	int set, value = 1;
	set = setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char*)&value, sizeof(value));
	if(set == -1)
	{
		perror("Error in attaching socket to port");
		exit(0);
	}
	else
		printf("Socket is successfully attached to port\n");

	// ASSIGNING IP, PORT
	Sin serveradd;
	int serveradd_length = sizeof(serveradd);
	bzero(&serveradd, serveradd_length);

	serveradd.sin_family = AF_INET; 
	serveradd.sin_addr.s_addr = INADDR_ANY; 
	serveradd.sin_port = htons(PORT); 

	// BINDING AND VERIFYING
	int bindint;
	bindint = bind(serverfd, (S*)&serveradd, sizeof(serveradd));
	if(bindint == -1)
	{
		perror("Error in binding");
		exit(0);
	}
	else
		printf("Socket is successfully binded\n");

	// LISTENING AND CHECKING QUEUE
	int listenint;
	listenint = listen(serverfd, 2);
	if(listenint == -1)
	{
		perror("Error in listening");
		exit(0);
	}
	else
		printf("Server is listening successfully\n");

	// ACCEPTING 1ST REQUEST IN QUEUE
	connfd = accept(serverfd, (S*)&serveradd, (socklen_t*)&serveradd_length);
	if(connfd == -1)
	{
		perror("Error in accepting");
		exit(0);
	}
	else
		printf("Server successfully accepted the client\n");

	// FUNCTION
	send_file();

	// CLOSING FILE DESCRIPTOR
	close(serverfd);

	return 0;
}
