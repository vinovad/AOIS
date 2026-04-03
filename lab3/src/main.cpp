#include <iostream>
#include <string>
#include "ParallelAdder.h"
#include "Constants.h"
#include "BCDSummarizer.h"
#include "SubtractorCounter.h"

using namespace MyProject;
void printTestResult(const std::string& label, const std::string& a, const std::string& b, 
                     bool cin, const ParallelAdder& adder) {
    std::cout << DELIMITER << std::endl;
    std::cout << LOG_LEVEL_INFO << label << std::endl;
    std::cout << "Input  A: " << a << std::endl;
    std::cout << "Input  B: " << b << std::endl;
    std::cout << "Carry In: " << (cin ? "1" : "0") << std::endl;
    std::cout << "Result S: " << adder.getResultString() << std::endl;
    std::cout << "Carry Out: " << (adder.getFinalCarry() ? "1" : "0") << std::endl;
}

int main() {
    ParallelAdder adder;
    const std::string valueA = "1000";
    const std::string valueB = "0110";
    const bool carryInTrue = true;
    adder.processAddition(valueA, valueB, carryInTrue);
    printTestResult("Test 1: Addition with Carry", valueA, valueB, carryInTrue, adder);
    const bool carryInFalse = false;
    adder.processAddition(valueA, valueB, carryInFalse);
    printTestResult("Test 2: Addition without Carry", valueA, valueB, carryInFalse, adder);

    std::cout << DELIMITER << std::endl;
    BCDSummarizer bcdProcessor;
    std::string valA = "1000"; // 8
    std::string valB = "0110"; // 6

    bcdProcessor.execute(valA, valB);

    std::cout << DELIMITER << std::endl;
    std::cout << LOG_LEVEL_INFO << "BCD Addition Test (Offset n=2)" << std::endl;
    std::cout << "Input A: " << valA << " (8)" << std::endl;
    std::cout << "Input B: " << valB << " (6)" << std::endl;
    std::cout << "Process: 8 + 6 + 2(offset) = 16" << std::endl;
    std::cout << "BCD Result (Tens Units): " << bcdProcessor.getFullResult() << std::endl;
    std::cout << DELIMITER << std::endl;
    SubtractorCounter counter(3);

    std::cout << DELIMITER << std::endl;
    std::cout << LOG_LEVEL_INFO << "Subtractor Counter Test (Countdown)" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "Step " << i << ": Decimal " << counter.getCurrentValueDecimal() 
                  << " | Binary " << counter.getCurrentValueBinary() << std::endl;
        counter.tick();
    }

    std::cout << DELIMITER << std::endl;

    return 0;
}
