#ifndef SHAREDVECTOR_H
#define SHAREDVECTOR_H

#include <vector>
#include <global.h>


template <class T> class SVM_Trainer {
    public:
		// Constructor:
        SharedVector();

		// Retrieves the index-th value as if the vectors were a continous space:
		T get(int index);

		// Adds value to the vector owned by thread threadId:
		void push(T value, int threadId);

		// Returns total number of elements across all vectors:
		int getSize();

		// Destructor:
        ~SharedVector();
    protected:

    private:

		vector<vector<T>>m_vector;	// Internal shared vector
		int m_numThreads;				// Total number of threads
};

#endif // SHAREDVECTOR_H
