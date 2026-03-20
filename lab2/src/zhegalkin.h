#pragma once
#include <iostream>
#include "truth_table.h"

namespace BoolLogic {

    const int MAX_ZHEGALKIN_LENGTH = 512;

    class Zhegalkin {
    private:
        bool coefficients[MAX_TRUTH_TABLE_ROWS];
        char polynomial[MAX_ZHEGALKIN_LENGTH];
        int  rowCount;
        int  variableCount;

        void computeCoefficients(const TruthTable& table);
        void buildPolynomialString(const TruthTable& table);

        void appendTerm(
            const TruthTable& table,
            int termIndex
        );

        void appendString(char* destination, const char* source) const;
        int  stringLength(const char* str) const;
    
    public:
        Zhegalkin();

        void build(const TruthTable& table);
        void print() const;
        const char* getPolynomial() const;

    };

}