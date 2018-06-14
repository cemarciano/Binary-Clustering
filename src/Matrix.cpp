#include <iostream>
#include <fstream>
#include <cmath>            // Math routines
#include <thread>			// To parallelize computation
#include <random>			// For random data generation
#include <sstream>
#include "Matrix.h"

using namespace std;

// Allocates space for matrix:
Matrix::Matrix(int dimRows, int dimColumns, bool columnsSeq){


	// Saves dimensions:
	rows = dimRows;
	columns = dimColumns;

	// Saves matrix layout:
	inverted = columnsSeq;

	if (inverted){

		// Allocates *k* columns for the data matrix:
		matrix = new data_t*[columns];

		// Allocates *n* rows:
		for (int j = 0; j < columns; j++){
			matrix[j] = new data_t[rows];
		}

	} else {

		// Allocates *n* rows for the data matrix:
		matrix = new data_t*[rows];

		// Allocates *k* columns:
		for (int j = 0; j < rows; j++){
			matrix[j] = new data_t[columns];
        }
	}
}


// Returns a value:
data_t Matrix::get(int i, int j){
	if (inverted){
    	return matrix[j][i];
	} else {
		return matrix[i][j];
	}
}


// Saves a value:
data_t Matrix::put(int i, int j, data_t value){
	if (inverted){
    	matrix[j][i] = value;
	} else {
		matrix[i][j] = value;
	}
}


// Fills matrix elements with file data:
void Matrix::readFromFile(const char* fileLocation){
    ifstream myFile;
    // Opens communication with input file:
    myFile.open(fileLocation);
    cout << endl << "Reading file " << fileLocation << "... Please stand by." << endl;
    if (!myFile){
        // If file was not found:
        cout << "File " << fileLocation << " not found." << endl;
        return;
    }
    // Loop to read the entire input file:
	string s;
    // Number of lines read:
    int lineCounter = 0;
    while (getline(myFile, s)) {
        // Helper variable to hold value of elements:
        float value;
		if (s.empty() == false){
            // Increases count of lines:
            lineCounter++;
			istringstream tmp(s);
            // Loops through elements of line:
            for (int j=0; j<D; j++){
                tmp >> value;
                put(lineCounter-1, j, value);
            }
		}
    }
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
			this->put(i, j, dice_roller());
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
			this->put(i, j, rand() % interval);
		}
	}

}

void Matrix::print(int numRows){
    cout << endl;
    // Loops through lines:
    for (int i = 0; i < numRows; i++){
        cout << "[";
        // Loops through columns:
        for (int j = 0; j < columns; j++){
            // Prints data:
            cout << this->get(i, j);
            if (j != columns-1){
                 cout << '\t';
            }
        }
        cout << "]" << endl;
    }
    cout << endl;
}

Matrix::~Matrix(){

	// Deletes every columns:
	for (int j = 0; j < columns; j++){
		delete[] matrix[j];
	}

    delete[] matrix;

}
