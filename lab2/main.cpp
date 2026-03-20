#include <iostream>
#include <string>
#include "src/parser.h"
#include "src/boolean_function.h"
#include "src/truth_table.h"
#include "src/normal_forms.h"
#include "src/post_classes.h"
#include "src/zhegalkin.h"
#include "src/fictitious_vars.h"
#include "src/boolean_diff.h"
#include "src/minimization.h"

using namespace BoolLogic;

static void printWelcome() {
    std::cout << "=== Анализатор булевых функций ===\n";
    std::cout << "Операции: & | ! -> ~\n";
    std::cout << "Переменные: a, b, c, d, e\n";
}

static void printMenu() {
    std::cout << "\n--- Меню ---\n";
    std::cout << "1.  Ввести новую формулу\n";
    std::cout << "2.  Вычислить значение функции\n";
    std::cout << "3.  Таблица истинности\n";
    std::cout << "4.  СДНФ, СКНФ, числовые и индексная формы\n";
    std::cout << "5.  Классы Поста\n";
    std::cout << "6.  Полином Жегалкина\n";
    std::cout << "7.  Фиктивные переменные\n";
    std::cout << "8.  Булева дифференциация\n";
    std::cout << "9.  Минимизация: расчётный метод\n";
    std::cout << "10. Минимизация: расчётно-табличный метод\n";
    std::cout << "11. Минимизация: карта Карно\n";
    std::cout << "0.  Выход\n";
    std::cout << "Выбор: ";
}

static bool readAndValidateFormula(char* buffer) {
    std::cout << "Формула: ";
    std::string line;
    std::getline(std::cin, line);
    int len = (int)line.size();
    if (len >= MAX_FORMULA_LENGTH) { len = MAX_FORMULA_LENGTH - 1; }
    for (int i = 0; i < len; i++) { buffer[i] = line[i]; }
    buffer[len] = '\0';

    Parser parser;
    try {
        parser.parse(buffer);
        std::cout << "Формула принята.\n";
        return true;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << "\n";
        return false;
    }
}

static void readVariableValues(
    const char variables[MAX_VARIABLES],
    bool values[MAX_VARIABLES],
    int variableCount
) {
    std::cout << "Введите значения (0 или 1):\n";
    for (int index = 0; index < variableCount; index++) {
        while (true) {
            std::cout << "  " << variables[index] << " = ";
            std::string inp;
            std::getline(std::cin, inp);
            if (inp == "0") { values[index] = false; break; }
            if (inp == "1") { values[index] = true;  break; }
            std::cout << "  Ошибка: введите 0 или 1\n";
        }
    }
}

static void printEvalResult(
    const char variables[MAX_VARIABLES],
    const bool values[MAX_VARIABLES],
    int variableCount,
    bool result
) {
    std::cout << "f(";
    for (int index = 0; index < variableCount; index++) {
        if (index > 0) { std::cout << ", "; }
        std::cout << variables[index] << "=" << values[index];
    }
    std::cout << ") = " << result << "\n";
}

static void runEvaluate(const char* formula) {
    Parser parser;
    BooleanFunction boolFunc;
    AstNode*  root = parser.parse(formula);
    int variableCount = parser.getVariableCount();
    char variables[MAX_VARIABLES];
    bool values[MAX_VARIABLES];
    parser.getVariables(variables);
    readVariableValues(variables, values, variableCount);
    bool result = boolFunc.evaluate(root, variables, values, variableCount);
    printEvalResult(variables, values, variableCount, result);
}

static TruthTable buildTable(const char* formula) {
    TruthTable table;
    table.build(formula);
    return table;
}

static void runTruthTable(const char* formula) {
    TruthTable table = buildTable(formula);
    std::cout << "\n=== Таблица истинности ===\n";
    table.print();
}

static void runNormalForms(const char* formula) {
    TruthTable  table = buildTable(formula);
    NormalForms forms;
    forms.build(table);
    std::cout << "\n=== Нормальные формы ===\n";
    forms.printSDNF();
    forms.printSKNF();
    forms.printNumericSDNF();
    forms.printNumericSKNF();
    forms.printIndexForm();
}

static void runPostClasses(const char* formula) {
    TruthTable  table = buildTable(formula);
    PostClasses post;
    post.build(table);
    post.print();
}

static void runZhegalkin(const char* formula) {
    TruthTable table = buildTable(formula);
    Zhegalkin  zhegalkin;
    zhegalkin.build(table);
    zhegalkin.print();
}

static void runFictitiousVars(const char* formula) {
    TruthTable     table = buildTable(formula);
    FictitiousVars fictitiousVars;
    fictitiousVars.build(table);
    fictitiousVars.print();
}

static void runBooleanDiff(const char* formula) {
    TruthTable  table = buildTable(formula);
    BooleanDiff diff;
    diff.build(table);
    diff.printPartialAll();
    diff.printAllMixed();
}

static void runMinimizationCalc(const char* formula) {
    TruthTable   table = buildTable(formula);
    Minimization minimization;
    minimization.runCalculation(table);
}

static void runMinimizationCalcTable(const char* formula) {
    TruthTable   table = buildTable(formula);
    Minimization minimization;
    minimization.runCalculationTable(table);
}

static void runMinimizationKarnaugh(const char* formula) {
    TruthTable   table = buildTable(formula);
    Minimization minimization;
    minimization.runKarnaugh(table);
}

int main() {
    printWelcome();

    char formula[MAX_FORMULA_LENGTH];
    std::string choiceStr;
    int choice;

    while (!readAndValidateFormula(formula)) {}

    while (true) {
        printMenu();
        std::getline(std::cin, choiceStr);

        if (choiceStr.empty()) { continue; }

        if (choiceStr == "10") { choice = 10; }
        else if (choiceStr == "11") { choice = 11; }
        else if (choiceStr.size() == 1 && choiceStr[0] >= '0' && choiceStr[0] <= '9') {
            choice = choiceStr[0] - '0';
        }
        else { choice = -1; }

        if (choice == 0)  {  break; }
        else if (choice == 1)  { while (!readAndValidateFormula(formula)) {} }
        else if (choice == 2)  { runEvaluate(formula); }
        else if (choice == 3)  { runTruthTable(formula); }
        else if (choice == 4)  { runNormalForms(formula); }
        else if (choice == 5)  { runPostClasses(formula); }
        else if (choice == 6)  { runZhegalkin(formula); }
        else if (choice == 7)  { runFictitiousVars(formula); }
        else if (choice == 8)  { runBooleanDiff(formula); }
        else if (choice == 9)  { runMinimizationCalc(formula); }
        else if (choice == 10) { runMinimizationCalcTable(formula); }
        else if (choice == 11) { runMinimizationKarnaugh(formula); }
        else { std::cout << "Неверный выбор. Введите число от 0 до 11.\n"; }
    }

    return 0;
}
