#include "normal_forms.h"

namespace BoolLogic {

    const char SYMBOL_NOT[] = "!";
    const char SYMBOL_AND[] = " & ";
    const char SYMBOL_OR[] = " | ";
    const char SYMBOL_PAREN_OPEN[] = "(";
    const char SYMBOL_PAREN_CLOSE[]= ")";
    const char SYMBOL_EMPTY[] = "0";
    const char SYMBOL_FULL[] = "1";

    NormalForms::NormalForms()
        : mintermCount(0), maxtermCount(0),
          rowCount(0), variableCount(0) {
        sdnf[0] = '\0';
        sknf[0] = '\0';
    }

    void NormalForms::appendString(char* destination, const char* source) const {
        int destLen   = stringLength(destination);
        int sourceLen = stringLength(source);
        for (int index = 0; index < sourceLen; index++) {
            destination[destLen + index] = source[index];
        }
        destination[destLen + sourceLen] = '\0';
    }

    int NormalForms::stringLength(const char* str) const {
        int length = 0;
        while (str[length] != '\0') {
            length++;
        }
        return length;
    }

    void NormalForms::appendMintermTerm(
        char* buffer,
        const TruthTable& table,
        int row
    ) const {
        appendString(buffer, SYMBOL_PAREN_OPEN);
        for (int varIndex = 0; varIndex < table.getVariableCount(); varIndex++) {
            if (varIndex > 0) {
                appendString(buffer, SYMBOL_AND);
            }
            if (!table.getInputValue(row, varIndex)) {
                appendString(buffer, SYMBOL_NOT);
            }
            char varStr[2] = {table.getVariables()[varIndex], '\0'};
            appendString(buffer, varStr);
        }
        appendString(buffer, SYMBOL_PAREN_CLOSE);
    }

    void NormalForms::appendMaxtermTerm(
        char* buffer,
        const TruthTable& table,
        int row
    ) const {
        appendString(buffer, SYMBOL_PAREN_OPEN);
        for (int varIndex = 0; varIndex < table.getVariableCount(); varIndex++) {
            if (varIndex > 0) {
                appendString(buffer, SYMBOL_OR);
            }
            if (table.getInputValue(row, varIndex)) {
                appendString(buffer, SYMBOL_NOT);
            }
            char varStr[2] = {table.getVariables()[varIndex], '\0'};
            appendString(buffer, varStr);
        }
        appendString(buffer, SYMBOL_PAREN_CLOSE);
    }

    void NormalForms::buildSDNF(const TruthTable& table) {
        sdnf[0] = '\0';
        mintermCount = 0;
        bool firstTerm = true;

        for (int row = 0; row < table.getRowCount(); row++) {
            if (!table.getOutputValue(row)) {
                continue;
            }
            mintermIndices[mintermCount++] = row;
            if (!firstTerm) {
                appendString(sdnf, SYMBOL_OR);
            }
            appendMintermTerm(sdnf, table, row);
            firstTerm = false;
        }

        if (mintermCount == 0) {
            appendString(sdnf, SYMBOL_EMPTY);
        }
    }

    void NormalForms::buildSKNF(const TruthTable& table) {
        sknf[0] = '\0';
        maxtermCount = 0;
        bool firstTerm = true;

        for (int row = 0; row < table.getRowCount(); row++) {
            if (table.getOutputValue(row)) {
                continue;
            }
            maxtermIndices[maxtermCount++] = row;
            if (!firstTerm) {
                appendString(sknf, SYMBOL_AND);
            }
            appendMaxtermTerm(sknf, table, row);
            firstTerm = false;
        }

        if (maxtermCount == 0) {
            appendString(sknf, SYMBOL_FULL);
        }
    }

    void NormalForms::build(const TruthTable& table) {
        rowCount = table.getRowCount();
        variableCount = table.getVariableCount();
        buildSDNF(table);
        buildSKNF(table);
    }

    void NormalForms::printSDNF() const {
        std::cout << "СДНФ: " << sdnf << "\n";
    }

    void NormalForms::printSKNF() const {
        std::cout << "СКНФ: " << sknf << "\n";
    }

    void NormalForms::printNumericSDNF() const {
        std::cout << "СДНФ (числовая): {";
        for (int index = 0; index < mintermCount; index++) {
            if (index > 0) std::cout << ", ";
            std::cout << mintermIndices[index];
        }
        std::cout << "}\n";
    }

    void NormalForms::printNumericSKNF() const {
        std::cout << "СКНФ (числовая): {";
        for (int index = 0; index < maxtermCount; index++) {
            if (index > 0) std::cout << ", ";
            std::cout << maxtermIndices[index];
        }
        std::cout << "}\n";
    }

    void NormalForms::printIndexForm() const {
        std::cout << "Индексная форма: f = (";
        for (int row = 0; row < rowCount; row++) {
            bool isMinterms = false;
            for (int index = 0; index < mintermCount; index++) {
                if (mintermIndices[index] == row) {
                    isMinterms = true;
                    break;
                }
            }
            std::cout << (isMinterms ? 1 : 0);
        }
        std::cout << ")\n";
    }


    int NormalForms::getMintermCount() const { return mintermCount; }
    int NormalForms::getMaxtermCount() const { return maxtermCount; }
    int NormalForms::getMinterm(int index) const { return mintermIndices[index]; }
    int NormalForms::getMaxterm(int index) const { return maxtermIndices[index]; }

}