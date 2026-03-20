#include "truth_table.h"

namespace BoolLogic {

    const int BINARY_BASE = 2;

    TruthTable::TruthTable()
        : variableCount(0), rowCount(0) {
    }

    void TruthTable::build(const char* formula) {
        AstNode* root   = parser.parse(formula);
        variableCount   = parser.getVariableCount();
        parser.getVariables(variables);

        rowCount = 1;
        for (int index = 0; index < variableCount; index++) {
            rowCount *= BINARY_BASE;
        }

        for (int row = 0; row < rowCount; row++) {
            fillRow(row, root);
        }
    }

    void TruthTable::fillRow(int row, AstNode* root) {
        for (int varIndex = 0; varIndex < variableCount; varIndex++) {
            int bitPosition = variableCount - 1 - varIndex;
            inputValues[row][varIndex] = (row >> bitPosition) & 1;
        }
        outputValues[row] = evaluator.evaluate(
            root, variables, inputValues[row], variableCount
        );
    }

    int TruthTable::getRowCount() const {
        return rowCount;
    }

    int TruthTable::getVariableCount() const {
        return variableCount;
    }

    void TruthTable::getVariables(char outVars[MAX_VARIABLES]) const {
        for (int index = 0; index < variableCount; index++) {
            outVars[index] = variables[index];
        }
    }

    bool TruthTable::getInputValue(int row, int varIndex) const {
        return inputValues[row][varIndex];
    }

    bool TruthTable::getOutputValue(int row) const {
        return outputValues[row];
    }

    void TruthTable::printSeparator() const {
        for (int index = 0; index < variableCount; index++) {
            std::cout << "----";
        }
        std::cout << "-----\n";
    }

    void TruthTable::printHeader() const {
        for (int index = 0; index < variableCount; index++) {
            std::cout << " " << variables[index] << " |";
        }
        std::cout << "  f\n";
        printSeparator();
    }

    void TruthTable::printRow(int row) const {
        for (int varIndex = 0; varIndex < variableCount; varIndex++) {
            std::cout << " " << inputValues[row][varIndex] << " |";
        }
        std::cout << "  " << outputValues[row] << "\n";
    }

    void TruthTable::print() const {
        printHeader();
        for (int row = 0; row < rowCount; row++) {
            printRow(row);
        }
    }


    const char* TruthTable::getVariables() const {
        return variables;
    }

}