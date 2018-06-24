#include <iostream>
#include <fstream>
#include <cmath>            // Math routines
#include <thread>			// To parallelize computation
#include <random>			// For random data generation
#include <sstream>
#include "Matrix.h"

using namespace std;

// Allocates space for matrix:
Matrix::Matrix(const char* fileLocation, bool columnsSeq){

	// File object:
	ifstream myFile;
    // Opens communication with input file:
    myFile.open(fileLocation);
    cout << endl << "Reading file " << fileLocation << "... Please stand by." << endl;
    if (!myFile){
        // If file was not found:
        cout << "File " << fileLocation << " not found." << endl;
        return;
    }


	////////////////////////////////////
	/// RETRIEVES METADATA FROM FILE ///
	////////////////////////////////////

	string s;
	// Retrieves signal size:
	getline(myFile, s);
	istringstream tmpSignal(s);
	tmpSignal >> m_signalSize;
	// Retrieves background size:
	getline(myFile, s);
	istringstream tmpBackground(s);
	tmpBackground >> m_backgroundSize;
	// Retrieves number of dimenions:
	getline(myFile, s);
	istringstream tmpDim(s);
	tmpDim >> m_columns;
	// Calculates total rows:
	m_rows = m_signalSize + m_backgroundSize;


	///////////////////////
	/// ALLOCATES SPACE ///
	///////////////////////

	// Saves matrix layout:
	m_inverted = columnsSeq;

	if (m_inverted){
		// Allocates *k* columns for the data matrix:
		m_matrix = new data_t*[m_columns];
		// Allocates *n* rows:
		for (int j = 0; j < m_columns; j++){
			m_matrix[j] = new data_t[m_rows];
		}
	} else {
		// Allocates *n* rows for the data matrix:
		m_matrix = new data_t*[m_rows];
		// Allocates *k* columns:
		for (int j = 0; j < m_rows; j++){
			m_matrix[j] = new data_t[m_columns];
        }
	}

	// Allocates space for bitmask of classes:
	m_class = new Bitmask(m_rows);

	// Allocates space for cluster array:
	m_cluster = new int[m_rows]();


	///////////////////////
	/// READS ALL LINES ///
	///////////////////////

    // Number of lines read:
    int lineCounter = 0;
	// Loop to read the entire input file:
    while (getline(myFile, s)) {
        // Helper variable to hold value of elements:
        float value;
		if ((s.empty() == false) && (s.front() != "#")){
            // Increases count of lines:
            lineCounter++;
			// Checks if second class was reached:
			if (lineCounter > m_signalSize){
				// Sets bitmask to second class (1):
				m_class->put(lineCounter, true);
			}
			istringstream tmp(s);
            // Loops through elements of line:
            for (int j = 1; j <= m_columns; j++){
                tmp >> value;
                put(lineCounter, j, value);
            }
		}
    }


}


// Returns a value in the matrix:
data_t Matrix::get(int i, int j){
	if (m_inverted){
    	return m_matrix[j-1][i-1];
	} else {
		return m_matrix[i-1][j-1];
	}
}


// Saves a value in the matrix:
data_t Matrix::put(int i, int j, data_t value){
	if (m_inverted){
    	m_matrix[j-1][i-1] = value;
	} else {
		m_matrix[i-1][j-1] = value;
	}
}


// Retrieves class of register:
int Matrix::getClassOf(int i){
	if (m_class->get(i) == true){
		return 1;
	} else {
		return 0;
	}
}

// Retrieves cluster of register:
int Matrix::getClusterOf(int i){
	return m_cluster[i-1];
}


// Saves cluster of register:
void Matrix::putClusterOf(int i, int cluster){
	m_cluster[i-1] = cluster;
}



// Prints all rows from [startRow, endRow)
void Matrix::print(int startRow, int endRow){
	// Treats invalid startRow values:
	if (startRow < 1){
		startRow = 1;
	}
	// Treats invalid endRow values:
	if (endRow > m_rows+1){
		endRow = m_rows+1;
	}
    cout << endl;
    // Loops through lines:
    for (int i = startRow; i < endRow; i++){
        cout << "[";
        // Loops through columns:
        for (int j = 0; j < m_columns; j++){
            // Prints data:
            cout << this->get(i, j);
            if (j != m_columns-1){
                 cout << '\t';
            }
        }
        cout << "] ---- Class: " << this->classOf(i) << endl;
    }
    cout << endl;
}

// Destructor:
Matrix::~Matrix(){

	// Deletes every columns:
	for (int j = 0; j < m_columns; j++){
		delete[] m_matrix[j];
	}

    delete[] m_matrix;

}
