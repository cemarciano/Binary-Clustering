#include "global.h"         // Global settings
#include <iostream>			// Formatted output
#include <ctime>			// For stopwatch
#include <cmath>            // Math routines
#include <thread>			// To parallelize computation
#include <limits>           // To use infinity
#include "Bitmask.h"		// Array class for storing bits
#include "Matrix.h"			// Data matrix class
#include <unistd.h>

using namespace std;


int* powArr;

// Function declarations:
void binaryClustering(Matrix* matrix);
void findCentroids(int threadId, data_t* centroids, data_t* stdDev, Matrix* matrix);
void clusterSplitting(int threadId, Matrix* boundaries, Matrix* matrix);


// Main program:
int main(){

	// Loads the data matrix:
	Matrix data("/home/cadu/signal.txt", true);

    // Starts the stopwatch:
	struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    cout << "Start!" << endl;

    // Runs binary clustering algorithm:
    //binaryClustering(&data);

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
    data_t* centroids = new data_t[ matrix->getDims() ]();
	// Allocates stddev array:
    data_t* stdDev = new data_t[ matrix->getDims() ]();

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
    for (int i = 0; i < matrix->getDims(); i++){
        cout << centroids[i] << " - ";
    }
	cout << endl;
	// Prints stddev values:
    for (int i = 0; i < matrix->getDims(); i++){
        cout << stdDev[i] << " - ";
    }

	/***************************/
    /*** DIVISION BOUNDARIES ***/
    /***************************/

	// Creates matrix D-DIMENSIONS by K-DIVISIONS:
	Matrix boundaries(matrix->getDims(), K);

	// Step to divide boundaries with:
	float step;
	// Checks how to divide the space:
	if (K % 2 == 0){
		// Calculates initial step:
		step = -1*floor((K - 1)/2);
	} else {
		// Calculates initial step:
		step = -1*(1.0*(K - 2)/2);
	}
	// Runs through dimensions of matrix:
	for (int i = 0; i < matrix->getDims(); i++){
		// Runs through each division:
		for (int j = 0; j < K-1; j++){
			// Saves the value of the boundary:
			boundaries.put(i, j, (WARP*(step+j)*stdDev[i])+centroids[i]);
		}
        // Adds infinity as last boundary:
        boundaries.put(i, K-1, numeric_limits<data_t>::infinity());
	}



    /*************************/
    /*** CLUSTER SPLITTING ***/
    /*************************/


    // Array containing future indexes of clusters:
    powArr = new int[ matrix->getDims() ];
    // Fills the array with powers of the number of divisions:
    for (int i = 0; i < matrix->getDims(); i++){
        powArr[i] = pow(K,i);
    }


    // Array of threads:
	std::thread splittingTasks[CORES];

	// Loops through threads:
	for (int threadId = 0; threadId < CORES; threadId++){
		// Fires up thread to fill matrix:
		splittingTasks[threadId] = thread(clusterSplitting, threadId, &boundaries, matrix);
	}

	// Waits until all threads are done:
	for (int threadId = 0; threadId < CORES; threadId++){
		splittingTasks[threadId].join();
	}


}

void findCentroids(int threadId, data_t* centroids, data_t* stdDev, Matrix* matrix){

    // Number of columns to sum:
	float each = (matrix->getDims())*1.0 / CORES;

    // Calculates chunck:
    int start = round(threadId*each);
    int end = round((threadId+1)*each);

    // Loops through designated columns:
	for (int j = start; j < end; j++){

		// CENTROID:

        // Accumulator:
        data_t acc = 0;
        // Loops through lines:
		for (int i = 0; i < matrix->getRows(); i++){
            acc += matrix->get(i, j);
        }
        // Writes accumulator mean:
        centroids[j] = acc / matrix->getRows();

		// STDDEV:

		// Accumulator:
        acc = 0;
		// Current centroid:
		data_t currCentroid = centroids[j];
        // Loops through lines:
		for (int i = 0; i < matrix->getRows(); i++){
            acc += pow( matrix->get(i, j) - currCentroid, 2);
        }
        // Writes accumulator stddev:
        stdDev[j] = sqrt(acc/matrix->getRows());
    }

}




void clusterSplitting(int threadId, Matrix* boundaries, Matrix* matrix){

    // Number of lines to check:
	float each = (matrix->getRows())*1.0 / CORES;

    // Calculates chunck:
    int start = round(threadId*each);
    int end = round((threadId+1)*each);

    // Loops through designated lines:
	for (int i = start; i < end; i++){
        // Memory position accumulator:
        int memAcc = 1;
        // Loops through columns:
        for (int j = 0; j < matrix->getDims(); j++){
            // Loops through boundaries:
            for (int k = 0; k < K; k++){
                //if (threadId == 0) cout << "Got " << matrix->get(i, j) << " and comparing to boundary " << boundaries->get(j,k) << endl;
                // Checks if data is to the left of boundary:
                if (matrix->get(i, j) < boundaries->get(j,k)){
                    //if (threadId == 0) cout << "Comparison was successfull, adding "<< powArr[j] + k - 1 << endl;
                    // Moves the memory position further (sets 1 in bitmask, represented in 10s):
                    memAcc += powArr[j]*k;
                    break;
                }
            }
        }
        //cout << "memAcc = " << memAcc << endl;
        // Assigns a cluster to the data:
        matrix->putClusterOf(i, memAcc);
    }
}
