#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>


#define PORT 9834
#define MATMAX 200 // velkost najvacsiej matice

typedef struct matrix
{
  int rozmer;
  int mat[MATMAX][MATMAX];
}matrix;





int read1(int matout[MATMAX][MATMAX], char subor[50],int *num ) // funkcia pre nacitanie matice zo suboru
{
	FILE *fr;
	//int **table;
	int sx = 0, sy = 0, min;
	//char subor[] = {"Vstup.txt"};
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit.\n");
		return -1;
	}

	fscanf(fr, "%d\n", &*(num));
	//table = create1(*(num));


	for (int x = 0; x < (*num); x++)

	{

		for (int y = 0; y < (*num); y++)

		{
			fscanf(fr, "%d", &matout[x][y]);
		}

	}

	return 0;

}

void matfill(int matin[MATMAX][MATMAX], int matout[MATMAX][MATMAX], int rozmer) // funkcia ktora naplni maticu inou maticou [nic nevracia]
{
	
	for (int x = 0; x < rozmer; x++)

	{

		for (int y = 0; y < rozmer; y++)

		{
			matout[x][y] = matin[x][y];
		}

	}
	
}


void Vypis(int pole[MATMAX][MATMAX], int n) { // Funkcia pre vypis matice
	for (int x = 0; x < n; x++)
	{
		for (int y = 0; y < n; y++)

		{
			printf("%d ", pole[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}

int main () 
{
	int sockfd, ret, newSocket;
	struct sockaddr_in serverAddr;

	socklen_t addr_size;

	struct sockaddr_in newAddr;
	pid_t childPID;
	char buffer[1024];
///////////////////////////////////////////////
	//////// SHM ////////
	char txtname[200];
	matrix *ptrmatrix;
	matrix *pommatrix;
	pommatrix = (matrix *) malloc(sizeof(matrix));
    ptrmatrix = (matrix *) malloc(sizeof(matrix));
    int matica[MATMAX][MATMAX]; 
    int shmid;
	key_t key;
	int readerr;
	int polek[1];
///////////////////////////////////////////////


	sockfd = socket(AF_INET, SOCK_STREAM, 0);// vytvorenie socketu
	if(sockfd < 0)
	{
		printf("[-]Error in connection\n");
		exit(1);
	}
	printf("[+]Server socket is created\n");

	memset(&serverAddr, '\0', sizeof(serverAddr)); //inicializacia vsetkych parametrov pre spojenie
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0)
	{
		printf("[-]Error in binding\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);

	if(listen(sockfd, 10) == 0) // pocuva caka kym sa niketo ozve
	{
		printf("[+]Listening...\n");
	}else
	{
	printf("[-]Error in binding\n");
	}

	while(1)
	{
		newSocket = accept(sockfd, (struct sockaddr*)&serverAddr, &addr_size);
		if(newSocket < 0)
		{
			exit(1);
		}
		printf("[+]Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
	

		if (childPID = fork() == 0)
		{
			close(sockfd);
			
			
				recv(newSocket, buffer, 1024, 0);
				if(strcmp(buffer, "exit") == 0)
				{
					printf("[-]Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}else
				{
					int i;
					for(i = 0; i < strlen(buffer); i++)
					{
						if(buffer[i] == '\0')
							break;
						txtname[i] = buffer[i];

					}
					txtname[i] = '\0';// bez pridania nuli to vypisovalo hovadiny
					printf("[DEBUG]Nazov matice: %s\n", txtname);

					
					
					if((key = ftok(txtname, 'R')) < 0)//Vytvorenie unikatneho kluca pre kazdy subor
					{
						polek[0] = key;// musi byt
						send(newSocket, polek, sizeof(polek), 0);// posle -1 ako kluc
						printf("[-]Subor sa nenasiel\n");// osetrenie ak sa nenajde subor
  						exit(1);
					}
					if ((shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666)) < 0) {
        				perror("shmget");
        				exit(1);
    				}

    				/*
     				*Now we attach the segment to our data space.
     				*/
    				if ((ptrmatrix = (matrix*)shmat(shmid, (void*)0, 0)) == (matrix *) -1) {
        				perror("shmat");
        				exit(1);
    				}
  					if (read1(matica, txtname, &(ptrmatrix->rozmer)) < 0)//***************************************************************
  					{
  					
  						printf("[-]Subor sa nenasiel\n");
  						exit(1);
  					}
    			
    				matfill(matica, ptrmatrix->mat, ptrmatrix->rozmer);
    			
    				polek[0] = key;
    				printf("[DEBUG]Toto je kluc: %d\n", key);//*************************DEBUG*************************//
    				send(newSocket, polek, sizeof(polek), 0);
					printf("[DEBUG]Do pamate sa zapisalo :\n");
					Vypis(ptrmatrix->mat, ptrmatrix->rozmer); 

				}
			
		}

    }

    close(newSocket);
	return 0;
}
