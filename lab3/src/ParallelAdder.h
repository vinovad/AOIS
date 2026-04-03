#ifndef PARALLEL_ADDER_H
#define PARALLEL_ADDER_H

#include "FullAdder.h"
#include "Constants.h"
#include <string>

namespace MyProject {
    class ParallelAdder {
    public:
        ParallelAdder();
        void processAddition(std::string binaryA, std::string binaryB, bool initialCarry);
        
        std::string getResultString() const;
        bool getFinalCarry() const;

    private:
        FullAdder adders[BITS_IN_TETRAD];
        bool resultBits[BITS_IN_TETRAD];
        bool finalCarry;
        bool charToBool(char bit) const;
    };
}

#endif