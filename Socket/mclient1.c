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
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
    ///////////////////////////////////////////////
    char txtname[200];
    matrix *ptrmatrix;
    matrix *pommatrix;
    pommatrix = (matrix *) malloc(sizeof(matrix));
    ptrmatrix = (matrix *) malloc(sizeof(matrix));
    int matica[MATMAX][MATMAX]; 
    int shmid;
    key_t key;
    double roots[MATMAX];
    char alf[30];
    int end[1];
    ///////////////////////////////////////////////
    

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
        int polek[1];

    	if(recv(clientSocket, polek, sizeof(polek), 0) < 0)
    	{
    	printf("[-]Error in recieving data(key)\n");
    	}else
    	{
             if(polek[0] < 0)// ak pride kluc < 1 tak vypise error
            {
                printf("[-]Subor s nazvom: %s sa nenasiel!\n", txtname);
                exit(1);
            }
    		printf("[DEBUG]Toto je kluc%d\n", polek[0]);
            //
            key = polek[0];
            //////////////////////// zapisanie do SHM //////////////////////
            if ((shmid = shmget(key, sizeof(matrix),0666)) < 0) {
                perror("[-]shmget");
                exit(1);
            }

            /*
            * Now we attach the segment to our data space.
            */
            if ((ptrmatrix = (matrix*)shmat(shmid, (void*)0, 0)) == (matrix *) -1) {
                perror("[-]shmat");
                exit(1);
            }else{
            
            
           //////////////////////// zapisanie do SHM //////////////////////
                pommatrix = ptrmatrix;

                int n = ptrmatrix->rozmer;///// toto je zbitocne ak tak upravit
                int mat_koef[MATMAX];
                
                printf("[+] Nacitala sa matica %dx%d\n", pommatrix->rozmer, pommatrix->rozmer);
                //printf("[DEBUG]V zdielanej pamati je toto:%d(romer matice)\n", ptrmatrix->rozmer);
                Vypis(pommatrix->mat, ptrmatrix->rozmer); 

                ////// nacitanie rozsirenej matice ///////
                printf("Zadajte koeficienty rozsirenej matice:\n");
                for(int i = 0; i < pommatrix->rozmer; i++)
                {
                    printf("\t Zadajte %d. koeficient rozsirenej matice:", i+1);
                    scanf("%d", &mat_koef[i]);

                }
                send(clientSocket, mat_koef, sizeof(mat_koef), 0);
               // printf("\n toto su koeficienty %d %d %d", mat_koef[0], mat_koef[1], mat_koef[2]);
                if(recv(clientSocket, roots, sizeof(roots), 0) < 0)
                {
                    printf("[-]Error in recieving data(key)\n");

                }else
                {
                    printf("Riesenia rovnic:\n");
                    for(int i = 0; i < pommatrix->rozmer; i++)
                    {
                        printf("\tRiesenie %d. rovnice = %f\n", i+1, roots[i]);// pride iba prvy x v poli
                    }

                }
                end[0] = 1;
                send(clientSocket, end, sizeof(end), 0);// akdostane vysledok posle naspat nulu ktora znaci ze skoncil
            //
            }
    	}
    


   

	return 0;
}
