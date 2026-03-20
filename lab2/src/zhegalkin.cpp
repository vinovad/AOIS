#include "zhegalkin.h"

namespace BoolLogic {

    const char ZHEGALKIN_XOR[]   = " + ";
    const char ZHEGALKIN_CONST[] = "1";
    const char ZHEGALKIN_ZERO[]  = "0";

    Zhegalkin::Zhegalkin()
        : rowCount(0), variableCount(0) {
        polynomial[0] = '\0';
    }

    void Zhegalkin::appendString(char* destination, const char* source) const {
        int destLen   = stringLength(destination);
        int sourceLen = stringLength(source);
        for (int index = 0; index < sourceLen; index++) {
            destination[destLen + index] = source[index];
        }
        destination[destLen + sourceLen] = '\0';
    }

    int Zhegalkin::stringLength(const char* str) const {
        int length = 0;
        while (str[length] != '\0') {
            length++;
        }
        return length;
    }

    void Zhegalkin::computeCoefficients(const TruthTable& table) {
        rowCount = table.getRowCount();
        variableCount = table.getVariableCount();

        for (int index = 0; index < rowCount; index++) {
            coefficients[index] = table.getOutputValue(index);
        }

        for (int step = 1; step < rowCount; step <<= 1) {
            for (int index = 0; index < rowCount; index += step << 1) {
                for (int offset = index; offset < index + step; offset++) {
                    coefficients[offset + step] =
                        coefficients[offset + step] ^ coefficients[offset];
                }
            }
        }
    }

    void Zhegalkin::appendTerm(const TruthTable& table, int termIndex) {
        if (termIndex == 0) {
            appendString(polynomial, ZHEGALKIN_CONST);
            return;
        }
        for (int bit = variableCount - 1; bit >= 0; bit--) {
            if ((termIndex >> bit) & 1) {
                char varStr[2] = {table.getVariables()[variableCount - 1 - bit], '\0'};
                appendString(polynomial, varStr);
            }
        }
    }

    void Zhegalkin::buildPolynomialString(const TruthTable& table) {
        polynomial[0]   = '\0';
        bool firstTerm  = true;

        for (int index = 0; index < rowCount; index++) {
            if (!coefficients[index]) {
                continue;
            }
            if (!firstTerm) {
                appendString(polynomial, ZHEGALKIN_XOR);
            }
            appendTerm(table, index);
            firstTerm = false;
        }

        if (firstTerm) {
            appendString(polynomial, ZHEGALKIN_ZERO);
        }
    }

    void Zhegalkin::build(const TruthTable& table) {
        computeCoefficients(table);
        buildPolynomialString(table);
    }

    void Zhegalkin::print() const {
        std::cout << "\n=== Полином Жегалкина ===\n";
        std::cout << polynomial << "\n";
    }


    const char* Zhegalkin::getPolynomial() const { return polynomial; }

}