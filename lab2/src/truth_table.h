#pragma once
#include <iostream>
#include "parser.h"
#include "boolean_function.h"

namespace BoolLogic {
    const int MAX_TRUTH_TABLE_ROWS = 32;
    class TruthTable {

    private:
        Parser parser;
        BooleanFunction evaluator;

        char variables[MAX_VARIABLES];
        bool inputValues[MAX_TRUTH_TABLE_ROWS][MAX_VARIABLES];
        bool outputValues[MAX_TRUTH_TABLE_ROWS];

        int  variableCount;
        int  rowCount;

        void fillRow(int row, AstNode* root);
        void printHeader() const;
        void printRow(int row) const;
        void printSeparator() const;
    
    public:
        TruthTable();
        void build(const char* formula);
        int getRowCount()  const;
        int getVariableCount() const;
        void getVariables(char outVars[MAX_VARIABLES])  const;
        const char* getVariables() const;
        bool  getInputValue(int row, int varIndex) const;
        bool  getOutputValue(int row) const;
        void print() const;
    };

}