#include "Bitmask.h"
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

// Constructor:
Bitmask::Bitmask(int length, bool setTrue){
    // Saves total length of bitmask:
    bitmaskLength = length;
    // Allocates memory space:
    vArray = new bitset<B_SIZE>[(int)ceil(bitmaskLength/B_SIZE)+B_SIZE];
    // Fills in all elements with starting value:
    for (int i=1; i<=bitmaskLength; i++){
        this->put(i, setTrue);
    }
}


// Returns the boolean value of the i-th element:
bool Bitmask::get(int i){
    int byteNum = floor((float)(i-1.0)/B_SIZE);
    int bitNum = (i-1) % B_SIZE;
    return (bool) vArray[byteNum][bitNum];
}


// Puts value in the i-th position:
void Bitmask::put(int i, bool value){
	// Returns immediately in case position i is outside allocated boundaries:
    if ((i<1) || (i>bitmaskLength)){
        cout << "ERRO: Acesso a indice indefinido no Bitmask (indice " << i << ", fora dos limites 1 a " << bitmaskLength << ")" << endl;
        return;
    }
    // Puts value in the i-th position:
    int byteNum = floor((float)(i-1.0)/B_SIZE);
    int bitNum = (i-1) % B_SIZE;
    vArray[byteNum][bitNum] = value;

}



// Returns the number of TRUE elements in O(n):
int Bitmask::getSize(){
    // Accumulator variable:
    int acc = 0;
    // Loops through the bitmask:
    for (int i=1; i<=bitmaskLength; i++){
        if (this->get(i) == true){
            acc += 1;
        }
    }
    // Returns sum of true elements:
    return acc;
}



// Returns total allocated length:
int Bitmask::getLength(){
    return bitmaskLength;
}


// Prints out the value of each position in a formatted list:
void Bitmask::print(){
    cout << "Bitmask: [";
    for (int i=1; i<=bitmaskLength; i++){
        if (this->get(i) == true){
            cout << 1;
        } else {
            cout << 0;
        }
        if (i != bitmaskLength) cout << " ";
    }
    cout << "]" << endl;
}


// Transfers the value of setTrue to every position in the bitmask:
void Bitmask::reset(bool setTrue){
    if (setTrue == false){
        for (int i=1; i<=bitmaskLength; i++){
            this->put(i, false);
        }
    } else {
        for (int i=1; i<=bitmaskLength; i++){
            this->put(i, true);
        }
    }
}


// Checks if this bitmask and bitmask R have the same value for all elements:
bool Bitmask::equals(Bitmask* R){
    if (this->getLength() != R->getLength()){
        return false;
    } else if (this->getSize() != R->getSize()){
        return false;
    } else {
        for (int i=1; i<=bitmaskLength; i++){
            if (this->get(i) != R->get(i)){
                return false;
            }
        }
        return true;
    }
}


// Prints out the index of the elements containing values equal to trueOrFalse:
void Bitmask::printIDs(bool trueOrFalse){
	(trueOrFalse == true) ? cout << "True " : cout << "False ";
	cout << "IDs: [";
	for (int i=1; i<=bitmaskLength; i++){
		if ((trueOrFalse == true) && (this->get(i) == true)){
			cout << i << " ";
		} else if ((trueOrFalse == false) && (this->get(i) == false)){
			cout << i << " ";
		}
	}
	cout << "\b" << "]" << endl;
}

// Destructor:
Bitmask::~Bitmask(){
	// Deletes allocated bitmask array:
    delete[] vArray;
}
