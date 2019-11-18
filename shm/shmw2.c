#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
/*
*** ************ shmw2 [staticka] ********************************* ***
*** Toto je verzia so statickou velkostou matice					***
*** netreba create1 matica ma velkost MATMAX x MATMAX				***
*** treba osetrit aby nebralo matice vacise ako staticky rozmer		***
*** POZOR vsetko je staticke										***
*/
#define MATMAX 200 // velkost najvacsiej matice

int **create1(int n)
{
	int **p, i;
	p = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		p[i] = (int *)malloc(n * sizeof(int));
	return p;
}

int read1(int matout[MATMAX][MATMAX], char subor[50],int *num ) // funkcia pre nacitanie matice zo suboru
{
	FILE *fr;
	//int **table;
	int sx = 0, sy = 0, min;
	//char subor[] = {"Vstup.txt"};
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit.\n");
		return 0;
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
typedef struct matrix
{
 int rozmer;
 int mat[MATMAX][MATMAX];
}matrix;


int main()
{
    char subor[] = {"Vstup.txt"};
    matrix *ptrmatrix;
    int shmid;
    key_t key;
    int matica[MATMAX][MATMAX]; 
    matrix *pommat;
    pommat = (matrix *) malloc(sizeof(matrix));
    ptrmatrix = (matrix *) malloc(sizeof(matrix));

	

    


    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 9804;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((ptrmatrix = (matrix*)shmat(shmid, (void*)0, 0)) == (matrix *) -1) {
        perror("shmat");
        exit(1);
    }
  
    read1(matica, subor, &(ptrmatrix->rozmer));
    matfill(matica, ptrmatrix->mat, ptrmatrix->rozmer);
	printf("[+]Do pamate sa zapisalo :\n");
	Vypis(ptrmatrix->mat, ptrmatrix->rozmer); 
    
    while(1)
    {
    sleep(1);
    }
    

   
    exit(0);
    
}
