#include "minimization.h"
#include <iomanip>
#include <string>

namespace BoolLogic {
    const int GRAY_ORDER_4[] = {0, 1, 3, 2};
    Implicant::Implicant() : mask(0), value(0), mintermCount(0), used(false) {}

    void  Implicant::setMask(int m)  { mask  = m; }
    void  Implicant::setValue(int v) { value = v; }
    void  Implicant::setUsed(bool u) { used  = u; }
    int   Implicant::getMask() const { return mask; }
    int   Implicant::getValue() const { return value; }
    int   Implicant::getMintermCount() const { return mintermCount; }
    bool  Implicant::isUsed() const { return used; }

    void Implicant::addMinterm(int minterm) {
        if (mintermCount < MAX_IMPLICANT_COVER) {
            mintermList[mintermCount++] = minterm;
        }
    }

    int Implicant::getMinterm(int index) const { return mintermList[index]; }

    bool Implicant::coversMinterm(int minterm) const {
        for (int i = 0; i < mintermCount; i++) {
            if (mintermList[i] == minterm) { return true; }
        }
        return false;
    }

    bool Implicant::equals(const Implicant& other) const {
        return mask == other.mask && value == other.value;
    }

    void Minimization::collectTerms(
        const TruthTable& table,
        bool useMinterms,
        int terms[MAX_TRUTH_TABLE_ROWS],
        int& termCount
    ) const {
        termCount = 0;
        for (int row = 0; row < table.getRowCount(); row++) {
            bool isMint = table.getOutputValue(row);
            if (useMinterms ? isMint : !isMint) {
                terms[termCount++] = row;
            }
        }
    }

    void Minimization::buildInitialImplicants(
        const TruthTable& table,
        bool useMinterms,
        Implicant implicants[MAX_IMPLICANTS],
        int& count
    ) const {
        int terms[MAX_TRUTH_TABLE_ROWS];
        int termCount = 0;
        collectTerms(table, useMinterms, terms, termCount);

        int fullMask = (1 << table.getVariableCount()) - 1;
        count = 0;
        for (int i = 0; i < termCount; i++) {
            implicants[count].setMask(fullMask);
            implicants[count].setValue(terms[i]);
            implicants[count].addMinterm(terms[i]);
            count++;
        }
    }

    bool Minimization::tryGlue(
        const Implicant& first,
        const Implicant& second,
        Implicant&  result
    ) const {
        if (first.getMask() != second.getMask()) { return false; }
        int diff = (first.getValue() ^ second.getValue()) & first.getMask();
        if (diff == 0 || (diff & (diff - 1)) != 0) { return false; }

        result.setMask(first.getMask() & ~diff);
        result.setValue(first.getValue() & ~diff);
        for (int i = 0; i < first.getMintermCount(); i++) {
            result.addMinterm(first.getMinterm(i));
        }
        for (int i = 0; i < second.getMintermCount(); i++) {
            result.addMinterm(second.getMinterm(i));
        }
        return true;
    }

    void Minimization::glueStep(
        Implicant  implicants[MAX_IMPLICANTS],
        int& count,
        Implicant primes[MAX_IMPLICANTS],
        int& primeCount,
        bool verbose,
        const char variables[MAX_VARIABLES],
        int variableCount,
        bool useMinterms
    ) const {
        Implicant next[MAX_IMPLICANTS];
        int  nextCount = 0;

        for (int i = 0; i < count; i++) { implicants[i].setUsed(false); }

        for (int a = 0; a < count; a++) {
            for (int b = a + 1; b < count; b++) {
                Implicant glued;
                if (!tryGlue(implicants[a], implicants[b], glued)) { continue; }
                implicants[a].setUsed(true);
                implicants[b].setUsed(true);

                bool dup = false;
                for (int c = 0; c < nextCount; c++) {
                    if (next[c].equals(glued)) { dup = true; break; }
                }
                if (!dup && nextCount < MAX_IMPLICANTS) {
                    next[nextCount++] = glued;
                    if (verbose) {
                        std::cout << "  Склейка (наборы ";
                        for (int i = 0; i < implicants[a].getMintermCount(); i++) {
                            if (i > 0) std::cout << ",";
                            std::cout << implicants[a].getMinterm(i);
                        }
                        std::cout << " и ";
                        for (int i = 0; i < implicants[b].getMintermCount(); i++) {
                            if (i > 0) std::cout << ",";
                            std::cout << implicants[b].getMinterm(i);
                        }
                        std::cout << "):\n";
                        std::cout << "    ";
                        printImplicant(implicants[a], variables, variableCount, useMinterms);
                        std::cout << "  +  ";
                        printImplicant(implicants[b], variables, variableCount, useMinterms);
                        std::cout << "  ->  ";
                        printImplicant(glued, variables, variableCount, useMinterms);
                        std::cout << "\n";
                    }
                }
            }
        }

        for (int i = 0; i < count; i++) {
            if (implicants[i].isUsed()) { continue; }
            bool dup = false;
            for (int c = 0; c < primeCount; c++) {
                if (primes[c].equals(implicants[i])) { dup = true; break; }
            }
            if (!dup && primeCount < MAX_IMPLICANTS) {
                primes[primeCount++] = implicants[i];
            }
        }

        for (int i = 0; i < nextCount; i++) { implicants[i] = next[i]; }
        count = nextCount;
    }

    void Minimization::findPrimeImplicants(
        const TruthTable& table,
        bool useMinterms,
        Implicant primes[MAX_IMPLICANTS],
        int&  primeCount,
        bool verbose
    ) const {
        Implicant implicants[MAX_IMPLICANTS];
        int count = 0;
        primeCount = 0;

        buildInitialImplicants(table, useMinterms, implicants, count);

        int stage = 1;
        while (count > 0) {
            if (verbose) {
                std::cout << "\n--- Этап склеивания " << stage++ << " ---\n";
            }
            glueStep(implicants, count, primes, primeCount, verbose,
                     table.getVariables(), table.getVariableCount(), useMinterms);
        }
    }

    bool Minimization::isCovered(
        const Implicant covering[MAX_IMPLICANTS],
        int coveringCount,
        int excludeIndex,
        const int terms[],
        int termCount
    ) const {
        for (int m = 0; m < termCount; m++) {
            bool found = false;
            for (int c = 0; c < coveringCount; c++) {
                if (c == excludeIndex) { continue; }
                if (covering[c].coversMinterm(terms[m])) { found = true; break; }
            }
            if (!found) { return false; }
        }
        return true;
    }

    void Minimization::removeRedundant(
        Implicant covering[MAX_IMPLICANTS],
        int& coveringCount,
        const int terms[],
        int termCount
    ) const {
        bool changed = true;
        while (changed) {
            changed = false;
            for (int i = 0; i < coveringCount; i++) {
                if (!isCovered(covering, coveringCount, i, terms, termCount)) { continue; }
                for (int s = i; s < coveringCount - 1; s++) { covering[s] = covering[s + 1]; }
                coveringCount--;
                changed = true;
                break;
            }
        }
    }

    void Minimization::selectCovering(
        const Implicant primes[MAX_IMPLICANTS],
        int primeCount,
        const int terms[],
        int termCount,
        Implicant result[MAX_IMPLICANTS],
        int&  resultCount
    ) const {
        resultCount = 0;
        bool covered[MAX_TRUTH_TABLE_ROWS] = {};

        for (int m = 0; m < termCount; m++) {
            int coverCount = 0, lastPrime = -1;
            for (int p = 0; p < primeCount; p++) {
                if (primes[p].coversMinterm(terms[m])) { coverCount++; lastPrime = p; }
            }
            if (coverCount != 1) { continue; }
            bool dup = false;
            for (int r = 0; r < resultCount; r++) {
                if (result[r].equals(primes[lastPrime])) { dup = true; break; }
            }
            if (!dup && resultCount < MAX_IMPLICANTS) {
                result[resultCount++] = primes[lastPrime];
                for (int i = 0; i < primes[lastPrime].getMintermCount(); i++) {
                    covered[primes[lastPrime].getMinterm(i)] = true;
                }
            }
        }

        for (int m = 0; m < termCount; m++) {
            if (covered[terms[m]]) { continue; }
            int bestIdx = -1, bestCount = 0;
            for (int p = 0; p < primeCount; p++) {
                if (!primes[p].coversMinterm(terms[m])) { continue; }
                int uncov = 0;
                for (int i = 0; i < primes[p].getMintermCount(); i++) {
                    if (!covered[primes[p].getMinterm(i)]) { uncov++; }
                }
                if (uncov > bestCount) { bestCount = uncov; bestIdx = p; }
            }
            if (bestIdx < 0) { continue; }
            bool dup = false;
            for (int r = 0; r < resultCount; r++) {
                if (result[r].equals(primes[bestIdx])) { dup = true; break; }
            }
            if (!dup && resultCount < MAX_IMPLICANTS) {
                result[resultCount++] = primes[bestIdx];
                for (int i = 0; i < primes[bestIdx].getMintermCount(); i++) {
                    covered[primes[bestIdx].getMinterm(i)] = true;
                }
            }
        }

        removeRedundant(result, resultCount, terms, termCount);
    }

    void Minimization::printImplicant(
        const Implicant& implicant,
        const char variables[MAX_VARIABLES],
        int  variableCount,
        bool useMinterms
    ) const {
        bool hasAnyVar = false;
        for (int bit = variableCount - 1; bit >= 0; bit--) {
            int bitMask = 1 << bit;
            if (!(implicant.getMask() & bitMask)) { continue; }
            hasAnyVar = true;
            bool bitSet = implicant.getValue() & bitMask;
            bool negated = useMinterms ? !bitSet : bitSet;
            if (negated) { std::cout << "!"; }
            std::cout << variables[variableCount - 1 - bit];
        }
        if (!hasAnyVar) { std::cout << (useMinterms ? "1" : "0"); }
    }

    static std::string implicantToString(
        const Implicant& implicant,
        const char variables[MAX_VARIABLES],
        int variableCount,
        bool useMinterms
    ) {
        std::string s;
        bool hasAnyVar = false;
        for (int bit = variableCount - 1; bit >= 0; bit--) {
            int bitMask = 1 << bit;
            if (!(implicant.getMask() & bitMask)) { continue; }
            hasAnyVar = true;
            bool bitSet = implicant.getValue() & bitMask;
            bool negated = useMinterms ? !bitSet : bitSet;
            if (negated) { s += '!'; }
            s += variables[variableCount - 1 - bit];
        }
        if (!hasAnyVar) { s = useMinterms ? "1" : "0"; }
        return s;
    }

    void Minimization::printResult(
        const Implicant result[MAX_IMPLICANTS],
        int resultCount,
        const char variables[MAX_VARIABLES],
        int variableCount,
        bool useMinterms
    ) const {
        std::cout << "Результат: ";
        const char* sep = useMinterms ? " | " : " & ";
        for (int i = 0; i < resultCount; i++) {
            if (i > 0) { std::cout << sep; }
            if (!useMinterms) { std::cout << "("; }
            printImplicant(result[i], variables, variableCount, useMinterms);
            if (!useMinterms) { std::cout << ")"; }
        }
        std::cout << "\n";
    }

    void Minimization::printCoveringTable(
        const Implicant primes[MAX_IMPLICANTS],
        int primeCount,
        const int terms[],
        int termCount,
        const char variables[MAX_VARIABLES],
        int variableCount,
        bool  useMinterms
    ) const {
        const int COL_W   = 4;
        const int LABEL_W = 20;

        std::cout << "\n--- Таблица покрытия ---\n";
        std::cout << std::string(LABEL_W, ' ');
        for (int m = 0; m < termCount; m++) {
            std::cout << std::setw(COL_W) << terms[m];
        }
        std::cout << "\n" << std::string(LABEL_W + termCount * COL_W, '-') << "\n";

        for (int p = 0; p < primeCount; p++) {
            std::string label = implicantToString(primes[p], variables, variableCount, useMinterms);
            std::cout << std::left << std::setw(LABEL_W) << label << std::right;
            for (int m = 0; m < termCount; m++) {
                std::cout << std::setw(COL_W)
                          << (primes[p].coversMinterm(terms[m]) ? "1" : "0");
            }
            std::cout << "\n";
        }
    }

    void Minimization::printKarnaugh3(const TruthTable& table) const {
        const char* hdrs[] = {"00", "01", "11", "10"};
        std::cout << "\n" << table.getVariables()[0] << "\\"
                  << table.getVariables()[1] << table.getVariables()[2] << "  ";
        for (int c = 0; c < 4; c++) { std::cout << std::setw(4) << hdrs[c]; }
        std::cout << "\n";
        for (int r = 0; r < 2; r++) {
            std::cout << "  " << r << "   ";
            for (int ci = 0; ci < 4; ci++) {
                std::cout << std::setw(4) << table.getOutputValue((r << 2) | GRAY_ORDER_4[ci]);
            }
            std::cout << "\n";
        }
    }

    void Minimization::printKarnaugh4(const TruthTable& table) const {
        const char* hdrs[] = {"00", "01", "11", "10"};
        std::cout << "\n" << table.getVariables()[0] << table.getVariables()[1]
                  << "\\" << table.getVariables()[2] << table.getVariables()[3] << "  ";
        for (int c = 0; c < 4; c++) { std::cout << std::setw(4) << hdrs[c]; }
        std::cout << "\n";
        for (int ri = 0; ri < 4; ri++) {
            std::cout << "  " << hdrs[ri] << " ";
            for (int ci = 0; ci < 4; ci++) {
                std::cout << std::setw(4) << table.getOutputValue((GRAY_ORDER_4[ri] << 2) | GRAY_ORDER_4[ci]);
            }
            std::cout << "\n";
        }
    }

    void Minimization::printKarnaughGeneric(const TruthTable& table) const {
        std::cout << "Карта Карно для " << table.getVariableCount() << " переменных:\n";
        for (int r = 0; r < table.getRowCount(); r++) {
            std::cout << "  " << r << ": " << table.getOutputValue(r) << "\n";
        }
    }

    void Minimization::runMinimization(
        const TruthTable& table,
        bool useMinterms,
        bool printTable,
        bool verbose
    ) const {
        int terms[MAX_TRUTH_TABLE_ROWS];
        int termCount = 0;
        collectTerms(table, useMinterms, terms, termCount);

        Implicant primes[MAX_IMPLICANTS];
        int primeCount = 0;
        findPrimeImplicants(table, useMinterms, primes, primeCount, verbose);

        if (verbose && !printTable) {
            std::cout << "\nПростые импликанты:\n";
            for (int i = 0; i < primeCount; i++) {
                std::cout << "  ";
                printImplicant(primes[i], table.getVariables(), table.getVariableCount(), useMinterms);
                std::cout << "\n";
            }
        }

        if (printTable) {
            printCoveringTable(primes, primeCount, terms, termCount,
            table.getVariables(), table.getVariableCount(), useMinterms);
        }

        Implicant result[MAX_IMPLICANTS];
        int       resultCount = 0;
        selectCovering(primes, primeCount, terms, termCount, result, resultCount);
        printResult(result, resultCount, table.getVariables(), table.getVariableCount(), useMinterms);
    }

    void Minimization::runCalculation(const TruthTable& table) const {
        std::cout << "\n=== Минимизация: расчётный метод ===\n";
        std::cout << "\n--- СДНФ ---\n";
        runMinimization(table, true,  false, true);
        std::cout << "\n--- СКНФ ---\n";
        runMinimization(table, false, false, true);
    }

    void Minimization::runCalculationTable(const TruthTable& table) const {
        std::cout << "\n=== Минимизация: расчётно-табличный метод ===\n";
        std::cout << "\n--- СДНФ ---\n";
        runMinimization(table, true,  true, true);
        std::cout << "\n--- СКНФ ---\n";
        runMinimization(table, false, true, true);
    }

    void Minimization::runKarnaugh(const TruthTable& table) const {
        std::cout << "\n=== Минимизация: карта Карно ===\n";
        int varCount = table.getVariableCount();
        if (varCount == 3) { printKarnaugh3(table); }
        else if (varCount == 4) { printKarnaugh4(table); }
        else { printKarnaughGeneric(table); }

        std::cout << "\n--- СДНФ ---\n";
        runMinimization(table, true,  false, false);
        std::cout << "\n--- СКНФ ---\n";
        runMinimization(table, false, false, false);
    }

    void Minimization::getPrimeResult(
        const TruthTable& table,
        Implicant result[MAX_IMPLICANTS],
        int& resultCount
    ) const {
        int terms[MAX_TRUTH_TABLE_ROWS];
        int termCount = 0;
        collectTerms(table, true, terms, termCount);
        Implicant primes[MAX_IMPLICANTS];
        int primeCount = 0;
        findPrimeImplicants(table, true, primes, primeCount, false);
        selectCovering(primes, primeCount, terms, termCount, result, resultCount);
    }

}