#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
    public:
		// Constructor:
        Matrix(int dimRows, int dimColumns);

		// Generates *n* elements, each containing *k* attributes:
		generateRandom(bool parallel);

		// Destructor:
        ~Matrix();

		// Operator overload to implement matrix functionality:
		data_t* operator [](int i) const {return matrix[i];}
    	data_t* & operator [](int i) {return matrix[i];}

    protected:

		// Returns the boolean value of the i-th element:
        void fillLinesSerial();

		// Puts value in the i-th position:
        void fillLinesParallel(int threadId);

    private:
        data_t matrix
        int i;
		int j;
};

#endif // MATRIX_H
