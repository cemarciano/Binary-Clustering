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
void findCentroids(int threadId, data_t* centroids, data_t* stdDev, Matrix* matrix);
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
	// Allocates stddev array:
    data_t* stdDev = new data_t[D]();

    // Array of threads:
	thread centroidTasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		centroidTasks[threadId] = thread(findCentroids, threadId, centroids, stdDev, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		centroidTasks[threadId].join();
	}

    // Prints centroid values:
    for (int i = 0; i < D; i++){
        cout << centroids[i] << " - ";
    }
	cout << endl;
	// Prints stddev values:
    for (int i = 0; i < D; i++){
        cout << stdDev[i] << " - ";
    }

	/***************************/
    /*** DIVISION BOUNDARIES ***/
    /***************************/

	// Creates matrix D-DIMENSIONS by K-DIVISIONS:
	Matrix boundaries(D, K-1);

	// Step to divide boundaries with:
	float step;
	// Checks how to divide the space:
	if (K % 2 == 0){
		// Calculates initial step:
		step = -1*floor((K - 1)/2);
        cout << "Step: -" << step << "- ";
	} else {
		// Calculates initial step:
		step = -1*(1.0*(K - 2)/2);
	}
	// Runs through dimensions of matrix:
	for (int i = 0; i < D; i++){
		// Runs through each division:
		for (int j = 0; j < K-1; j++){
			// Saves the value of the boundary:
			boundaries.put(i, j, (step+j)*stdDev[i]);
            cout << endl << "Putting " << (step+j)*stdDev[i];
		}
	}

	cout << endl;
	// Prints boundaries:
	boundaries.print(D);

    /************************/
    /*** SIMILARITY CHECK ***/
    /************************/

    // Bitmask to hold unused similarity combinations (number of divisions ^ total dimensions):
    Bitmask bitmask(pow(K,D+1), false);

    // Array containing future indexes of bitmask:
    powArr = new int[D];
    // Fills the array with powers of the number of divisions:
    for (int i = 0; i < D; i++){
        powArr[i] = pow(K,i);
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

void findCentroids(int threadId, data_t* centroids, data_t* stdDev, Matrix* matrix){

    // Number of columns to sum:
	float each = D*1.0 / CORES;

    // Calculates chunck:
    int start = round(threadId*each);
    int end = round((threadId+1)*each);

    // Loops through designated columns:
	for (int j = start; j < end; j++){

		// CENTROID:

        // Accumulator:
        data_t acc = 0;
        // Loops through lines:
		for (int i = 0; i < N; i++){
            acc += matrix->get(i, j);
        }
        // Writes accumulator mean:
        centroids[j] = acc / N;

		// STDDEV:

		// Accumulator:
        acc = 0;
		// Current centroid:
		data_t currCentroid = centroids[j];
        // Loops through lines:
		for (int i = 0; i < N; i++){
            acc += pow( matrix->get(i, j) - currCentroid, 2);
        }
        // Writes accumulator stddev:
        stdDev[j] = sqrt(acc/N);
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
