#ifndef FULL_ADDER_H
#define FULL_ADDER_H

namespace MyProject {
    class FullAdder {
    private:
        bool sum;
        bool carryOut;
    public:
        FullAdder();
        void calculate(bool bitA, bool bitB, bool carryIn);
        bool getSum() const;
        bool getCarryOut() const;

    };
}

#endif