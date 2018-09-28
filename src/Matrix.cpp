#include <iostream>			// For basic input and output
#include <fstream>
#include <cmath>            // To calculate pow in space allocation
#include <thread>			// To parallelize computation
#include <random>			// For random data generation
#include <sstream>
#include <iomanip>			// For printing tables
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

	// Allocates storage space:
	this->allocateSpace(true);


	///////////////////////
	/// READS ALL LINES ///
	///////////////////////

    // Number of lines read:
    int lineCounter = 0;
	// Loop to read the entire input file:
    while (getline(myFile, s)) {
        // Helper variable to hold value of elements:
        float value;
		if ((s.empty() == false) && (s[0] != '#')){
            // Increases count of lines:
            lineCounter++;
			// Checks if second class was reached:
			if (lineCounter > m_signalSize){
				// Sets bitmask to second class (1):
				m_class->put(lineCounter, true);
			}
			istringstream tmp(s);
            // Loops through elements of line:
            for (int j = 0; j < m_columns; j++){
                tmp >> value;
                this->put(lineCounter-1, j, value);
            }
		}
    }


}


// Constructor, takes a N x D parameter and allocates space:
Matrix::Matrix(int rows, int columns, bool columnsSeq){

	// Sets up meta data:
	m_rows = rows;
	m_columns = columns;
	m_inverted = columnsSeq;

	// Allocates storage space:
	this->allocateSpace(false);

}


// Uses previously set information to allocate storage space. If extraArrays is true, also
// allocates space for m_class, m_cluster, m_signalDist, m_backgroundDist:
void Matrix::allocateSpace(bool extraArrays){
	// Checks if columns should be stored sequentially:
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

	// If extra arrays (described in method description) should be allocated, do it:
	if (extraArrays == true){
		// Allocates space for bitmask of classes:
		m_class = new Bitmask(m_rows);
		// Allocates space for cluster array:
		m_cluster = new int[m_rows]();
		// Calculates total number of clusters:
		int totalClusters = pow(K, m_columns);
		// Allocates space for signal distribution:
		m_signalDist = new int[totalClusters]();
		// Allocates space for background distribution:
		m_backgroundDist = new int[totalClusters]();
		// Allocates space for bitmask of contamination:
		m_contamination = new Bitmask(totalClusters);
		// Allocates space for bitmask to check if cluster has both classes:
		m_hasBothClasses = new Bitmask(totalClusters);
		// Signals that this matrix has extra information:
		m_extraArrays = true;
	} else {
		// Signals that this matrix doesn't have extra information:
		m_extraArrays = false;
	}

}


// Returns a value in the matrix:
data_t Matrix::get(int i, int j){
	if (m_inverted){
    	return m_matrix[j][i];
	} else {
		return m_matrix[i][j];
	}
}


// Saves a value in the matrix:
data_t Matrix::put(int i, int j, data_t value){
	if (m_inverted){
    	m_matrix[j][i] = value;
	} else {
		m_matrix[i][j] = value;
	}
}


// Retrieves number of rows:
int Matrix::getRows(){
	return m_rows;
}


// Retrieves number of columns:
int Matrix::getDims(){
	return m_columns;
}


// Retrieves class of register:
int Matrix::getClassOf(int i){
	if (m_class->get(i+1) == true){
		return 1;
	} else {
		return 0;
	}
}

// Retrieves cluster of register:
int Matrix::getClusterOf(int i){
	return m_cluster[i];
}



// Saves cluster of register:
void Matrix::putClusterOf(int i, int cluster){
	// Sets cluster of register:
	m_cluster[i] = cluster;
	// Increments the number of registers a cluster has:
	if (getClassOf(i) == 0){
		m_signalDist[cluster]++;
	} else {
		m_backgroundDist[cluster]++;
	}
}

// Signals that cluster has size signal registers:
void Matrix::putSignalDist(int cluster, int size){
	m_signalDist[cluster] = size;
}

// Signals that cluster has size background registers:
void Matrix::putBackgroundDist(int cluster, int size){
	m_backgroundDist[cluster] = size;
}

// Retrieves the number of signal registers a cluster has divided by m_signalSize:
int Matrix::getSignalDist(int cluster){
	return m_signalDist[cluster];
}


// Retrieves the number of background registers a cluster has divided by m_backgroundSize:
int Matrix::getBackgroundDist(int cluster){
	return m_backgroundDist[cluster];
}


// Retrieves total number of signal registers:
int Matrix::getSignalSize(){
	return m_signalSize;
}


// Retrieves total number of background registers:
int Matrix::getBackgroundSize(){
	return m_backgroundSize;
}


// Sets cluster as contamined by class classBool (i.e. more elements of classBool exist in this cluster):
void Matrix::putContamination(int cluster, bool classBool){
	m_contamination->put(cluster+1, classBool);
}


// Retrieves the class that contamines a cluster:
bool Matrix::getContamination(int cluster){
	return m_contamination->get(cluster+1);
}

// Sets cluster as having at least one element of each class:
void Matrix::putHasBothClasses(int cluster, bool situation){
	m_hasBothClasses->put(cluster+1, situation);
}


// Retrieves if cluster has at least one register of each class:
bool Matrix::getHasBothClasses(int cluster){
	return m_hasBothClasses->get(cluster+1);
}


// Prints all rows from [startRow, endRow)
void Matrix::print(int startRow, int endRow){
	// Treats invalid startRow values:
	if (startRow < 0){
		startRow = 0;
	}
	// Treats invalid endRow values:
	if (endRow > m_rows){
		endRow = m_rows;
	}
    cout << endl << "Printing rows " << startRow << " to " << endRow-1 << ":" << endl;
    // Loops through lines:
    for (int i = startRow; i < endRow; i++){
        cout << "[";
        // Loops through columns:
        for (int j = 0; j < m_columns; j++){
            // Prints data:
            cout << setw(10) << this->get(i, j);
            if (j != m_columns-1){
                 cout << '\t';
            }
        }
        cout << "]";
		// Only prints extra array info if they have been previously allocated:
		if (m_extraArrays == true) {
			cout << " ---- Class " << this->getClassOf(i) << ", Cluster " << this->getClusterOf(i);
		}
		cout << endl;
    }
    cout << endl;
}


void Matrix::saveClusterDist(){
	// Array of register distribution (distArr[i] returns how many clusters exist with i registers):
	int* distArr = new int[m_numRegisters]();
	// Variable to hold maximum registers in a cluster:
	int maxRegInCluster = 0;
	// Retrieves maximum number of registers in a cluster:
	for (int i = 0; i < totalClusters; i++){
		// Retrieves the registers in this cluster:
		int totalDist = this->getSignalDist(i) + this->getBackgroundDist(i);
		// Saves distribution data:
		distArr[totalDist-1]++;
		// Checks if this cluster holds more register than the current known:
		if ( totalDist > maxRegInCluster){
			// Updates current known number of registers in a single cluster:
			maxRegInCluster = totalDist;
		}
	}
	// Opens file:
	ofstream myFile;
    myFile.open("/home/cemarciano/Documents/clusterDist.txt");
	// Writes to file the distribution of registers in clusters.
	// E.g.: 1 300 means that 300 clusters have a single register in them.
	for (int i = 1; i <= maxRegInCluster; i++){
		myFile << i << " " << distArr[i-1] << endl;
	}
    myFile.close();
}

// Destructor:
Matrix::~Matrix(){

	// Deletes every columns:
	for (int j = 0; j < m_columns; j++){
		delete[] m_matrix[j];
	}

    delete[] m_matrix;

}
