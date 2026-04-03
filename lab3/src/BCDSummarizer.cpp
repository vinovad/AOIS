#include "BCDSummarizer.h"
#include "Constants.h"

namespace MyProject {
    BCDSummarizer::BCDSummarizer() : tensDigit("0000"), unitsDigit("0000") {}

    int BCDSummarizer::stringToInt(std::string binary) const {
        int res = 0;
        int weight = 1;
        for (int i = BITS_IN_TETRAD - 1; i >= 0; --i) {
            if (binary[i] == '1') res += weight;
            weight *= 2;
        }
        return res;
    }

    std::string BCDSummarizer::intToBinary4Bit(int value) const {
        std::string res = "0000";
        for (int i = BITS_IN_TETRAD - 1; i >= 0; --i) {
            res[i] = (value % 2 == 1) ? '1' : '0';
            value /= 2;
        }
        return res;
    }

    void BCDSummarizer::execute(std::string bcdA, std::string bcdB) {
        int valA = stringToInt(bcdA);
        int valB = stringToInt(bcdB);
        int rawSum = valA + valB + BCD_OFFSET;
        int units = rawSum % 10;
        int tens = rawSum / 10;
        unitsDigit = intToBinary4Bit(units);
        tensDigit = intToBinary4Bit(tens);
    }

    std::string BCDSummarizer::getFullResult() const {
        return tensDigit + " " + unitsDigit;
    }
}