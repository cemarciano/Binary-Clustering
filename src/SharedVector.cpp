#include <SharedVector.h>
#include <iostream>

using namespace std;

template<class T>
SharedVector<T>::SharedVector(int numThreads){
	// Saves number of threads:
	m_numThreads = numThreads;
	// Sets size:
	m_size = 0;
	m_sizeHasChanged = false;
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
	while (index >= m_vector[vec].size()){
		// Keeps searching:
		index -= m_vector[vec].size();
		vec++;
	}
	// Returns the element from vector vec:
	return (m_vector[vec])[index];
}


// Adds value to the vector owned by thread threadId:
template<class T>
void SharedVector<T>::push(T value, int threadId){
	// Adds value to the corresponding vector:
	m_vector[threadId].push_back(value);
	// Signals that size has changed:
	m_sizeHasChanged = true;
}


// Returns total number of elements across all vectors:
template<class T>
int SharedVector<T>::getSize(){
	// Checks if size needs to be calculated:
	if (m_sizeHasChanged == true){
		// Accumulator variable:
		m_size = 0;
		// Sums all vectors:
		for (int i=0; i < m_vector.size(); i++){
			m_size += m_vector[i].size();
		}
	}
	// Sets size as calculated:
	m_sizeHasChanged = false;
	// Returns the size:
	return m_size;
}



// Destructor:
template<class T>
SharedVector<T>::~SharedVector(){
}


// Instantiates relevant template types:
template class SharedVector<int>;
template class SharedVector<float>;
template class SharedVector<double>;
