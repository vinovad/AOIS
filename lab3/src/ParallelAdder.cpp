#include "ParallelAdder.h"

namespace MyProject {
    ParallelAdder::ParallelAdder() : finalCarry(false) {
        for (int i = 0; i < BITS_IN_TETRAD; ++i) {
            resultBits[i] = false;
        }
    }

    bool ParallelAdder::charToBool(char bit) const {
        return bit == '1';
    }

    void ParallelAdder::processAddition(std::string binaryA, std::string binaryB, bool initialCarry) {
        bool currentCarry = initialCarry;
        for (int i = BITS_IN_TETRAD - 1; i >= 0; --i) {
            adders[i].calculate(charToBool(binaryA[i]), charToBool(binaryB[i]), currentCarry);
            resultBits[i] = adders[i].getSum();
            currentCarry = adders[i].getCarryOut();
        }
        finalCarry = currentCarry;
    }

    std::string ParallelAdder::getResultString() const {
        std::string res = "";
        for (int i = 0; i < BITS_IN_TETRAD; ++i) {
            res += resultBits[i] ? "1" : "0";
        }
        return res;
    }

    bool ParallelAdder::getFinalCarry() const { return finalCarry; }
}