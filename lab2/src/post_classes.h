#pragma once
#include <iostream>
#include "truth_table.h"

namespace BoolLogic {

    class PostClasses {
    private:
        bool preservesZero;
        bool preservesOne;
        bool isSelfDual;
        bool isMonotone;
        bool isLinear;

        bool checkPreservesZero(const TruthTable& table) const;
        bool checkPreservesOne(const TruthTable& table)  const;
        bool checkSelfDual(const TruthTable& table) const;
        bool checkMonotone(const TruthTable& table) const;
        bool checkLinear(const TruthTable& table) const;

        bool rowDominates(
            const TruthTable& table,
            int rowA,
            int rowB
        ) const;

        void printClass(const char* name, const char* desc, bool value) const;
    
    public:
        PostClasses();

        void build(const TruthTable& table);
        void print() const;

        bool getPreservesZero() const;
        bool getPreservesOne()  const;
        bool getIsSelfDual() const;
        bool getIsMonotone() const;
        bool getIsLinear() const;

    };

}