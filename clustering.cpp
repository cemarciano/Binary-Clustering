#include <iostream>			// Formatted output
#include <random>			// For random data generation
#include <ctime>			// For stopwatch
#include <thread>			// To parallelize computation
#include "Bitmask.h"		// Array class for storing bits

#define N 6000000			// Size of data
#define K 20				// Dimension of data
#define CORES 4				// Number of CPUs

using namespace std;

typedef double data_t;


// Function declarations:
data_t** generateRandomMatrix();
void fillColumns(int threadId);


// Main program:
int main(){

	// Starts the stopwatch:
	clock_t begin = clock();

	// Loads the data matrix:
	data_t** data = generateRandomMatrix();

	cout << "(debug) Line 61:" << endl;
	for (int i = 0; i < K; i++){
		cout << data[61][i] << "|";
	}

	// Stops the stopwatch:
	clock_t end = clock();

	// Prints out elapsed time:
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	cout << endl << "Elapsed time: " << time_spent << " seconds." << endl;

}


// Generates *n* elements, each containing *k* attributes:
data_t** generateRandomMatrix(){

	// Allocates *n* rows for the data matrix:
	data_t** matrix = new data_t*[N];

	// Allocates *k* columns:
	for (int i = 0; i < N; i++){
		matrix[i] = new data_t[K];
	}

	// Initializes random seed:
	srand (time(NULL));


	// Array of threads:
	std::thread tasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		tasks[threadId] = thread(fillColumns, threadId, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		tasks[threadId].join();
	}

	// Returns filled structure:
	return matrix;

}

void fillColumns(int threadId, data_t** matrix){

	// Number of columns to fill:
	int each = K / CORES;

	// Loops through designated columns:
	for (int j = threadId*each; j < threadId*each + each; j++){
		// Randomizes interval of data for each column:
		int interval = rand() % 200 - 100;
		// Loops through lines:
		for (int i = 0; i < N; i++){
			// Fills in random data:
			matrix[i][j] = (rand() % interval) - (interval / 2);
		}
	}

}
