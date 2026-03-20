#pragma once
#include <iostream>
#include "truth_table.h"

namespace BoolLogic {
    const int MAX_DERIVATIVE_VARS = 4;
    class BooleanDiff {
    private:
        bool outputValues[MAX_TRUTH_TABLE_ROWS];
        char variables[MAX_VARIABLES];
        int  rowCount;
        int  variableCount;

        void computePartial(
            const bool sourceValues[MAX_TRUTH_TABLE_ROWS],
            bool resultValues[MAX_TRUTH_TABLE_ROWS],
            int varIndex) const;

        void printDerivativeVector( const bool derivValues[MAX_TRUTH_TABLE_ROWS] ) const;
        void printDerivativeLabel( const int varIndices[], int count) const;
        void generateMixedCombinations(
            int startIndex,
            int currentCombo[MAX_DERIVATIVE_VARS],
            int currentSize,
            int maxSize
        ) const;
    
    public:
        BooleanDiff();
        void build(const TruthTable& table);
        void printPartialAll() const;
        void printMixed(const int varIndices[], int count) const;
        void printAllMixed() const;
        void getPartialDerivative(int varIndex, bool result[MAX_TRUTH_TABLE_ROWS]) const;

    
    };

}