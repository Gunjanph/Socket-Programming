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
#include <arpa/inet.h>
extern int errno;
#define PORT 8080
typedef struct sockaddr_in Sin;
typedef struct sockaddr S;

int clientfd;

void recv_file()
{
	while(1)
	{
		int retv;
		char buffer[66000];
		//bzero(buffer,66000-1);
		char *str = (char *)malloc(sizeof(char)*1024);
		//bzero(str,1024-1);
		char command[100][1024];
		/*for(int i=0;i<100;i++)
			bzero(command[i],1024-1);*/
		size_t bufsize = 1024;
		getline(&str,&bufsize,stdin);
		if(strcmp(str,"\0")==0)
		{
			//printf("1\n");
			continue;
		}
		str[strlen(str)-1]='\0';
		// printf("%s\n",str );

		// DIVING STRING IN SPACES
		int j=0;
		char* token = strtok(str, " "); 
		while (token != NULL) 
		{ 
			//		printf("1: %s\n", token);
			strcpy(command[j],token);
			token = strtok(NULL, " ");
			j++; 
		}
		if(strcmp(command[0],"exit") == 0)
		{
			// printf("in exit\n");
			write(clientfd,command[0],strlen(command[0]));
			read(clientfd,str,4);
			printf("Connection with the server is closed\n");
			fflush(NULL);
			break;
		}
		else if(strcmp(command[0],"get") == 0)
		{
			if(j == 1)
			{
				printf("Files not provided\n");
				fflush(NULL);
			}
			else
			{
				for(int i=1;i<j;i++)
				{
					// printf("%d\n",i);
					fflush(NULL);
					int fd = open(command[i],O_CREAT|O_WRONLY|O_TRUNC, 0644);
					if(fd == -1)
					{
						perror("Error in creating or opening a file");
						exit(EXIT_FAILURE);
					}
					else
					{
						write(clientfd, command[i], strlen(command[i]));
						printf("File name sent to server is %s\n", command[i]);
						fflush(NULL);
						// CODE FOR ERROR FILE
						//bzero(buffer,66000-1);
						retv=read(clientfd, buffer, 65000);
						buffer[retv] = '\0';
						// printf("%s\n",buffer);
						fflush(NULL);
						write(clientfd, buffer, strlen(buffer));
						if(strcmp(buffer,"open")==0)
						{
							int bytes_per_read,n,m;
							char num[20];
							int length;
							retv=read(clientfd,num,20);
							num[retv]='\0';
							length = atoi(num);
							// printf("%d\n", length);
							write(clientfd,num,strlen(num));

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

							int k=0,sum=0;
							while(n--)
							{
								//for(int i=0;i<65005;i++)
								//	buffer[i]='\0';
								retv=read(clientfd,buffer,bytes_per_read);
								buffer[bytes_per_read] = '\0';
								// printf("%s\n", buffer);
								write(fd,buffer,bytes_per_read);
								sum+=bytes_per_read;
								long double percentage = (long double)(((long double)sum/(long double)length)*100);
								printf("\033[0;36m");
								printf("\r%0.2Lf%% DOWNLOADING", percentage);
								printf("\033[0m");
								fflush(NULL);
								//printf("To be sent is %s\n",buffer);
								write(clientfd,buffer,bytes_per_read);
							}
							if(m!=0)
							{
								// bzero(buffer, sizeof(buffer));
								//for(int i=0;i<65000;i++)
								//	buffer[i]='\0';
								bytes_per_read = m;
								read(clientfd,buffer,bytes_per_read);
								buffer[bytes_per_read] = '\0';
								//printf("c1: %s\n", buffer);
								write(fd,buffer,bytes_per_read);
								sum=length;
								long double percentage = (long double)(((long double)sum/(long double)length)*100);
								printf("\033[0;36m");
								printf("\r%0.2Lf%% DOWNLOADING", percentage);
								printf("\033[0m");
								fflush(NULL);
								write(clientfd,buffer,bytes_per_read);
							}
							read(clientfd,buffer,bytes_per_read);
							if(length == 0)
							{
								printf("\033[0;36m");
								long double per = 100;
								printf("\r%0.2LF%% DOWNLOADING", per);
								printf("\033[0m");
								fflush(NULL);
							}
							printf("\nFile Download Completed\n");
							fflush(NULL);
							close(fd);
						}
						else if(strcmp(buffer,"error")==0)
						{
							close(fd);
							continue;
						}
					}
				}
			}
		}
		else
		{
			printf("\033[0;31mRequired commands are get or exit\n");
			printf("\033[0m");
			fflush(NULL);
		}
	}
}

int main()
{
	// CREATING SOCKET
	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd == -1)
	{
		perror("Error in socket");
		exit(0);
	}
	else
		printf("Socket is successfully created\n");

	// ASSIGNING IP, PORT
	Sin serveradd;
	int serveradd_length = sizeof(serveradd);
	bzero(&serveradd, (size_t)serveradd_length);

	serveradd.sin_family = AF_INET; 
	serveradd.sin_port = htons(PORT);

	// CONVERT ADDRESS TO BINARY
	int inet;
	inet = inet_pton(AF_INET, "127.0.0.1", &serveradd.sin_addr);
	if(inet == -1)
	{
		perror("Invalid Address");
		exit(0);
	}

	// CONNECT CLIENT TO SERVER
	int connint;
	connint = connect(clientfd, (S*)&serveradd, sizeof(serveradd));
	if(connint == -1)
	{
		perror("Error in connecting");
		exit(0);
	}
	else
		printf("Client is successfully Connected to Server\n");


	// FUNCTION
	recv_file();

	// CLOSING
	close(clientfd);
	return 0;
}
