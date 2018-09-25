#ifndef SVMTRAINER_H
#define SVMTRAINER_H

#include <vector>
#include "Matrix.h"
#include "svm.h"
#include "SharedVector.h"


using namespace std;

class SVM_Trainer {
    public:
		// Constructor:
        SVM_Trainer(Matrix* matrixData, SharedVector<int>* indexes, struct svm_parameter param);

		// Returns the total number of support vectors:
		int getTotalSV();

		// Returns the i-th support vector:
		int getSV(int i);

		// Destructor:
        ~SVM_Trainer();
    protected:

		// Function to transform data into the format used by the SVM program:
		vector<vector<data_t>> generateData(Matrix* data, SharedVector<int>* indexes);

		// Function to transform labels into the format used by the SVM program:
		vector<int> generateLabels(Matrix* data, SharedVector<int>* indexes);

    private:

		int m_numRegisters; 			// Total number of registers
		int m_numDimensions;			// Total number of dimensions
		SharedVector<int>* m_indexes;	// Real indices of data, in relation to data matrix
		struct svm_problem m_prob;
		struct svm_model* m_model;
		struct svm_node* m_Xspace;
};

#endif // SVMTRAINER_H
