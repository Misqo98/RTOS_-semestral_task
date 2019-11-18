#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

/*
*** ************ shmr2 [staticka] ********************************* ***
*** Toto je verzia so statickou velkostou matice					***
*** netreba create1 matica ma velkost MATMAX x MATMAX				***
*** treba osetrit aby nebralo matice vacise ako staticky rozmer		***
*** POZOR vsetko je staticke										***
*/

#define MATMAX 200



void Vypis(int pole[MATMAX][MATMAX], int n) {
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
      int n = ptrmatrix->rozmer;
      
	
	printf("\nV zdielanej pamati je toto:%d(romer matice)\n", ptrmatrix->rozmer);
	Vypis(pommat->mat, ptrmatrix->rozmer); 
	
    

   
    exit(0);
    
}
