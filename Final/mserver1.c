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
#include <signal.h>
#include <pthread.h>
#include <time.h>



#define PORT 9834
#define RECMAX 20 // maximany pocet zaznamov
#define MATMAX 200 // velkost najvacsiej matice
#define online_clients_info 1 	// *				*//
#define port_info 2				// * Info switches	*//
#define records_info 3			// *				*//
#define printinfo printf("\t[*]\t  Server info\t\t[*]\n\t[*] Print online client - 1\t[*]\n\t[*] Print PORT - 2\t\t[*]\n\t[*] Print record info - 3\t[*]\n")

int num_of_client = 0;
int childdata;
pid_t childPID;
timer_t vytvorCasovac(int);
void spustiCasovac(timer_t, int);

void sigusr1();

typedef struct matrix
{
  int rozmer;
  int mat[MATMAX][MATMAX];
}matrix;

typedef struct table
{
 int rozmer_table[RECMAX];
 int pid_table[RECMAX];
 int key_table[RECMAX];
 int num_of_records;
 int id[RECMAX];
}table;

void *myThread()
{
	int a = 0;
	int record;
	table *ptrtable2;
	
    ptrtable2 = (table *) malloc(sizeof(table));
    int matica[MATMAX][MATMAX]; 
    int shmid2;
	key_t key2;
	key2 = 9804;
//////////////////////////////////////////////////
	//////// nacitanie z main SHM ////////
	if ((shmid2 = shmget(key2, sizeof(table),0666)) < 0) {
        perror("shmget1");
        exit(1);
    }

  
    if ((ptrtable2 = (table*)shmat(shmid2, (void*)0, 0)) == (table *) -1) {
        perror("shmat");
        exit(1);
    }
//////////////////////////////////////////////////

	while(1)
	{
    	scanf("%d", &a);
    	switch(a)
    	{
    		case online_clients_info:
    			printf("Online clients: %d\n", num_of_client);
    			break;
    		case port_info:
    			printf("Port: %d\n", PORT);
    			break;
    		case records_info:
    			printf("Number of records %d\n", ptrtable2->num_of_records-1);
    			printf("Enter number:");
    			scanf("%d", &record);
    			printf("\tRecord ID: %d\n", ptrtable2->id[record]);
    			printf("\tProces ID:%d\n", ptrtable2->pid_table[record]);
    			printf("\tKey:%d\n", ptrtable2->key_table[record]);
    			printf("\tDimension od matrix:%d\n", ptrtable2->rozmer_table[record]);

    			break;	
    	}
      
    } 
    free(ptrtable2); 
}


void sigFlag(int signum)// ak skonci proces potomka odpocita sa premena podla ktorej rozhodujeme vypnutie servera
{
	wait(NULL);
	printf("[+]Client disconnected\n");
	num_of_client --;

}

int **create(int n)
{
	int **p, i;
	p = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		p[i] = (int *)malloc(n * sizeof(int));
	return p;
}

void VypisDym(int **pole, int n) {
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

void matfillDym(int matin[MATMAX][MATMAX], int **matout, int rozmer) // funkcia ktora naplni DYNAMICKU maticu inou STATICKOU maticou [nic nevracia]
{

	for (int x = 0; x < rozmer; x++)

	{

		for (int y = 0; y < rozmer; y++)

		{
			matout[x][y] = matin[x][y];
		}

	}

}

void getCofactor(int **mat, int **temp, int p, int q, int n)
{
	int i = 0, j = 0;

	// prejde kazdu cast matice
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			//  vyberie prvky da ich do docasnej amtice
			if (row != p && col != q)
			{
				temp[i][j++] = mat[row][col];

				if (j == n - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
}


int determinantOfMatrix(int **mat, int n)
{
	int D = 0; // Vynuluje vysledok

	//  Ak je matica 1 x 1 
	if (n == 1)
		return mat[0][0];

	int **temp = create(n);// docasna matica pre ulozenie matice //dynamicka

	int sign = 1;  // Znamienko sa meni zacina na 1 

	 
	for (int f = 0; f < n; f++)
	{
		
		getCofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);// pouziva rekurzovu pre pocitanie

		
		sign = -sign;// po kazdom sa znamienok meni
	}
	for (int i = 0; i < n; i++)	//** uvolni pamat 	**//
		free(temp[i]);			//** pre docasnu	**//
	free(temp);					//** maticou		**//

	return D;
}



int *Eq_solver(int **table1, int n, int ext[MATMAX])
{
	int **temp1 = create(n);
	int *det = (int*)malloc(n * sizeof(int));
	det[n] = determinantOfMatrix(table1, n);// determinant hlavnej matice pripoji na posledne misto pola
	if(det[n] == 0)
	{
		printf("Delenie nulou1");
		exit(1);
	}
	for (int m = 0; m < n; m++)
	{

		for (int x = 0; x < n; x++)

		{

			for (int y = 0; y < n; y++)

			{
				temp1[x][y] = table1[x][y];
			}

		}

		for (int j = 0; j < n; j++)

		{

			for (int k = 0; k < n; k++)

			{
				if (k == m)
				{
					temp1[j][k] = ext[j];
				}

			}

		}
		det[m] = determinantOfMatrix(temp1, n);
		
/////////////////////////////////////////////////////
	}
for (int i = 0; i < n; i++)	
		free(temp1[i]);
	free(temp1);
	return det;

}



int read1(int matout[MATMAX][MATMAX], char subor[50],int *num ) // funkcia pre nacitanie matice zo suboru
{
	FILE *fr;
	int sx = 0, sy = 0, min;
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("[-]Subor sa nepodarilo otvorit.\n");
		return -1;
	}

	fscanf(fr, "%d\n", &*(num));
	for (int x = 0; x < (*num); x++)

	{

		for (int y = 0; y < (*num); y++)

		{
			fscanf(fr, "%d", &matout[x][y]);
		}

	}
	fclose(fr);
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

double *roots_of_eq(int *determinants, int n)
{
	
	double *pole = (double*)malloc(n * sizeof(double));
	//printf("\n\t roots_of_determinant %d\n", determinants[n]);
	for (int i = 0; i < n; i++)
	{
		if (determinants[n] == 0)
			exit;
		pole[i] = ((double)determinants[i]) / ((double)determinants[n]);
		//printf("\n\t%d / %d = %f \n", determinants[i], determinants[n], pole[i]);

	}

	return pole;
	
}






int main () 
{
	int sockfd, ret, newSocket;
	struct sockaddr_in serverAddr;

	socklen_t addr_size;

	struct sockaddr_in newAddr;
	
	char buffer[1024];
///////////////////////////////////////////////
			//////// SHM ////////
	char txtname[200];
	matrix *ptrmatrix;
	matrix *pommatrix;
	table *ptrtable;
	
    ptrmatrix = (matrix *) malloc(sizeof(matrix));
    ptrtable = (table *) malloc(sizeof(table));
    int matica[MATMAX][MATMAX]; 
    int shmid;
	key_t key;
	int readerr;
	int polek[1];
///////////////////////////////////////////////
			////Determinant////
	int **maticaDym;
	int *det;
	double *roots;
	int end[1];
///////////////////////////////////////////////
			//////// Thread ////////
	pthread_t tid1;
	pthread_create(&tid1, NULL, myThread, NULL); 

///////////////////////////////////////////////
			//////// main SHM ////////
	int shmid1;
	key_t key1 = 9804;

	 if ((shmid1 = shmget(key1, sizeof(table), IPC_CREAT | 0666)) < 0) {
        perror("shmget2");
        exit(1);
    }

    if ((ptrtable = (table*)shmat(shmid1, (void*)0, 0)) == (table*) -1) {
        perror("shmat");
        exit(1);
    }
    ptrtable->num_of_records = 1;
///////////////////////////////////////////////
    		//////// Timer ////////
		timer_t casovac;
  		signal(SIGUSR1, sigusr1);
  		casovac = vytvorCasovac(SIGUSR1);
  		spustiCasovac(casovac,30);
  		signal(SIGCHLD,sigFlag);// zachytava sa signal ktory posle potomok ked umiera
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
	printinfo;
	
	while(1)
	{
		newSocket = accept(sockfd, (struct sockaddr*)&serverAddr, &addr_size);
		if(newSocket < 0)
		{
			exit(1);
		}
		printf("[+]Connection accepted\n");
		num_of_client++;// pripojil sa klient

		if (childPID = fork() == 0)
		{
			close(sockfd);
			

				recv(newSocket, buffer, 1024, 0);
				if(strcmp(buffer, "exit") == 0)
				{
					printf("[-]Client disconected\n");
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
					txtname[i] = '\0';// na poslene miesto nul. znak
					printf("Matrix from: %s\n", txtname);

					if((key = ftok(txtname, 'R')) < 0)//Vytvorenie unikatneho kluca pre kazdy subor
					{
						polek[0] = key;// musi byt
						send(newSocket, polek, sizeof(polek), 0);// posle -1 ako kluc
						printf("[-]File not found\n");// osetrenie ak sa nenajde subor
  						exit(1);
					}
					if ((shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666)) < 0) {
        				perror("shmget");
        				exit(1);
    				}

    			
    				if ((ptrmatrix = (matrix*)shmat(shmid, (void*)0, 0)) == (matrix *) -1) {
        				perror("shmat");
        				exit(1);
    				}
  					if (read1(matica, txtname, &(ptrmatrix->rozmer)) < 0)
  					{
  						printf("[-]File not found\n");
  						exit(1);
  					}
    			
    				matfill(matica, ptrmatrix->mat, ptrmatrix->rozmer);

    				///////////////////////////////////////////////
    						//////// Recording ////////
    				ptrtable->rozmer_table[ptrtable->num_of_records] = ptrmatrix->rozmer;
    				ptrtable->pid_table[ptrtable->num_of_records] = getpid();
    				ptrtable->key_table[ptrtable->num_of_records] = key;
    				ptrtable->id[ptrtable->num_of_records] = ptrtable->num_of_records;//
    				ptrtable->num_of_records++;// zapocita sa len ked vsetko zapise
    				///////////////////////////////////////////////

    				polek[0] = key;
    				
    				send(newSocket, polek, sizeof(polek), 0);
					printf("Matrix in SHM:\n");
					Vypis(ptrmatrix->mat, ptrmatrix->rozmer); 

					///////////////////////////////////////////////
						/////// Ratanie Determinantu ///////
					maticaDym = create(ptrmatrix->rozmer);
					matfillDym(ptrmatrix->mat, maticaDym, ptrmatrix->rozmer);// naplnenie dynamickej matice [musi byt dynamicka]
					int mat_koef[MATMAX];
					recv(newSocket, mat_koef,sizeof(mat_koef), 0);
					roots = roots_of_eq(Eq_solver(maticaDym, ptrmatrix->rozmer, mat_koef), ptrmatrix->rozmer);//vypocita korene rovnic
					///////////////////////////////////////////////
					
					
					
					send(newSocket, roots, sizeof(double)*ptrmatrix->rozmer, 0);// posle korene rovnic
					recv(newSocket, end,sizeof(end), 0);
					if(end[0] == 1)
					{
						printf("[+]Equations from: %s solved\n",txtname);

					}
					
				}
			
		}
		

    }
  
	for (int i = 0; i < ptrmatrix->rozmer; i++)	
		free(maticaDym[i]);
	free(maticaDym);				
    free(ptrmatrix);
    free(ptrtable);
	free(roots);
	free(det);
    close(newSocket);

	return 0;
}


timer_t vytvorCasovac(int signal)
{
  struct sigevent kam;
  kam.sigev_notify=SIGEV_SIGNAL;
  kam.sigev_signo=signal;
  
  timer_t casovac;
  timer_create(CLOCK_REALTIME, &kam, &casovac);
  return(casovac); 
}


void spustiCasovac(timer_t casovac, int sekundy)
{
  struct itimerspec casik;
  casik.it_value.tv_sec=sekundy;
  casik.it_value.tv_nsec=0;
  casik.it_interval.tv_sec=sekundy;
  casik.it_interval.tv_nsec=0;
  timer_settime(casovac,CLOCK_REALTIME,&casik,NULL);
}
void sigusr1()//handler pre signal
 {
 	signal(SIGUSR1, sigusr1);
 	printf("\n[+]Timer check -> \tOnline clients:%d\n", num_of_client);//[DEBUG]//
 	
 	if (num_of_client  <= 0)
 	{
 		printf("[-]Connection timeout\n"); //[DEBUG]//
 		exit(0);
 	}else
 		{
 			printinfo;
 		}

 } 
