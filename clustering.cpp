#include <iostream>			// Formatted output
#include <random>			// For random data generation
#include <ctime>			// For stopwatch
#include <thread>			// To parallelize computation
#include "Bitmask.h"		// Array class for storing bits

#define N 6000000			// Size of data
#define K 20				// Dimension of data
#define CORES 4 			// Number of CPUs

using namespace std;

typedef double data_t;


// Function declarations:
data_t** generateRandomMatrix();
void fillLines(int threadId, data_t** matrix);
void binaryClustering(data_t** matrix);
void findCentroids(int threadId, data_t* centroids, data_t** matrix);


// Main program:
int main(){

	// Loads the data matrix:
	data_t** data = generateRandomMatrix();

    // Starts the stopwatch:
	struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    cout << "Start!" << endl;
    // Runs binary clustering algorithm:
    binaryClustering(data);

	// Stops the stopwatch:
	clock_gettime(CLOCK_MONOTONIC, &finish);

	// Prints out elapsed time:
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << endl << "Elapsed time: " << elapsed << " seconds." << endl;

}


// Generates *n* elements, each containing *k* attributes:
data_t** generateRandomMatrix(){

	// Allocates *k* columns for the data matrix:
	data_t** matrix = new data_t*[K];

	// Allocates *n* rows:
	for (int i = 0; i < K; i++){
		matrix[i] = new data_t[N];
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

// Dependency function executed by threads to fill a random data matrix:
void fillLines(int threadId, data_t** matrix){

    // Randomizes interval of data for each thread:
    int interval = 15241*threadId % 200 - 100;

    // Linear congruent generator:
    int gen = (15245 + 12345) % interval;

	// Number of columns to fill:
	int each = K / CORES;

	// Loops through designated columns:
	for (int j = threadId*each; j < threadId*each + each; j++){
		// Loops through lines:
		for (int i = 0; i < N; i++){
			// Fills in random data:
            gen = (1664521*gen + 12341) % interval;
			matrix[j][i] = gen;
		}
	}

}

void binaryClustering(data_t** matrix){

    /****************************/
    /*** CENTROID CALCULATION ***/
    /****************************/

    // Allocates centroid array:
    data_t* centroids = new data_t[K]();

    // Array of threads:
	std::thread centroidTasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		centroidTasks[threadId] = thread(findCentroids, threadId, centroids, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		centroidTasks[threadId].join();
	}

    // Prints centroid values:
    for (int i = 0; i < K; i++){
        cout << centroids[i] << " - ";
    }

}

void findCentroids(int threadId, data_t* centroids, data_t** matrix){

    // Number of columns to sum:
	int each = K / CORES;

    // Loops through designated columns:
	for (int j = threadId*each; j < threadId*each + each; j++){
        // Accumulator:
        data_t acc = 0;
        // Loops through lines:
		for (int i = 0; i < N; i++){
            acc += matrix[j][i];
        }
        // Write accumulator mean:
        centroids[j] = acc / N;
    }

}
