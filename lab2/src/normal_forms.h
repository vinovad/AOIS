#pragma once
#include <iostream>
#include "truth_table.h"

namespace BoolLogic {

    const int MAX_TERM_LENGTH  = 64;
    const int MAX_FORMS_LENGTH = 2048;

    class NormalForms {

    private:
        char sdnf[MAX_FORMS_LENGTH];
        char sknf[MAX_FORMS_LENGTH];
        int  mintermIndices[MAX_TRUTH_TABLE_ROWS];
        int  maxtermIndices[MAX_TRUTH_TABLE_ROWS];
        int  mintermCount;
        int  maxtermCount;
        int  rowCount;
        int  variableCount;

        void buildSDNF(const TruthTable& table);
        void buildSKNF(const TruthTable& table);

        void appendMintermTerm(
            char* buffer,
            const TruthTable& table,
            int row
        ) const;

        void appendMaxtermTerm(
            char* buffer,
            const TruthTable& table,
            int row
        ) const;

        void appendString(char* destination, const char* source) const;
        int  stringLength(const char* str) const;

    public:
        NormalForms();

        void build(const TruthTable& table);

        void printSDNF() const;
        void printSKNF() const;
        void printNumericSDNF() const;
        void printNumericSKNF() const;
        void printIndexForm() const;

        int getMintermCount() const;
        int getMaxtermCount() const;
        int getMinterm(int index) const;
        int getMaxterm(int index) const;

 
    };

}