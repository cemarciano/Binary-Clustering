#include <SharedVector.h>

using namespace std;

template<class T>
SharedVector<T>::SharedVector(int numThreads){
	// Saves number of threads:
	m_numThreads = numThreads;
	// Loops through number of threads:
	for (int i=0; i < m_numThreads; i++){
		// Creates a new vector for this thread:
		vector<T> threadVec;
		// Pushes this vector to the shared vector:
		m_vector.push_back(threadVec);
	}
}



// Retrieves the index-th value as if the vectors were a continous space:
template <class T>
T SharedVector<T>::get(int index){
	// Checks which vector this index belongs to.
	// Checks if subtracting vector size from index makes it negative:
	int vec = 0;
	while ((index - m_vector[vec].size()) > 0){
		// Keeps searching:
		index -= m_vector[vec].size();
		vec++;
	}
	// Returns the element from vector vec:
	return (m_vector[vec])[index-1];
}


// Adds value to the vector owned by thread threadId:
template<class T>
void SharedVector<T>::push(T value, int threadId){
	// Adds value to the corresponding vector:
	m_vector[threadId].push_back(value);
}


// Returns total number of elements across all vectors:
template<class T>
int SharedVector<T>::getSize(){
	// Accumulator variable:
	int acc = 0;
	// Sums all vectors:
	for (int i=0; i < m_vector.size(); i++){
		acc += m_vector[i].size();
	}
	// Returns total sum:
	return acc;
}



// Destructor:
template<class T>
SharedVector<T>::~SharedVector(){
}


// Instantiates relevant template types:
template class SharedVector<int>;
template class SharedVector<float>;
template class SharedVector<double>;
