#include <iostream>			// Formatted output
#include <random>			// For random data generation
#include <ctime>			// For stopwatch
#include <thread>			// To parallelize computation
#include "Bitmask.h"		// Array class for storing bits

#define N 6000000			// Size of data
#define K 20				// Dimension of data
#define CORES 19 			// Number of CPUs

using namespace std;

typedef double data_t;


// Function declarations:
data_t** generateRandomMatrix();
void fillLines(int threadId, data_t** matrix);


// Main program:
int main(){

	// Starts the stopwatch:
	struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

	// Loads the data matrix:
	data_t** data = generateRandomMatrix();

	cout << "(debug) Line 61:" << endl;
	for (int i = 0; i < K; i++){
		cout << data[61][i] << "|";
	}

	// Stops the stopwatch:
	clock_gettime(CLOCK_MONOTONIC, &finish);

	// Prints out elapsed time:
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << endl << "Elapsed time: " << elapsed << " seconds." << endl;

}


// Generates *n* elements, each containing *k* attributes:
data_t** generateRandomMatrix(){

	// Allocates *n* rows for the data matrix:
	data_t** matrix = new data_t*[N];

	// Allocates *k* columns:
	for (int i = 0; i < N; i++){
		matrix[i] = new data_t[K];
	}


	// Array of threads:
	std::thread tasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		tasks[threadId] = thread(fillLines, threadId, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		tasks[threadId].join();
	}

	// Returns filled structure:
	return matrix;

}

void fillLines(int threadId, data_t** matrix){

    // Randomizes interval of data for each thread:
    int interval = 15241*threadId % 200 - 100;

    // Linear congruent generator:
    int gen = (15245 + 12345) % interval;

	// Number of columns to fill:
	int each = N / CORES;

	// Loops through designated lines:
	for (int i = threadId*each; i < threadId*each + each; i++){
		// Loops through columns:
		for (int j = 0; j < K; j++){
			// Fills in random data:
            gen = (1664521*gen + 12341) % interval;
			matrix[i][j] = gen;//(rand() % interval) - (interval / 2);
		}
	}

}
