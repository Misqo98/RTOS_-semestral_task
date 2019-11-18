#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

#define MATMAX 200
int **create1(int n)
{
	int **p, i;
	p = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		p[i] = (int *)malloc(n * sizeof(int));
	return p;
}

int **read1(char subor[50],int *num ) 
{
	FILE *fr;
	int **table;
	int sx = 0, sy = 0, min;
	//char subor[] = {"Vstup.txt"};
	if ((fr = fopen(subor, "r")) == NULL) {
		printf("Subor sa nepodarilo otvorit.\n");
		return 0;
	}

	fscanf(fr, "%d\n", &*(num));
	table = create1(*(num));


	for (int x = 0; x < (*num); x++)

	{

		for (int y = 0; y < (*num); y++)

		{
			fscanf(fr, "%d", &table[x][y]);
		}

	}

	return table;

}

void Vypis(int **pole, int n) {
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
 int **mat;
}matrix;


int main()
{
    char subor[] = {"Vstup.txt"};
    matrix *ptrmatrix, *pommat;
    int shmid;
    key_t key;
    int **matica; 
    ptrmatrix = (matrix *) malloc(sizeof(matrix));
    pommat = (matrix *) malloc(sizeof(matrix));

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 9804;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, sizeof(matrix),0666)) < 0) {
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
       pommat = ptrmatrix;
     //pommat = ptrmatrix;
	printf("\nV zdielanej pamati je toto:%d\n", pommat->mat[0][0]);
	//Vypis(pommat->mat, pommat->rozmer); 
    

   
    exit(0);
    
}
