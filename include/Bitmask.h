#ifndef BITMASK_H
#define BITMASK_H
#include <bitset>

#define B_SIZE 8

class Bitmask {
    public:
		// Constructor:
        Bitmask(int length, bool setTrue=false);

		// Returns the boolean value of the i-th element:
        bool get(int i);

		// Puts value in the i-th position:
        void put(int i, bool value);

        // Returns the number of TRUE elements in O(n):
        int getSize();

		// Returns total allocated length:
        int getLength();

		// Prints out the value of each position in a formatted list:
        void print();

		// Transfers the value of setTrue to every position in the bitmask:
        void reset(bool setTrue=false);

		// Checks if this bitmask and bitmask R have the same value for all elements:
        bool equals(Bitmask* R);

		// Prints out the index of the elements containing values equal to trueOrFalse:
		void printIDs(bool trueOrFalse);

		// Destructor:
        ~Bitmask();
    protected:
    private:
        std::bitset<8>* vArray;
        int bitmaskLength;
};

#endif // BITMASK_H
