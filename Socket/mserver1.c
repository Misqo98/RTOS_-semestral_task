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
//#include <pthread.h>
#include <time.h>



#define PORT 9834
#define MATMAX 200 // velkost najvacsiej matice
int connFlag = 0;
pid_t childPID;
timer_t vytvorCasovac(int);
void spustiCasovac(timer_t, int);

void sigusr1();
void sigusr2();

typedef struct matrix
{
  int rozmer;
  int mat[MATMAX][MATMAX];
}matrix;

void sigFlag(int signum)// ak skonci proces potomka odpocota sa premena podla ktorej rozhodujeme vypnutie servera
{
	wait(NULL);
	printf("[+]Client disconnected\n");
	connFlag --;

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

	// Looping for each element of the matrix 
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			//  Copying into temporary matrix only those element 
			//  which are not in given row and column 
			if (row != p && col != q)
			{
				temp[i][j++] = mat[row][col];

				// Row is filled, so increase row index and 
				// reset col index 
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
	int D = 0; // Initialize result 

	//  Base case : if matrix contains single element 
	if (n == 1)
		return mat[0][0];

	int **temp = create(n);// To store cofactors 

	int sign = 1;  // To store sign multiplier 

	 // Iterate for each element of first row 
	for (int f = 0; f < n; f++)
	{
		// Getting Cofactor of mat[0][f] 
		getCofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);

		// terms are to be added with alternate sign 
		sign = -sign;
	}
	for (int i = 0; i < n; i++)
		free(temp[i]);
	free(temp);
	//printf("\nVykonal som sa !!\n");
	return D;
}



int *Eq_solver(int **table1, int n, int ext[MATMAX])
{

	int **temp1 = create(n);
	//int ext[] = { 1 , 2, 3, 4, 5, };
	int *det = (int*)malloc(n * sizeof(int));
	//int det[100];
	det[n] = determinantOfMatrix(table1, n);
	if(det[n] == 0)
	{
		printf("Delenie nulou1");
		exit(1);
	}
	// determinant hlavnej matice pripoji na posledne misto pola
	

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
		//printf("\n Determinant cislo %d: %d\n", m, det[m]);//[DEBUG]
		//VypisDym(temp1, n);//[DEBUG]
		
/////////////////////////////////////////////////////
	}

	return det;

}



int read1(int matout[MATMAX][MATMAX], char subor[50],int *num ) // funkcia pre nacitanie matice zo suboru
{
	FILE *fr;
	//int **table;

	int sx = 0, sy = 0, min;
	//char subor[] = {"Vstup.txt"};
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("[-]Subor sa nepodarilo otvorit.\n");
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
	pommatrix = (matrix *) malloc(sizeof(matrix));
    ptrmatrix = (matrix *) malloc(sizeof(matrix));
    int matica[MATMAX][MATMAX]; 
    int shmid;
	key_t key;
	int readerr;
	int polek[1];
///////////////////////////////////////////////
			////pre determimant////
	int **maticaDym;
	int *det;
	double *roots;
	int end[1];
///////////////////////////////////////////////


	signal(SIGCHLD,sigFlag);// zachytava sa signal ktory posle potomok ked umiera

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
		//Casovac//
		timer_t casovac;
  		signal(SIGUSR1, sigusr1);
  		casovac=vytvorCasovac(SIGUSR1);
  		spustiCasovac(casovac,10);
		//Casovac//

		newSocket = accept(sockfd, (struct sockaddr*)&serverAddr, &addr_size);
		if(newSocket < 0)
		{
			exit(1);
		}
		printf("[+]Connection accepted\n");
		connFlag++;// pripaja sa

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
					txtname[i] = '\0';// bez pridania nuli to vypisovalo hovadiny
					printf("[DEBUG]File name: %s\n", txtname);

					
					
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
    			
    				polek[0] = key;
    				printf("[DEBUG]Key: %d\n", key);//*************************DEBUG*************************//
    				send(newSocket, polek, sizeof(polek), 0);
					printf("[DEBUG]Matrix in SHM:\n");
					Vypis(ptrmatrix->mat, ptrmatrix->rozmer); 

					/////// * Ratanie Determinantu * ///////
					maticaDym = create(ptrmatrix->rozmer);
					matfillDym(ptrmatrix->mat, maticaDym, ptrmatrix->rozmer);// naplnenie dynamickej matice [musi byt dynamicka]
					
					int mat_koef[MATMAX];
					recv(newSocket, mat_koef,sizeof(mat_koef), 0);
					//printf("\n toto su koeficienty %d %d %d", mat_koef[0], mat_koef[1], mat_koef[2]);//*************************DEBUG*************************//
					
					roots = roots_of_eq(Eq_solver(maticaDym, ptrmatrix->rozmer, mat_koef), ptrmatrix->rozmer);//vypocita korene rovnic
					//printf("determinanty:%f %f \n", roots[0], roots[1]);//*************************DEBUG*************************//
					
					
					send(newSocket, roots, sizeof(double)*ptrmatrix->rozmer, 0);// posle korene rovnic
					recv(newSocket, end,sizeof(end), 0);
					if(end[0] == 1)
					{
						printf("[+]Equations from: %s solved\n",txtname);

						//connFlag = 0;
					}
		
				}
			
		}
		

    }
    free(pommatrix);
    free(ptrmatrix);
	free(roots);
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
 	printf("\nTimer check\n");//[DEBUG]//
 	if (connFlag <= 0)
 	{
 		printf("Time out\n"); //[DEBUG]//
 		exit(0);
 	}

 } 
