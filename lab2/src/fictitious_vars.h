#pragma once
#include <iostream>
#include "truth_table.h"

namespace BoolLogic {

    class FictitiousVars {

    private:
        bool isFictitious[MAX_VARIABLES];
        char variables[MAX_VARIABLES];
        int  variableCount;

        bool checkVariableFictitious(
            const TruthTable& table,
            int varIndex
        ) const;

        int findPartnerRow(int row, int varIndex, int variableCount) const;
        
    public:
        FictitiousVars();
        void build(const TruthTable& table);
        void print() const;
        bool isFictitiousVar(int index) const;

    
    };

}