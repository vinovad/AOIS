#include "boolean_diff.h"

namespace BoolLogic {

    BooleanDiff::BooleanDiff()
        : rowCount(0), variableCount(0) {
        for (int index = 0; index < MAX_TRUTH_TABLE_ROWS; index++) {
            outputValues[index] = false;
        }
    }

    void BooleanDiff::build(const TruthTable& table) {
        rowCount = table.getRowCount();
        variableCount = table.getVariableCount();
        table.getVariables(variables);

        for (int index = 0; index < rowCount; index++) {
            outputValues[index] = table.getOutputValue(index);
        }
    }

    void BooleanDiff::computePartial( const bool sourceValues[MAX_TRUTH_TABLE_ROWS],
        bool resultValues[MAX_TRUTH_TABLE_ROWS],
        int varIndex
    ) const {
        int bitPosition = variableCount - 1 - varIndex;

        for (int row = 0; row < rowCount; row++) {
            int rowWithZero = row & ~(1 << bitPosition);
            int rowWithOne  = row |  (1 << bitPosition);
            resultValues[row] = sourceValues[rowWithZero] ^ sourceValues[rowWithOne];
        }
    }

    void BooleanDiff::printDerivativeVector( const bool derivValues[MAX_TRUTH_TABLE_ROWS] ) const {
        std::cout << "(";
        for (int row = 0; row < rowCount; row++) {
            std::cout << derivValues[row];
        }
        std::cout << ")";
    }

    void BooleanDiff::printDerivativeLabel( const int varIndices[], int count ) const {
        std::cout << "df/d(";
        for (int index = 0; index < count; index++) {
            if (index > 0) std::cout << ",";
            std::cout << variables[varIndices[index]];
        }
        std::cout << ") = ";
    }

    void BooleanDiff::printPartialAll() const {
        std::cout << "\n=== Частные производные ===\n";

        for (int varIndex = 0; varIndex < variableCount; varIndex++) {
            bool derivValues[MAX_TRUTH_TABLE_ROWS];
            computePartial(outputValues, derivValues, varIndex);

            int singleVar[1] = {varIndex};
            printDerivativeLabel(singleVar, 1);
            printDerivativeVector(derivValues);
            std::cout << "\n";
        }
    }

    void BooleanDiff::printMixed(
        const int varIndices[],
        int count
    ) const {
        bool currentValues[MAX_TRUTH_TABLE_ROWS];
        bool nextValues[MAX_TRUTH_TABLE_ROWS];

        for (int index = 0; index < rowCount; index++) {
            currentValues[index] = outputValues[index];
        }

        for (int step = 0; step < count; step++) {
            computePartial(currentValues, nextValues, varIndices[step]);
            for (int index = 0; index < rowCount; index++) {
                currentValues[index] = nextValues[index];
            }
        }

        printDerivativeLabel(varIndices, count);
        printDerivativeVector(currentValues);
        std::cout << "\n";
    }

    void BooleanDiff::generateMixedCombinations(
        int startIndex,
        int currentCombo[MAX_DERIVATIVE_VARS],
        int currentSize,
        int maxSize
    ) const {
        if (currentSize >= 2) {
            printMixed(currentCombo, currentSize);
        }
        if (currentSize == maxSize) {
            return;
        }
        for (int index = startIndex; index < variableCount; index++) {
            currentCombo[currentSize] = index;
            generateMixedCombinations(index + 1, currentCombo, currentSize + 1, maxSize);
        }
    }

    void BooleanDiff::printAllMixed() const {
        std::cout << "\n=== Смешанные производные ===\n";

        int maxOrder = variableCount < MAX_DERIVATIVE_VARS  ? variableCount : MAX_DERIVATIVE_VARS;
        int combo[MAX_DERIVATIVE_VARS];
        generateMixedCombinations(0, combo, 0, maxOrder);
    }

    void BooleanDiff::getPartialDerivative(
        int  varIndex,
        bool result[MAX_TRUTH_TABLE_ROWS]
    ) const {
        computePartial(outputValues, result, varIndex);
    }

}
