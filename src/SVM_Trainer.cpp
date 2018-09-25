#include <SVM_Trainer.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace std;



SVM_Trainer::SVM_Trainer(Matrix* matrixData, SharedVector<int>* indexes, struct svm_parameter param){

	m_indexes = indexes;
	m_numRegisters = m_indexes->getSize(); //number of lines with labels
	m_numDimensions = matrixData->getDims(); //number of features for each data vector

	// Organizes data so this SVM library will accept it:
	vector<vector<data_t>> data = this->generateData(matrixData);
	vector<int> labels = this->generateLabels(matrixData);

	//initialize the size of the m_problem with just an int
	m_prob.l = m_numRegisters;
	//here we need to give some memory to our structures
	// @param m_prob.l = number of labels
	// @param m_numDimensions = number of features for each label
	m_prob.y = Malloc(double,m_prob.l); //space for m_prob.l doubles
	m_prob.x = Malloc(struct svm_node *, m_prob.l); //space for m_prob.l pointers to struct svm_node
	m_Xspace = Malloc(struct svm_node, (m_numDimensions+1) * m_prob.l); //memory for pairs of index/value

	//here we are going to initialize it all a bit


	//initialize the different lables with an array of labels
	for (int i=0; i < m_prob.l; ++i) {
		m_prob.y[i] = labels[i];
	}
	//initialize the svm_node vector with input data array as follows:
	int j=0; //counter to traverse m_Xspace[i];
	for (int i=0;i < m_prob.l; ++i) {
		//set i-th element of m_prob.x to the address of m_Xspace[j].
		//m_numDimensions from m_Xspace[j] to m_Xspace[j+data[i].size] get filled right after next line
		m_prob.x[i] = &m_Xspace[j];
		for (int k=0; k<data[i].size(); ++k, ++j) {
			m_Xspace[j].index=k+1; //index of value
			m_Xspace[j].value=data[i][k]; //value
		}
		m_Xspace[j].index=-1;//state the end of data vector
		m_Xspace[j].value=0;

		j++;

	}


	//try to actually execute it
	m_model = svm_train(&m_prob, &param);

	//print support vectors:
	/*cout << "SV m_indexes:" << endl;
	for (int i=0; i<29; i++){
		cout << model->sv_indices[i] << endl;
	}*/

}


// Function to transform data into the format used by the SVM program:
vector<vector<data_t>> SVM_Trainer::generateData(Matrix* data) {
	// Vector to hold organized data:
	vector<vector<data_t>> vecData;
	// Loops through all assigned registers:
	for (int i=0; i<m_numRegisters; ++i) {
		// Retrieves the index of the register:
		int index = m_indexes->get(i);
		// Vector to hold attributes:
		vector<double> featureSet;
		// Loops through all dimensions:
		for (int j=0; j<m_numDimensions; ++j) {
			// Retrieves attribute from the data matrix:
			data_t attribute = data->get(index, j);
			// Adds attribute to the vector representing the register:
			featureSet.push_back(attribute);
		}
		// Adds register to data vector:
		vecData.push_back(featureSet);
	}
	// Returns the data vector:
	return vecData;
}


// Function to transform labels into the format used by the SVM program:
vector<int> SVM_Trainer::generateLabels(Matrix* data){
	// Vector to hold labels:
	vector<int> labelsVec;
	// Loops through assigned registers:
	for (int i=0; i < m_numRegisters; ++i) {
		// Retrieves label (and adds 1 since SVM goes [1,inf) ):
		int label = data->getClassOf(m_indexes->get(i)) + 1;
		// Adds label to labels vector:
		labelsVec.push_back(label);
	}
	// Return labels vector:
	return labelsVec;
}


// Returns the total number of support vectors:
int SVM_Trainer::getTotalSV(){
	// Sums support vectors of both classes:
	return m_model->l;
}

// Returns an array of indices corresponding to support vectors:
int SVM_Trainer::getSV(int i){
	// Returns the index in the data matrix corresponding to the i-th SV:
	return m_indexes->get(m_model->sv_indices[i]);
}


// Destructor:
SVM_Trainer::~SVM_Trainer(){
}
