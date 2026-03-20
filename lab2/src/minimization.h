#pragma once

#include <iostream>
#include "truth_table.h"

namespace BoolLogic {

    const int MAX_IMPLICANTS      = 256;
    const int MAX_IMPLICANT_COVER = 32;

    class Implicant {
    
    private:
        int  mask;
        int  value;
        int  mintermList[MAX_IMPLICANT_COVER];
        int  mintermCount;
        bool used;
    
    public:
        Implicant();

        void  setMask(int mask);
        void  setValue(int value);
        void  addMinterm(int minterm);
        void  setUsed(bool used);

        int   getMask()   const;
        int   getValue()  const;
        int   getMintermCount()  const;
        int   getMinterm(int index) const;
        bool  isUsed()  const;
        bool  coversMinterm(int minterm)   const;
        bool  equals(const Implicant& other) const;


    };

    class Minimization {
    private:
        void collectTerms(
            const TruthTable& table,
            bool useMinterms,
            int  terms[MAX_TRUTH_TABLE_ROWS],
            int& termCount
        ) const;

        void buildInitialImplicants(
            const TruthTable& table,
            bool useMinterms,
            Implicant implicants[MAX_IMPLICANTS],
            int& count
        ) const;

        bool tryGlue(
            const Implicant& first,
            const Implicant& second,
            Implicant& result
        ) const;

        void glueStep(
            Implicant implicants[MAX_IMPLICANTS],
            int& count,
            Implicant  primes[MAX_IMPLICANTS],
            int&  primeCount,
            bool  verbose,
            const char variables[MAX_VARIABLES],
            int variableCount,
            bool useMinterms
        ) const;

        void findPrimeImplicants(
            const TruthTable& table,
            bool useMinterms,
            Implicant  primes[MAX_IMPLICANTS],
            int& primeCount,
            bool verbose
        ) const;

        void selectCovering(
            const Implicant primes[MAX_IMPLICANTS],
            int primeCount,
            const int terms[],
            int termCount,
            Implicant result[MAX_IMPLICANTS],
            int& resultCount
        ) const;

        void removeRedundant(
            Implicant covering[MAX_IMPLICANTS],
            int& coveringCount,
            const int terms[],
            int termCount
        ) const;

        bool isCovered(
            const Implicant covering[MAX_IMPLICANTS],
            int coveringCount,
            int excludeIndex,
            const int terms[],
            int termCount
        ) const;

        void printImplicant(
            const Implicant&  implicant,
            const char variables[MAX_VARIABLES],
            int  variableCount,
            bool useMinterms
        ) const;

        void printCoveringTable(
            const Implicant primes[MAX_IMPLICANTS],
            int primeCount,
            const int terms[],
            int termCount,
            const char variables[MAX_VARIABLES],
            int variableCount,
            bool useMinterms
        ) const;

        void printResult(
            const Implicant result[MAX_IMPLICANTS],
            int resultCount,
            const char variables[MAX_VARIABLES],
            int variableCount,
            bool useMinterms
        ) const;

        void runMinimization(
            const TruthTable& table,
            bool useMinterms,
            bool printTable,
            bool verbose
        ) const;

        void printKarnaugh3(const TruthTable& table) const;
        void printKarnaugh4(const TruthTable& table) const;
        void printKarnaughGeneric(const TruthTable& table) const;

    public:
        void runCalculation(const TruthTable& table) const;
        void runCalculationTable(const TruthTable& table) const;
        void runKarnaugh(const TruthTable& table) const;
        void getPrimeResult(const TruthTable& table, Implicant result[MAX_IMPLICANTS], int& resultCount) const;

    
    };

}