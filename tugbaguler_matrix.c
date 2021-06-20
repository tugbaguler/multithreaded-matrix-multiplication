/*
Author: Tuğba Güler
Student Number: 20150807012
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define ELEMENT_SIZE 4 // the number of elements in the matrix size
#define MAX_STRING_LENGTH 100
#define N 4096

int matrix_size[] = {128, 256, 512, 1024, 2048, 4096};

int matrix_a[N][N];
int matrix_b[N][N];
int result_matrix[N][N];
//transpose matrix of matrix matrix_b
int transpose_matrix[N][N];

// This is the type of value returned by the clock function. Values of type clock_t are in units of clock ticks.
clock_t serial_clock[ELEMENT_SIZE];
clock_t caching_clock[ELEMENT_SIZE];
clock_t decompositional_clock[ELEMENT_SIZE][5]; // thread counts --> 1, 2, 4, 8, 16
clock_t loading_clock[ELEMENT_SIZE];

pthread_t threads[16];
int size;
int number_of_threads;

void GenerateMatrixAndSave()
{
	int m, i, j;
	char FileName[MAX_STRING_LENGTH];
	FILE *fp_matrix_a;
	FILE *fp_matrix_b;

	for (m = 0; m < ELEMENT_SIZE; m++)
	{
		// Creating the matrix
		for (i = 0; i < matrix_size[m]; i++)
		{
			for (j = 0; j < matrix_size[m]; j++)
			{
				matrix_a[i][j] = rand() % 10;
				matrix_b[i][j] = rand() % 10;
			}
		}

		// Saving the matrix
		sprintf(FileName, "a%d.txt", matrix_size[m]);
		fp_matrix_a = fopen(FileName, "w");

		sprintf(FileName, "b%d.txt", matrix_size[m]);
		fp_matrix_b = fopen(FileName, "w");

		for (i = 0; i < matrix_size[m]; i++)
		{
			for (j = 0; j < matrix_size[m]; j++)
			{
				if (j == matrix_size[m] - 1)
				{
					fprintf(fp_matrix_a, "%d\n", matrix_a[i][j]);
					fprintf(fp_matrix_b, "%d\n", matrix_b[i][j]);
				}
				else
				{
					fprintf(fp_matrix_a, "%d, ", matrix_a[i][j]);
					fprintf(fp_matrix_b, "%d, ", matrix_b[i][j]);
				}
			}
		}

		fclose(fp_matrix_a);
		fclose(fp_matrix_b);
	}
}

void LoadMatrixFromFile(int m)
{
	char FileName[MAX_STRING_LENGTH];
	FILE *fp_matrix_a = NULL;
	FILE *fp_matrix_b = NULL;
	int i, j;

	//loading time
	loading_clock[m] = clock();

	//load
	sprintf(FileName, "a%d.txt", matrix_size[m]);
	fp_matrix_a = fopen(FileName, "r");

	sprintf(FileName, "b%d.txt", matrix_size[m]);
	fp_matrix_b = fopen(FileName, "r");

	for (i = 0; i < matrix_size[m]; i++)
	{
		for (j = 0; j < matrix_size[m]; j++)
		{
			if (j == matrix_size[m] - 1)
			{
				fscanf(fp_matrix_a, "%d\n", &matrix_a[i][j]);
				fscanf(fp_matrix_b, "%d\n", &matrix_b[i][j]);
			}
			else
			{
				fscanf(fp_matrix_a, "%d, ", &matrix_a[i][j]);
				fscanf(fp_matrix_b, "%d, ", &matrix_b[i][j]);
			}
		}
	}

	fclose(fp_matrix_a);
	fclose(fp_matrix_b);

	loading_clock[m] = clock() - loading_clock[m];
}

void ComputingResultBySerialMatrixMultiplication(int m, int serial_save)
{
	// A and B nXn matrices --> C = A X B (A is matrix_a, B is matrix_b, C is result matrix)
	// This algorithm requires n^3 multiplications and n^3 additions.
	// Time complexity is O(n^3)
	int i, j, k;
	char FileName[MAX_STRING_LENGTH];
	FILE *fp_matrix_result = NULL;

	serial_clock[m] = clock();
	// traditional matrix multiplication (i-j-k)
	for (i = 0; i < matrix_size[m]; i++)
    {
		for (j = 0; j < matrix_size[m]; j++)
        {
			for (k = 0; k < matrix_size[m]; k++)
            {
				result_matrix[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

	serial_clock[m] = clock() - serial_clock[m];

	//save result
	if (serial_save)
	{
		sprintf(FileName, "result%d.txt", matrix_size[m]);
		fp_matrix_result = fopen(FileName, "w");

		for (i = 0; i < matrix_size[m]; i++)
		{
			for (j = 0; j < matrix_size[m]; j++)
			{
				if (j == matrix_size[m] - 1)
					fprintf(fp_matrix_result, "%d\n", result_matrix[i][j]);
				else
					fprintf(fp_matrix_result, "%d, ", result_matrix[i][j]);
			}
		}

		fclose(fp_matrix_result);
	}
}

void ComputingResultUsingByCaching(int m)
{
	int i, j, k;
	caching_clock[m] = clock();
	for (i = 0; i < matrix_size[m]; i++)
    {
		for (j = 0; j < matrix_size[m]; j++)
        {
			transpose_matrix[i][j] = matrix_b[j][i];
        }
    }

	//computing cache only matrix multiplication
	for (i = 0; i < matrix_size[m]; i++)
	{
		for (j = 0; j < matrix_size[m]; j++)
		{
			result_matrix[i][j] = 0;
			for (k = 0; k < matrix_size[m]; k++)
			{
				result_matrix[i][j] += matrix_a[i][k] * transpose_matrix[j][k];
				
			}
		}
	}
	caching_clock[m] = clock() - caching_clock[m];
	
	/*
	// i-k-j
	for (int i = 0; i < matrix_size[m]; i++)
	{
		for (int k = 0; k <  matrix_size[m]; k++)
		{
			matrix_a[i][k];
			for (int j = 0; j <  matrix_size[m]; j++)
			{
				result_matrix[i][j] += matrix_a[i][k] * matrix_b[k][j];
			}
		}
	}
	caching_clock[m] = clock() - caching_clock[m];
	*/

}

// Data Decomposition in Parallel Programming
void *decomposition_worker(void *arg)
{
	int i, j, k;
	int tid, portion_size;
	int row_start, row_end;

	tid = *(int *)(arg); // get the thread ID assigned sequentially.
	portion_size = size / number_of_threads;
	row_start = tid * portion_size;
	row_end = (tid + 1) * portion_size;

	for (i = row_start; i < row_end; i++) // hold row index of matrix_a
	{
		for (j = 0; j < size; j++) // hold column index of matrix_b
		{
			result_matrix[i][j] = 0; // hold value of a cell
			// Sum the products of the row vector and the corresponding cells in the column vector
			for (k = 0; k < size; k++)
			{
				result_matrix[i][j] += matrix_a[i][k] * transpose_matrix[j][k];
				
			}
		}
	}
}

void ComputingResultUsingByCachingAndDecomposition(int m, int thread_count)
{
	int i, j, k;
	size = matrix_size[m];
	// The number of threads should be tested up to 1,2,4,8,16, 
	// and these numbers are the powers of 2 to the power of 0,1,2,3,4.
	number_of_threads = (int)pow(2.0, (double)thread_count);
	decompositional_clock[m][thread_count] = clock();

	for (i = 0; i < number_of_threads; i++)
	{
		int *tid;
		tid = (int *)malloc(sizeof(int));
		*tid = i;
		pthread_create(&threads[i], NULL, decomposition_worker, (void *)tid);
	}

	for (i = 0; i < number_of_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	decompositional_clock[m][thread_count] = clock() - decompositional_clock[m][thread_count];
}

int main(int m, int thread_count)
{
	int i, j;
	clock_t tstart, tend;
	char sztmp[MAX_STRING_LENGTH];

	printf("\nThe process is running. It may take some time to work. Please wait...\n");

	//generate matrix and save into txt files
	GenerateMatrixAndSave();

	//loading and computing
	for (i = 0; i < ELEMENT_SIZE; i++)
	{
		//load matrix form file
		LoadMatrixFromFile(i);

		//compute matrix multiplication
		ComputingResultBySerialMatrixMultiplication(i, 1);
		ComputingResultUsingByCaching(i);

		for (j = 0; j < 5; j++)
			ComputingResultUsingByCachingAndDecomposition(i, j);

	}

	//output of all result
	printf("\nComputing...\n");
	for (i = 0; i < ELEMENT_SIZE; i++)
	{
		sprintf(sztmp, "%d X %d Matrix : \n", matrix_size[i], matrix_size[i]);
		printf("%s", sztmp);

		sprintf(sztmp, "\tLoading matrix time : %fs\n", (float)loading_clock[i] / 1000000.0);
		printf("%s", sztmp);

		sprintf(sztmp, "\tSerial Computing time : %fs\n", (float)serial_clock[i] / 1000000.0);
		printf("%s", sztmp);

		sprintf(sztmp, "\tCaching Computing time : %fs\n", (float)caching_clock[i] / 1000000.0);
		printf("%s", sztmp);

		for (j = 0; j < 5; j++)
		{
			sprintf(sztmp, "\tCaching & Decomposition Computing time (%d threads) : %fs\n", (int)pow(2.0, (double)j), (float)decompositional_clock[i][j] / 1000000.0);
			printf("%s", sztmp);
		}

	}
	printf("\nComputing is completed.\n");

	return 0;
}