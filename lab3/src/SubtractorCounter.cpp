#include "SubtractorCounter.h"
#include "Constants.h"

namespace MyProject {
    SubtractorCounter::SubtractorCounter(int startValue) {
        const int MAX_3BIT = 7;
        currentValue = (startValue >= 0 && startValue <= MAX_3BIT) 
                       ? startValue 
                       : MAX_3BIT;
    }

    void SubtractorCounter::tick() {
        if (currentValue > 0) {
            currentValue--;
        } else {
            currentValue = 7; 
        }
    }

    int SubtractorCounter::getCurrentValueDecimal() const {
        return currentValue;
    }

    std::string SubtractorCounter::getCurrentValueBinary() const {
        return toBinary3Bit(currentValue);
    }
    std::string SubtractorCounter::toBinary3Bit(int value) const {
        std::string res = "000"; 
        int temp = value;
        for (int i = 2; i >= 0; --i) {
            res[i] = (temp % 2 == 1) ? '1' : '0';
            temp /= 2;
        }
        return res;
    }
}