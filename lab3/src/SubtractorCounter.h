#ifndef SUBTRACTOR_COUNTER_H
#define SUBTRACTOR_COUNTER_H

#include <string>

namespace MyProject {
    class SubtractorCounter {
    public:
        explicit SubtractorCounter(int startValue);
        void tick();
        std::string getCurrentValueBinary() const;
        int getCurrentValueDecimal() const;

    private:
        int currentValue;
        std::string toBinary3Bit(int value) const;
    };
}

#endif