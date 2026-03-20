#include "post_classes.h"

namespace BoolLogic {

    PostClasses::PostClasses()
        : preservesZero(false), preservesOne(false),
          isSelfDual(false), isMonotone(false), isLinear(false) {
    }

    bool PostClasses::checkPreservesZero(const TruthTable& table) const {
        return !table.getOutputValue(0);
    }

    bool PostClasses::checkPreservesOne(const TruthTable& table) const {
        return table.getOutputValue(table.getRowCount() - 1);
    }

    bool PostClasses::checkSelfDual(const TruthTable& table) const {
        int rowCount = table.getRowCount();
        for (int row = 0; row < rowCount; row++) {
            int mirrorRow = rowCount - 1 - row;
            bool outputMatch  = table.getOutputValue(row) == table.getOutputValue(mirrorRow);
            if (outputMatch) {
                return false;
            }
        }
        return true;
    }

    bool PostClasses::rowDominates(
        const TruthTable& table,
        int rowA,
        int rowB
    ) const {
        for (int varIndex = 0; varIndex < table.getVariableCount(); varIndex++) {
            bool aLessB = !table.getInputValue(rowA, varIndex)
                        && table.getInputValue(rowB, varIndex);
            if (aLessB) {
                return false;
            }
        }
        return true;
    }

    bool PostClasses::checkMonotone(const TruthTable& table) const {
        int rowCount = table.getRowCount();
        for (int rowA = 0; rowA < rowCount; rowA++) {
            for (int rowB = 0; rowB < rowCount; rowB++) {
                bool aLeqB  = rowDominates(table, rowB, rowA);
                bool outputBreaks = table.getOutputValue(rowA)
                                 && !table.getOutputValue(rowB);
                if (aLeqB && outputBreaks) {
                    return false;
                }
            }
        }
        return true;
    }

    bool PostClasses::checkLinear(const TruthTable& table) const {
        int rowCount = table.getRowCount();
        bool zhegalkinCoeffs[MAX_TRUTH_TABLE_ROWS];

        for (int index = 0; index < rowCount; index++) {
            zhegalkinCoeffs[index] = table.getOutputValue(index);
        }

        for (int step = 1; step < rowCount; step <<= 1) {
            for (int index = 0; index < rowCount; index += step << 1) {
                for (int offset = index; offset < index + step; offset++) {
                    zhegalkinCoeffs[offset + step] =
                        zhegalkinCoeffs[offset + step] ^ zhegalkinCoeffs[offset];
                }
            }
        }

        for (int index = 0; index < rowCount; index++) {
            int setBitsCount = 0;
            for (int bit = 0; bit < table.getVariableCount(); bit++) {
                if ((index >> bit) & 1) {
                    setBitsCount++;
                }
            }
            if (setBitsCount >= 2 && zhegalkinCoeffs[index]) {
                return false;
            }
        }
        return true;
    }

    void PostClasses::build(const TruthTable& table) {
        preservesZero = checkPreservesZero(table);
        preservesOne  = checkPreservesOne(table);
        isSelfDual = checkSelfDual(table);
        isMonotone = checkMonotone(table);
        isLinear = checkLinear(table);
    }

    void PostClasses::printClass(
        const char* name,
        const char* desc,
        bool        value
    ) const {
        std::cout << name << " (" << desc << "): " << (value ? "ДА" : "НЕТ") << "\n"; 
    }

    void PostClasses::print() const {
        std::cout << "\n=== Классы Поста ===\n";
        printClass("T0", "сохраняет 0", preservesZero);
        printClass("T1", "сохраняет 1", preservesOne);
        printClass("S ", "самодвойственная", isSelfDual);
        printClass("M ", "монотонная", isMonotone);
        printClass("L ", "линейная", isLinear);

        bool isComplete = !preservesZero || !preservesOne || !isSelfDual || !isMonotone || !isLinear;
        std::cout << "\nФункционально полная система: "
                  << (isComplete ? "ДА" : "НЕТ") << "\n";
    }


    bool PostClasses::getPreservesZero() const { return preservesZero; }
    bool PostClasses::getPreservesOne()  const { return preservesOne;  }
    bool PostClasses::getIsSelfDual()  const { return isSelfDual;  }
    bool PostClasses::getIsMonotone()  const { return isMonotone;  }
    bool PostClasses::getIsLinear()   const { return isLinear;  }

}