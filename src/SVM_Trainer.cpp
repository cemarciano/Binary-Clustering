#include <SVM_Trainer.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace std;

struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;

SVM_Trainer::SVM_Trainer(Matrix* matrixData, SharedVector<int>* indexes, struct svm_parameter param){

	m_numRegisters = indexes->getSize(); //number of lines with labels
	m_numDimensions = matrixData->getDims(); //number of features for each data vector
	cout << "Dealing with organization" << endl;
	vector<vector<data_t>> data = this->generateData(matrixData, indexes);
	vector<int> labels = this->generateLabels(matrixData, indexes);
	cout << "Finished dealing with organization" << endl;
	//initialize the size of the problem with just an int
	prob.l = m_numRegisters;
	//here we need to give some memory to our structures
	// @param prob.l = number of labels
	// @param m_numDimensions = number of features for each label
	prob.y = Malloc(double,prob.l); //space for prob.l doubles
	prob.x = Malloc(struct svm_node *, prob.l); //space for prob.l pointers to struct svm_node
	x_space = Malloc(struct svm_node, (m_numDimensions+1) * prob.l); //memory for pairs of index/value

	//here we are going to initialize it all a bit


	//initialize the different lables with an array of labels
	for (int i=0; i < prob.l; ++i) {
		prob.y[i] = labels[i];
		cout<<"prob.y["<<i<<"] = "<<prob.y[i]<<endl;
	}
	//initialize the svm_node vector with input data array as follows:
	int j=0; //counter to traverse x_space[i];
	for (int i=0;i < prob.l; ++i) {
		//set i-th element of prob.x to the address of x_space[j].
		//m_numDimensions from x_space[j] to x_space[j+data[i].size] get filled right after next line
		prob.x[i] = &x_space[j];
		for (int k=0; k<data[i].size(); ++k, ++j) {
			x_space[j].index=k+1; //index of value
			x_space[j].value=data[i][k]; //value
		}
		x_space[j].index=-1;//state the end of data vector
		x_space[j].value=0;

		j++;

	}


	//try to actually execute it
	model = svm_train(&prob, &param);

	//print support vectors:
	/*cout << "SV indexes:" << endl;
	for (int i=0; i<29; i++){
		cout << model->sv_indices[i] << endl;
	}*/

}


// Function to transform data into the format used by the SVM program:
vector<vector<data_t>> SVM_Trainer::generateData(Matrix* data, SharedVector<int>* indexes) {
	cout << "Generating " << m_numRegisters << "data for SVM..." << endl;
	// Vector to hold organized data:
	vector<vector<data_t>> vecData;
	// Loops through all assigned registers:
	for (int i=0; i<m_numRegisters; ++i) {
		// Retrieves the index of the register:
		int index = indexes->get(i);
		cout << "Found index " << index << endl;
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
vector<int> SVM_Trainer::generateLabels(Matrix* data, SharedVector<int>* indexes){
	// Vector to hold labels:
	vector<int> labelsVec;
	cout << "Generating " << m_numRegisters << " labels" << endl;
	// Loops through assigned registers:
	for (int i=0; i < m_numRegisters; ++i) {
		// Retrieves label (and adds 1 since SVM goes [1,inf) ):
		int label = data->getClassOf(indexes->get(i)) + 1;
		// Adds label to labels vector:
		labelsVec.push_back(label);
	}
	// Return labels vector:
	return labelsVec;
}


// Destructor:
SVM_Trainer::~SVM_Trainer(){
}
