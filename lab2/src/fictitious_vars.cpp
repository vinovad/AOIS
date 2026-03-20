#include "fictitious_vars.h"

namespace BoolLogic {

    FictitiousVars::FictitiousVars() : variableCount(0) {
        for (int index = 0; index < MAX_VARIABLES; index++) {
            isFictitious[index] = false;
            variables[index] = '\0';
        }
    }

    int FictitiousVars::findPartnerRow(
        int row,
        int varIndex,
        int varCount
    ) const {
        int bitPosition = varCount - 1 - varIndex;
        return row ^ (1 << bitPosition);
    }

    bool FictitiousVars::checkVariableFictitious( const TruthTable& table, int varIndex ) const {
        int rowCount = table.getRowCount();
        for (int row = 0; row < rowCount; row++) {
            int  partnerRow    = findPartnerRow(row, varIndex, table.getVariableCount());
            bool outputsDiffer = table.getOutputValue(row) != table.getOutputValue(partnerRow);
            if (outputsDiffer) {
                return false;
            }
        }
        return true;
    }

    void FictitiousVars::build(const TruthTable& table) {
        variableCount = table.getVariableCount();
        table.getVariables(variables);

        for (int varIndex = 0; varIndex < variableCount; varIndex++) {
            isFictitious[varIndex] = checkVariableFictitious(table, varIndex);
        }
    }

    void FictitiousVars::print() const {
        std::cout << "\n=== Фиктивные переменные ===\n";

        bool anyFictitious = false;
        for (int index = 0; index < variableCount; index++) {
            std::cout << "  " << variables[index] << ": " << (isFictitious[index] ? "фиктивная" : "существенная")  << "\n";
            if (isFictitious[index]) {
                anyFictitious = true;
            }
        }

        if (!anyFictitious) {
            std::cout << "Фиктивных переменных нет.\n";
        }
    }
    bool FictitiousVars::isFictitiousVar(int index) const { return isFictitious[index]; }

}