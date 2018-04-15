
// Allocates space for matrix:
void Matrix::Matrix(int dimRows, int dimColumns){

	// Saves dimensions:
	rows = dimRows;
	columns = dimColumns;

	// Allocates *k* columns for the data matrix:
	data_t** matrix = new data_t*[columns];

	// Allocates *n* rows:
	for (int j = 0; j < columns; j++){
		matrix[j] = new data_t[rows];
	}
}



// Generates *n* elements, each containing *k* attributes:
void Matrix::generateRandom(bool parallel){

    if (parallel){
    	// Array of threads:
    	thread tasks[CORES];

    	// Loops through threads:
    	for (int threadId = 0; threadId < CORES; threadId++){
    		// Fires up thread to fill matrix:
    		tasks[threadId] = thread(fillLinesParallel, threadId, matrix);
    	}

    	// Waits until all threads are done:
    	for (int threadId = 0; threadId < CORES; threadId++){
    		tasks[threadId].join();
    	}
    } else {
        fillLinesSerial(matrix);
    }

}


// Dependency function executed by threads to fill a random data matrix:
void Matrix::fillLinesParallel(int threadId){

    // Randomizes interval of data for each thread:
    int interval = (threadId+1)*2633 % 200;

	// Number of columns to fill:
	int each = K / CORES;

    // RNG init:
	uniform_int_distribution<int> dice_distribution(interval, 2*interval);
	mt19937 random_number_engine; // pseudorandom number generator
	auto dice_roller = bind(dice_distribution, random_number_engine);

	// Loops through designated columns:
	for (int j = threadId*each; j < threadId*each + each; j++){
		// Loops through lines:
		for (int i = 0; i < rows; i++){
			// Fills in random data:
			matrix[j][i] = dice_roller();
		}
	}

}

// Dependency function executed by threads to fill a random data matrix:
void Matrix::fillLinesSerial(){

    // Interval of data:
    int interval = 500;

    // Seed:
    srand(time(NULL));

	// Loops through columns:
	for (int j = 0; j < columns; j++){
		// Loops through rows:
		for (int i = 0; i < rows; i++){
			// Fills in random data:
			matrix[j][i] = rand() % interval;
		}
	}

}

void Matrix::~Matrix(){

	// Deletes every columns:
	for (int j = 0; j < columns; j++){
		delete[] matrix[j];
	}

	// Deletes main array:
	delete[] matrix;
}
