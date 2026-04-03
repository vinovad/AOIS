#include "FullAdder.h"

namespace MyProject {
    FullAdder::FullAdder() : sum(false), carryOut(false) {}

    void FullAdder::calculate(bool bitA, bool bitB, bool carryIn) {
        sum = (bitA ^ bitB) ^ carryIn;
        carryOut = (bitA & bitB) | (carryIn & (bitA ^ bitB));
    }

    bool FullAdder::getSum() const { return sum; }
    bool FullAdder::getCarryOut() const { return carryOut; }
}