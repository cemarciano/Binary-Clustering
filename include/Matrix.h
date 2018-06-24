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

		// Retrieves the number of signal registers a cluster has:
		int getSignalDist(int cluster);

		// Retrieves the number of background registers a cluster has:
		int getBackgroundDist(int cluster);

        // Prints all rows from [startRow, endRow):
        void print(int startRow, int endRow);

		// Destructor:
        ~Matrix();

	protected:

		// Uses previously set information to allocate storage space. If extraArrays is true, also
		// allocates space for m_class, m_cluster, m_signalDist, m_backgroundDist:
		void allocateSpace(bool extraArrays);

    private:
        data_t** m_matrix;			// Matrix to hold registers
		Bitmask* m_class;			// m_class of i returns the class register i-1 belongs to
		int* m_cluster;				// m_cluster of i returns the cluster register i belongs to
		int* m_signalDist		// m_signalDist of i returns the number of signal registers present in cluster i
		int* m_backgroundDist	// m_backgroundDist of i returns the number of signal registers present in cluster i
        int m_rows;					// Total number of registers
		int m_columns;				// Total dimensions
		int m_signalSize;			// Total elements of class 0
		int m_backgroundSize;		// Total elements of class 1
		bool m_inverted;			// If set, columns will be stored sequentially
};

#endif // MATRIX_H
