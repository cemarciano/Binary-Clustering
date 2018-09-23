#ifndef SVMTRAINER_H
#define SVMTRAINER_H

#include <vector>
#include "Matrix.h"
#include "svm.h"

struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;

using namespace std;

class SVM_Trainer {
    public:
		// Constructor:
        SVM_Trainer(Matrix* matrixData, vector<int>* indexes, struct svm_parameter param);

		// Destructor:
        ~SVM_Trainer();
    protected:

		// Function to transform data into the format used by the SVM program:
		vector<vector<data_t>> generateData(Matrix* data, vector<int>* indexes);

		// Function to transform labels into the format used by the SVM program:
		vector<int> generateLabels(Matrix* data, vector<int>* indexes);

    private:

		int m_numRegisters; 	// Total number of registers
		int m_numDimensions;		// Total number of dimensions
};

#endif // SVMTRAINER_H
