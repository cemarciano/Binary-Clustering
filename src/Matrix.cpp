#include <iostream>
#include <fstream>
#include <cmath>            // Math routines
#include <thread>			// To parallelize computation
#include <random>			// For random data generation
#include "Matrix.h"

using namespace std;

// Allocates space for matrix:
Matrix::Matrix(int dimRows, int dimColumns){

	// Saves dimensions:
	rows = dimRows;
	columns = dimColumns;

	// Allocates *k* columns for the data matrix:
	matrix = new data_t*[columns];

	// Allocates *n* rows:
	for (int j = 0; j < columns; j++){
		matrix[j] = new data_t[rows];
	}
}


// Returns a value:
data_t Matrix::get(int i, int j){
    return matrix[j][i];
}


// Saves a value:
data_t Matrix::put(data_t value, int i, int j){
    matrix[j][i] = value;
}



// Generates *n* elements, each containing *k* attributes:
void Matrix::generateRandom(bool parallel){

    if (parallel){
    	// Array of threads:
    	thread tasks[CORES];

    	// Loops through threads:
    	for (int threadId = 0; threadId < CORES; threadId++){
    		// Fires up thread to fill matrix:
    		tasks[threadId] = thread(&Matrix::fillLinesParallel, this, threadId);
    	}

    	// Waits until all threads are done:
    	for (int threadId = 0; threadId < CORES; threadId++){
    		tasks[threadId].join();
    	}
    } else {
        this->fillLinesSerial();
    }

}


// Dependency function executed by threads to fill a random data matrix:
void Matrix::fillLinesParallel(int threadId){

    // Randomizes interval of data for each thread:
    int interval = (threadId+1)*2633 % 200;

	// Number of columns to fill:
	int each = columns / CORES;

    // RNG init:
	uniform_int_distribution<int> dice_distribution(interval, 2*interval);
	mt19937 random_number_engine; // pseudorandom number generator
	auto dice_roller = bind(dice_distribution, random_number_engine);

	// Loops through designated columns:
	for (int j = threadId*each; j < threadId*each + each; j++){
		// Loops through lines:
		for (int i = 0; i < rows; i++){
			// Fills in random data:
			this->put(dice_roller(), i, j);
		}
	}

}

// Dependency function executed by threads to fill a random data matrix:
void Matrix::fillLinesSerial(){

    // Interval of data:
    int interval = 500;

    // Seed:
    srand(time(NULL));

	// Loops through columns:
	for (int j = 0; j < columns; j++){
		// Loops through rows:
		for (int i = 0; i < rows; i++){
			// Fills in random data:
			this->put(rand() % interval, i, j);
		}
	}

}

Matrix::~Matrix(){

	// Deletes every columns:
	for (int j = 0; j < columns; j++){
		delete[] matrix[j];
	}

    delete[] matrix;

}
