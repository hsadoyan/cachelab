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
	// Block height and width //
	int block_height; 
	int block_width;

	// Loop iterators //
	int i, j, iblock, jblock; 

	// Stores the row and column we're working with //
	int currentRow, currentCol;

	// The 64x64 case //
	if(M == 64 && N == 64)
	{
		block_height = 4;  //16
		block_width = 4;  //4


	}
	// The 32x32 and irregular sized matrix case //
	else
	{
		block_height = 16;
		block_width = 8;
	}

	for (i = 0; i < N; i += block_height)
	{
		currentRow = i + block_height;

		if (currentRow > N) 
		{
			currentRow = N;
		}

		for (j = 0; j < M; j+= block_width)
		{
			currentCol = j+block_width;
		
			if (currentCol > M) 
			{ 
				currentCol = M;
			}
		
			for (iblock = i; iblock < currentRow; iblock++)
			{
				for (jblock = j; jblock < currentCol; jblock++)
				{
					if (iblock != jblock)
					{
						B[jblock][iblock] = A[iblock][jblock];
					}
				}
				if (j <= iblock && iblock < jblock) 
				{
					B[iblock][iblock] = A[iblock][iblock];
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
	// Block height and width //
	int block_height; 
	int block_width;

	// Loop iterators //
	int i, j, iblock, jblock; 

	// Stores the row and column we're working with //
	//int currentRow, 
	//int currentCol;

	int tempi = 0;
	int tempj = 0;

	

	// The 64x64 case //
	if(M == 64 && N == 64)
	{
		block_height = 4;  //16
		block_width = 4;  //4


	}
	// The 32x32 and irregular sized matrix case //
	else
	{
		block_height = 16;
		block_width = 8;
	}


	for (i = block_height; i < N + block_height; i += block_height)
	{
	//  currentRow = i;
		tempi = i - block_height;

		if (i > N) 
		{
			i = N;
		}

		for (j = block_width; j < M + block_width; j+= block_width)
		{
			tempj = j - block_width;
		
			if (j > M) 
			{ 
				j = M;
			}
		
			for (iblock = tempi; iblock < i; iblock++)
			{
				for (jblock = tempj; jblock < j; jblock++)
				{
					if (iblock != jblock)
					{
						B[jblock][iblock] = A[iblock][jblock];
					}
			
				}
				if (tempj <= iblock && iblock < jblock)
				//if(i == j) 
				{
					B[iblock][iblock] = A[iblock][iblock];
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

