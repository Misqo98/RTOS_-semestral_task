#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 9834

int main () 
{
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
    char txtname[200];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);// vytvorenie socketu
	if(clientSocket < 0)
	{
		printf("[-]Error in connection\n");
		exit(1);
	}
	printf("[+]Client socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
    {
    	printf("[-]Error in connection\n");
		exit(1);
    }
    printf("[+]Connected to Server\n");

    

    	printf("Zadajte nazov suboru pre nacitanie matice: \t");
    	scanf("%s", txtname);
        strcpy(buffer, txtname);////////
        buffer[sizeof(buffer)] = '\n';////////
    	send(clientSocket, buffer, sizeof(buffer), 0);

    	if(strcmp(buffer, "exit") == 0)
    	{
    		close(clientSocket);
    		printf("[-]Disconnected from server\n");
    		exit(1);
    	}
        /*
    	if(recv(clientSocket, buffer, 1024, 0) < 0)
    	{
    	printf("[-]Error in recieving data\n");
    	}else
    	{
    		printf("Server: \t%s\n", buffer);
    	}
    */


   

	return 0;
}
