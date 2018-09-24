#ifndef MATRIX_H
#define MATRIX_H

#include "global.h"			// General configuration file
#include "Bitmask.h"		// Array class for storing bits

typedef double data_t;

class Matrix {
    public:
		// Constructor, requires a fileLocation (path to file where data is stored in text format).
		// Variable columnsSeq, if set, inverts storage format for columns and rows:
        Matrix(const char* fileLocation, bool columnsSeq=false);

		// Constructor, takes a N x D parameter and allocates space:
		Matrix(int rows, int columns, bool columnsSeq=false);

        // Retrieves a value in the matrix:
        data_t get(int i, int j);

        // Saves a value in the matrix:
        data_t put(int i, int j, data_t value);

		// Retrieves number of rows:
		int getRows();

		// Retrieves number of columns:
		int getDims();

		// Retrieves class of data:
		int getClassOf(int i);

		// Retrieves cluster of data:
		int getClusterOf(int i);

		// Saves cluster of data:
		void putClusterOf(int i, int cluster);

		// Signals that cluster has size signal registers:
		void putSignalDist(int cluster, int size);

		// Signals that cluster has size background registers:
		void putBackgroundDist(int cluster, int size);

		// Retrieves the number of signal registers a cluster has:
		int getSignalDist(int cluster);

		// Retrieves the number of background registers a cluster has:
		int getBackgroundDist(int cluster);

		// Retrieves total number of signal registers:
		int getSignalSize();

		// Retrieves total number of background registers:
		int getBackgroundSize();

		// Sets cluster as contamined by class classBool (i.e. more elements of classBool exist in this cluster):
		void putContamination(int cluster, bool classBool);

		// Retrieves the class that contamines a cluster:
		bool getContamination(int cluster);

		// Sets cluster as having at least one element of each class:
		void putHasBothClasses(int cluster, bool situation);

		// Retrieves if cluster has at least one register of each class:
		bool getHasBothClasses(int cluster);

        // Prints all rows from [startRow, endRow):
        void print(int startRow, int endRow);

		// Destructor:
        ~Matrix();

	protected:

		// Uses previously set information to allocate storage space. If extraArrays is true, also
		// allocates space for m_class, m_cluster, m_signalDist, m_backgroundDist:
		void allocateSpace(bool extraArrays);

    private:
        data_t** m_matrix;				// Matrix to hold registers
		Bitmask* m_class;				// m_class of i returns the class register i-1 belongs to
		Bitmask* m_contamination;		// m_contamination of i returns the class that contamines cluster i the most
		Bitmask* m_hasBothClasses;		// m_hasBothClasses of i returns true if cluster i contains at least one register of each class
		int* m_cluster;					// m_cluster of i returns the cluster register i belongs to
		int* m_signalDist;				// m_signalDist of i returns the number of signal registers present in cluster i / m_signalSize
		int* m_backgroundDist;			// m_backgroundDist of i returns the number of background registers present in cluster i / m_backgroundSize
        int m_rows;						// Total number of registers
		int m_columns;					// Total dimensions
		int m_signalSize;				// Total elements of class 0
		int m_backgroundSize;			// Total elements of class 1
		bool m_inverted;				// If set, columns will be stored sequentially
		bool m_extraArrays;				// Signals whether this matrix holds space for the extra arrays (cluster, contamination, etc)
};

#endif // MATRIX_H
