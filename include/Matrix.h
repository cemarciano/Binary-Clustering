#ifndef MATRIX_H
#define MATRIX_H

#include "global.h"

typedef double data_t;

class Matrix {
    public:
		// Constructor:
        Matrix(int dimRows, int dimColumns, bool columnsSeq=false);

		// Generates *n* elements, each containing *k* attributes:
		void generateRandom(bool parallel);

        // Fills matrix elements with file data:
        void readFromFile(const char* fileLocation);

        // Retrieves a value:
        data_t get(int i, int j);

        // Saves a value:
        data_t put(int i, int j, data_t value);

        // Saves a value:
        void print(int numRows);

		// Destructor:
        ~Matrix();


    protected:

		// Returns the boolean value of the i-th element:
        void fillLinesSerial();

		// Puts value in the i-th position:
        void fillLinesParallel(int threadId);

    private:
        data_t** matrix;
        int rows;
		int columns;
		bool inverted;
};

#endif // MATRIX_H
