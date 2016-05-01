/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int bh = 32;
	int bw = 8;
	int i, j, k, l;
//	int temp = 0;
	//	int dioganal = 0; 
	int currentRow, currentCol;
	if(M == 64 && N == 64)
	{
		//Iterate through the columns
		for (k = 0; k < N; k += 4) 
		{
			for (l = 0; l < N; l += 4) 
			{
				//Iterate through the rows      
				for (i = l; i < l + 4; i ++) 
				{
					for (j = k; j < k + 4; j ++) 
					{
						if (i != j) //If Not on the dioganal 
						{
							B[j][i] = A[i][j];
						}
						else { //If on the dioganal

							B[i][j] = A[i][j];
						}
					}

				}   

			}
		}

	}
	else
	{

		for (i = 0; i < N; i += bh)
		{
			currentRow = i+bh;
			if (currentRow > N) 
			{
				currentRow = N;
			}

			for (j = 0; j < M; j+= bw)
			{
				currentCol = j+bw;
				if (currentCol > M) 
				{ 
					currentCol = M;
				}

				for (k = i; k < currentRow; k++)
				{
					for (l = j; l < currentCol; l++)
					{
						if (k != l)
						{
							B[l][k] = A[k][l];
						}
					}

					if (j <= k && k < currentCol) 
					//if(k == l)
					{
						B[k][k] = A[k][k];
					}
				}
			}
		}
	} 
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{

	int bh = 32;
	int bw = 8;
	int i, j, k, l;
//	int temp = 0;
	//	int dioganal = 0; 
	int currentRow, currentCol;
	if(M == 64 && N == 64)
	{
		//Iterate through matrix using column-major iteration over blocks
		for (k = 0; k < N; k += 4) 
		{
			for (l = 0; l < N; l += 4) 
			{
				//Iterate over each row using row-major iteration           
				for (i = l; i < l + 4; i ++) 
				{
					for (j = k; j < k + 4; j ++) 
					{
						if (i != j) //If Not on th dioganal 
						{
							B[j][i] = A[i][j];
						}
						else {

							//temp = A[i][j];
							//dioganal = i;
							B[i][j] = A[i][j];
						}
					}

					//			if (l == k) 
					//			{
					//				B[dioganal][dioganal] = temp;
					//			}
				}   

			}
		}

	}
	else
	{

		for (i = 0; i < N; i += bh)
		{
			currentRow = i+bh;
			if (currentRow > N) 
			{
				currentRow = N;
			}

			for (j = 0; j < M; j+= bw)
			{
				currentCol = j+bw;
				if (currentCol > M) 
				{ 
					currentCol = M;
				}

				for (k = i; k < currentRow; k++)
				{
					for (l = j; l < currentCol; l++)
					{
						if (k != l)
						{
							B[l][k] = A[k][l];
						}
					}

					if (j <= k && k < currentCol) 
					//if(k == l)
					{
						B[k][k] = A[k][k];
					}
				}
			}
		}
	} 

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
	registerTransFunction(transpose_submit, transpose_submit_desc); 

	/* Register any additional transpose functions */
	registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
	int i, j;

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; ++j) {
			if (A[i][j] != B[j][i]) {
				return 0;
			}
		}
	}
	return 1;
}

