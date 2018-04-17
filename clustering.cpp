#include "global.h"         // Global settings
#include <iostream>			// Formatted output
#include <ctime>			// For stopwatch
#include <cmath>            // Math routines
#include <thread>			// To parallelize computation
#include "Bitmask.h"		// Array class for storing bits
#include "Matrix.h"			// Data matrix class

using namespace std;


int similar = 0;
int* powArr;

// Function declarations:
void binaryClustering(Matrix* matrix);
void findCentroids(int threadId, data_t* centroids, Matrix* matrix);
void removeSimilar(int threadId, data_t* centroids, Bitmask* bitmask, Matrix* matrix);


// Main program:
int main(){

	// Loads the data matrix:
	Matrix data(N, D, true);

	// Generates random data for matrix:
	data.generateRandom(false);

    // Starts the stopwatch:
	struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    cout << "Start!" << endl;

    // Runs binary clustering algorithm:
    binaryClustering(&data);

	// Stops the stopwatch:
	clock_gettime(CLOCK_MONOTONIC, &finish);

	// Prints out elapsed time:
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << endl << "Elapsed time: " << elapsed << " seconds." << endl;


}



void binaryClustering(Matrix* matrix){

    /****************************/
    /*** CENTROID CALCULATION ***/
    /****************************/

    // Allocates centroid array:
    data_t* centroids = new data_t[D]();

    // Array of threads:
	thread centroidTasks[CORES];

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
    for (int i = 0; i < D; i++){
        cout << centroids[i] << " - ";
    }

    /************************/
    /*** SIMILARITY CHECK ***/
    /************************/

    // Bitmask to hold unused similarity combinations:
    Bitmask bitmask(pow(2,D+1), false);

    // Array containing future indexes of bitmask:
    powArr = new int[D];
    // Fills the array:
    for (int i = 0; i < D; i++){
        powArr[i] = pow(2,i);
    }


    // Array of threads:
	std::thread similarityTasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		similarityTasks[threadId] = thread(removeSimilar, threadId, centroids, &bitmask, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		similarityTasks[threadId].join();
	}

    cout << endl << "Selected items: " << bitmask.getSize() << endl;
    cout << "Similar items: " << similar << endl;

}

void findCentroids(int threadId, data_t* centroids, Matrix* matrix){

    // Number of columns to sum:
	float each = D*1.0 / CORES;

    // Calculates chunck:
    int start = round(threadId*each);
    int end = round((threadId+1)*each);

    // Loops through designated columns:
	for (int j = start; j < end; j++){
        // Accumulator:
        data_t acc = 0;
        // Loops through lines:
		for (int i = 0; i < N; i++){
            acc += matrix->get(i, j);
        }
        // Writes accumulator mean:
        centroids[j] = acc / N;
    }

}


void removeSimilar(int threadId, data_t* centroids, Bitmask* bitmask, Matrix* matrix){

    // Number of lines to check:
	float each = N*1.0 / CORES;

    // Calculates chunck:
    int start = round(threadId*each);
    int end = round((threadId+1)*each);

    // Loops through designated lines:
	for (int i = start; i < end; i++){
        // Memory position accumulator:
        int memAcc = 1;
        // Loops through columns:
        for (int j = 0; j < D; j++){
            // Checks where data is positioned in regards to centroid:
            if (matrix->get(i, j) > centroids[j]){
                // Moves the memory position further (sets 1 in bitmask, represented in 10s):
                memAcc += powArr[j];
            }
        }
        // Checks if similar data has not yet been found:
        if (bitmask->get(memAcc) == false){
            // Marks data as found:
            bitmask->put(memAcc, true);
        } else {
            similar++;
        }
    }
}
