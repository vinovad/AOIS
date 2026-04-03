#ifndef BCD_SUMMARIZER_H
#define BCD_SUMMARIZER_H

#include <string>
#include "ParallelAdder.h"

namespace MyProject {
    class BCDSummarizer {
    private:
        ParallelAdder binaryAdder;
        int stringToInt(std::string binary) const;
        std::string intToBinary4Bit(int value) const;
        
        std::string tensDigit;   // Десятки
        std::string unitsDigit;  // Единицы
    
    public:
        BCDSummarizer();
        void execute(std::string bcdA, std::string bcdB);
        std::string getFullResult() const;
 };
}

#endif