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



int *Eq_solver(int **table1, int n)
{

	/////////////////////
	int **temp1 = create(n);
	int ext[] = { 1 , 2, 3, 4, 5, };
	int *det = (int*)malloc(n * sizeof(int));
	//int det[100];
	det[n] = determinantOfMatrix(table1, n);
	//printf("Eq_solver %f\n", det[n]);
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
		printf("TOto %d\n", det[m]);
		Vypis(temp1, n);

	}
	
	/////////////////////
	return det;

}
double *roots_of_eq(int *determinants, int n)
{
	double pole[100];
	//double *pole = (double*)malloc(n * sizeof(double));
	printf("\n\t roots_of_determinant %d\n", determinants[n]);
	for (int i = 0; i < n; i++)
	{
		if (determinants[n] == 0)
			exit;
		pole[i] = ((double)determinants[i]) / ((double)determinants[n]);
		//printf("\n\t%f / %f = %f \n", determinants[i], determinants[n], pole[i]);

	}

	return pole;
}
int **create(int n)
{
	int **p, i;
	p = (int **)malloc(n * sizeof(int *));
	for (i = 0; i < n; i++)
		p[i] = (int *)malloc(n * sizeof(int));
	return p;
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
