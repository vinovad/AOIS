#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include "parser.h"
#include "boolean_function.h"
#include "truth_table.h"
#include "normal_forms.h"
#include "post_classes.h"
#include "zhegalkin.h"
#include "fictitious_vars.h"
#include "boolean_diff.h"
#include "minimization.h"

using namespace BoolLogic;


// Parser


TEST(ParserTest, VariableCount_ThreeVars) {
    Parser parser;
    parser.parse("a&b|c");
    EXPECT_EQ(parser.getVariableCount(), 3);
}

TEST(ParserTest, VariableCount_SingleVar) {
    Parser parser;
    parser.parse("a");
    EXPECT_EQ(parser.getVariableCount(), 1);
}

TEST(ParserTest, DuplicateVariables_CountedOnce) {
    Parser parser;
    parser.parse("a&a|a");
    EXPECT_EQ(parser.getVariableCount(), 1);
}

TEST(ParserTest, InvalidFormula_Throws) {
    Parser parser;
    EXPECT_THROW(parser.parse("a&&b"), std::exception);
}

TEST(ParserTest, EmptyFormula_Throws) {
    Parser parser;
    EXPECT_THROW(parser.parse(""), std::exception);
}

TEST(ParserTest, Implication_ParsesOk) {
    Parser parser;
    EXPECT_NE(parser.parse("a->b"), nullptr);
}

TEST(ParserTest, Equality_ParsesOk) {
    Parser parser;
    EXPECT_NE(parser.parse("a~b"), nullptr);
}

TEST(ParserTest, NestedFormula_ParsesOk) {
    Parser parser;
    EXPECT_NE(parser.parse("!(!a->!b)&c"), nullptr);
    EXPECT_EQ(parser.getVariableCount(), 3);
}

TEST(ParserTest, FiveVariables) {
    Parser parser;
    parser.parse("a&b|c&d|e");
    EXPECT_EQ(parser.getVariableCount(), 5);
}

TEST(ParserTest, GetVariables_Order) {
    Parser parser;
    parser.parse("a&b&c");
    char vars[MAX_VARIABLES];
    parser.getVariables(vars);
    EXPECT_EQ(vars[0], 'a');
    EXPECT_EQ(vars[1], 'b');
    EXPECT_EQ(vars[2], 'c');
}

TEST(ParserTest, Parentheses_ParsesOk) {
    Parser parser;
    EXPECT_NE(parser.parse("(a|b)&c"), nullptr);
}

TEST(ParserTest, DoubleNot_ParsesOk) {
    Parser parser;
    EXPECT_NE(parser.parse("!!a"), nullptr);
}


// BooleanFunction


static bool evalFormula(
    const char* formula,
    const char  vars[MAX_VARIABLES],
    const bool  vals[MAX_VARIABLES],
    int   varCount
) {
    Parser          parser;
    BooleanFunction bf;
    AstNode*        root = parser.parse(formula);
    return bf.evaluate(root, vars, vals, varCount);
}

TEST(BooleanFunctionTest, And_BothTrue) {
    char vars[] = {'a', 'b'};
    bool vals[] = {true, true};
    EXPECT_EQ(evalFormula("a&b", vars, vals, 2), true);
}

TEST(BooleanFunctionTest, And_OneFalse) {
    char vars[] = {'a', 'b'};
    bool vals[] = {true, false};
    EXPECT_EQ(evalFormula("a&b", vars, vals, 2), false);
}

TEST(BooleanFunctionTest, Or_OneFalse) {
    char vars[] = {'a', 'b'};
    bool vals[] = {false, true};
    EXPECT_EQ(evalFormula("a|b", vars, vals, 2), true);
}

TEST(BooleanFunctionTest, Or_BothFalse) {
    char vars[] = {'a', 'b'};
    bool vals[] = {false, false};
    EXPECT_EQ(evalFormula("a|b", vars, vals, 2), false);
}

TEST(BooleanFunctionTest, Not_False) {
    char vars[] = {'a'};
    bool vals[] = {false};
    EXPECT_EQ(evalFormula("!a", vars, vals, 1), true);
}

TEST(BooleanFunctionTest, Not_True) {
    char vars[] = {'a'};
    bool vals[] = {true};
    EXPECT_EQ(evalFormula("!a", vars, vals, 1), false);
}

TEST(BooleanFunctionTest, Implication_AllCases) {
    char vars[] = {'a', 'b'};
    bool vals00[] = {false, false};
    bool vals01[] = {false, true};
    bool vals10[] = {true,  false};
    bool vals11[] = {true,  true};
    EXPECT_EQ(evalFormula("a->b", vars, vals00, 2), true);
    EXPECT_EQ(evalFormula("a->b", vars, vals01, 2), true);
    EXPECT_EQ(evalFormula("a->b", vars, vals10, 2), false);
    EXPECT_EQ(evalFormula("a->b", vars, vals11, 2), true);
}

TEST(BooleanFunctionTest, Equality_AllCases) {
    char vars[] = {'a', 'b'};
    bool vals00[] = {false, false};
    bool vals11[] = {true,  true};
    bool vals10[] = {true,  false};
    bool vals01[] = {false, true};
    EXPECT_EQ(evalFormula("a~b", vars, vals00, 2), true);
    EXPECT_EQ(evalFormula("a~b", vars, vals11, 2), true);
    EXPECT_EQ(evalFormula("a~b", vars, vals10, 2), false);
    EXPECT_EQ(evalFormula("a~b", vars, vals01, 2), false);
}

TEST(BooleanFunctionTest, NestedFormula_Minterm3) {
    char vars[] = {'a', 'b', 'c'};
    bool vals011[] = {false, true,  true};
    bool vals111[] = {true,  true,  true};
    EXPECT_EQ(evalFormula("!(!a->!b)&c", vars, vals011, 3), true);
    EXPECT_EQ(evalFormula("!(!a->!b)&c", vars, vals111, 3), false);
}

TEST(BooleanFunctionTest, ComplexNested_FiveVars) {
    char vars[] = {'a', 'b', 'c', 'd', 'e'};
    bool vals[]  = {true, false, true, false, true};
    bool result = evalFormula("(a|b)&(c|d)&e", vars, vals, 5);
    EXPECT_EQ(result, true);
}


// TruthTable


TEST(TruthTableTest, RowCount_TwoVars) {
    TruthTable t;
    t.build("a&b");
    EXPECT_EQ(t.getRowCount(), 4);
}

TEST(TruthTableTest, RowCount_ThreeVars) {
    TruthTable t;
    t.build("a&b&c");
    EXPECT_EQ(t.getRowCount(), 8);
}

TEST(TruthTableTest, RowCount_FourVars) {
    TruthTable t;
    t.build("a&b&c&d");
    EXPECT_EQ(t.getRowCount(), 16);
}

TEST(TruthTableTest, RowCount_FiveVars) {
    TruthTable t;
    t.build("a&b&c&d&e");
    EXPECT_EQ(t.getRowCount(), 32);
}

TEST(TruthTableTest, And_AllRows) {
    TruthTable t;
    t.build("a&b");
    EXPECT_EQ(t.getOutputValue(0), false);
    EXPECT_EQ(t.getOutputValue(1), false);
    EXPECT_EQ(t.getOutputValue(2), false);
    EXPECT_EQ(t.getOutputValue(3), true);
}

TEST(TruthTableTest, Or_AllRows) {
    TruthTable t;
    t.build("a|b");
    EXPECT_EQ(t.getOutputValue(0), false);
    EXPECT_EQ(t.getOutputValue(1), true);
    EXPECT_EQ(t.getOutputValue(2), true);
    EXPECT_EQ(t.getOutputValue(3), true);
}

TEST(TruthTableTest, NestedFormula_OnlyMinterm3) {
    TruthTable t;
    t.build("!(!a->!b)&c");
    EXPECT_EQ(t.getOutputValue(0), false);
    EXPECT_EQ(t.getOutputValue(3), true);
    EXPECT_EQ(t.getOutputValue(7), false);
}

TEST(TruthTableTest, GetInputValue_Correct) {
    TruthTable t;
    t.build("a&b");
    EXPECT_EQ(t.getInputValue(0, 0), false);
    EXPECT_EQ(t.getInputValue(0, 1), false);
    EXPECT_EQ(t.getInputValue(3, 0), true);
    EXPECT_EQ(t.getInputValue(3, 1), true);
    EXPECT_EQ(t.getInputValue(2, 0), true);
    EXPECT_EQ(t.getInputValue(2, 1), false);
}

TEST(TruthTableTest, GetVariables_Correct) {
    TruthTable t;
    t.build("a&b&c");
    const char* vars = t.getVariables();
    EXPECT_EQ(vars[0], 'a');
    EXPECT_EQ(vars[1], 'b');
    EXPECT_EQ(vars[2], 'c');
}

TEST(TruthTableTest, Print_DoesNotCrash) {
    TruthTable t;
    t.build("a&b");
    EXPECT_NO_THROW(t.print());
}

TEST(TruthTableTest, Implication_AllRows) {
    TruthTable t;
    t.build("a->b");
    EXPECT_EQ(t.getOutputValue(0), true);
    EXPECT_EQ(t.getOutputValue(1), true);
    EXPECT_EQ(t.getOutputValue(2), false);
    EXPECT_EQ(t.getOutputValue(3), true);
}

TEST(TruthTableTest, Equality_AllRows) {
    TruthTable t;
    t.build("a~b");
    EXPECT_EQ(t.getOutputValue(0), true);
    EXPECT_EQ(t.getOutputValue(1), false);
    EXPECT_EQ(t.getOutputValue(2), false);
    EXPECT_EQ(t.getOutputValue(3), true);
}


// NormalForms


TEST(NormalFormsTest, And_MintermMaxtermCount) {
    TruthTable  t;
    NormalForms nf;
    t.build("a&b");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 1);
    EXPECT_EQ(nf.getMaxtermCount(), 3);
}

TEST(NormalFormsTest, Or_MintermMaxtermCount) {
    TruthTable  t;
    NormalForms nf;
    t.build("a|b");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 3);
    EXPECT_EQ(nf.getMaxtermCount(), 1);
}

TEST(NormalFormsTest, NestedFormula_Minterms) {
    TruthTable  t;
    NormalForms nf;
    t.build("!(!a->!b)&c");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 1);
    EXPECT_EQ(nf.getMinterm(0), 3);
    EXPECT_EQ(nf.getMaxtermCount(), 7);
}

TEST(NormalFormsTest, Contradiction_ZeroMinterms) {
    TruthTable  t;
    NormalForms nf;
    t.build("a&!a");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 0);
    EXPECT_EQ(nf.getMaxtermCount(), 2);
}

TEST(NormalFormsTest, Tautology_ZeroMaxterms) {
    TruthTable  t;
    NormalForms nf;
    t.build("a|!a");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 2);
    EXPECT_EQ(nf.getMaxtermCount(), 0);
}

TEST(NormalFormsTest, Implication_Minterms) {
    TruthTable  t;
    NormalForms nf;
    t.build("a->b");
    nf.build(t);
    EXPECT_EQ(nf.getMintermCount(), 3);
    EXPECT_EQ(nf.getMaxtermCount(), 1);
    EXPECT_EQ(nf.getMaxterm(0), 2);
}

TEST(NormalFormsTest, PrintSDNF_DoesNotCrash) {
    TruthTable  t;
    NormalForms nf;
    t.build("a&b");
    nf.build(t);
    EXPECT_NO_THROW(nf.printSDNF());
    EXPECT_NO_THROW(nf.printSKNF());
    EXPECT_NO_THROW(nf.printNumericSDNF());
    EXPECT_NO_THROW(nf.printNumericSKNF());
    EXPECT_NO_THROW(nf.printIndexForm());
}

TEST(NormalFormsTest, ThreeVars_Minterms) {
    TruthTable  t;
    NormalForms nf;
    t.build("a&b|c");
    nf.build(t);
    EXPECT_GT(nf.getMintermCount(), 0);
    EXPECT_GT(nf.getMaxtermCount(), 0);
}


// PostClasses


TEST(PostClassesTest, And_AllClasses) {
    TruthTable  t;
    PostClasses pc;
    t.build("a&b");
    pc.build(t);
    EXPECT_EQ(pc.getPreservesZero(), true);
    EXPECT_EQ(pc.getPreservesOne(),  true);
    EXPECT_EQ(pc.getIsSelfDual(),    false);
    EXPECT_EQ(pc.getIsMonotone(),    true);
    EXPECT_EQ(pc.getIsLinear(),      false);
}

TEST(PostClassesTest, Equality_Classes) {
    TruthTable  t;
    PostClasses pc;
    t.build("a~b");
    pc.build(t);
    EXPECT_EQ(pc.getPreservesZero(), false);
    EXPECT_EQ(pc.getPreservesOne(),  true);
    EXPECT_EQ(pc.getIsLinear(),      true);
}

TEST(PostClassesTest, Or_Classes) {
    TruthTable  t;
    PostClasses pc;
    t.build("a|b");
    pc.build(t);
    EXPECT_EQ(pc.getPreservesZero(), true);
    EXPECT_EQ(pc.getPreservesOne(),  true);
    EXPECT_EQ(pc.getIsMonotone(),    true);
    EXPECT_EQ(pc.getIsLinear(),      false);
}

TEST(PostClassesTest, Not_Classes) {
    TruthTable  t;
    PostClasses pc;
    t.build("!a");
    pc.build(t);
    EXPECT_EQ(pc.getPreservesZero(), false);
    EXPECT_EQ(pc.getPreservesOne(),  false);
    EXPECT_EQ(pc.getIsSelfDual(),    true);
    EXPECT_EQ(pc.getIsMonotone(),    false);
    EXPECT_EQ(pc.getIsLinear(),      true);
}

TEST(PostClassesTest, Print_DoesNotCrash) {
    TruthTable  t;
    PostClasses pc;
    t.build("a&b");
    pc.build(t);
    EXPECT_NO_THROW(pc.print());
}


// Zhegalkin


TEST(ZhegalkinTest, And_Polynomial) {
    TruthTable t;
    Zhegalkin  z;
    t.build("a&b");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "ab");
}

TEST(ZhegalkinTest, Contradiction_Zero) {
    TruthTable t;
    Zhegalkin  z;
    t.build("a&!a");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "0");
}

TEST(ZhegalkinTest, Tautology_One) {
    TruthTable t;
    Zhegalkin  z;
    t.build("a|!a");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "1");
}

TEST(ZhegalkinTest, NestedFormula_Polynomial) {
    TruthTable t;
    Zhegalkin  z;
    t.build("!(!a->!b)&c");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "bc + abc");
}

TEST(ZhegalkinTest, SingleVar_Polynomial) {
    TruthTable t;
    Zhegalkin  z;
    t.build("a");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "a");
}

TEST(ZhegalkinTest, Not_Polynomial) {
    TruthTable t;
    Zhegalkin  z;
    t.build("!a");
    z.build(t);
    EXPECT_EQ(std::string(z.getPolynomial()), "1 + a");
}

TEST(ZhegalkinTest, Print_DoesNotCrash) {
    TruthTable t;
    Zhegalkin  z;
    t.build("a&b");
    z.build(t);
    EXPECT_NO_THROW(z.print());
}


// FictitiousVars


TEST(FictitiousVarsTest, And_NoFictitious) {
    TruthTable     t;
    FictitiousVars fv;
    t.build("a&b");
    fv.build(t);
    EXPECT_EQ(fv.isFictitiousVar(0), false);
    EXPECT_EQ(fv.isFictitiousVar(1), false);
}

TEST(FictitiousVarsTest, Tautology_SecondFictitious) {
    TruthTable     t;
    FictitiousVars fv;
    t.build("a|(b&!b)");
    fv.build(t);
    EXPECT_EQ(fv.isFictitiousVar(0), false);
    EXPECT_EQ(fv.isFictitiousVar(1), true);
}

TEST(FictitiousVarsTest, Contradiction_BothFictitious) {
    TruthTable     t;
    FictitiousVars fv;
    t.build("a&!a|b&!b");
    fv.build(t);
    EXPECT_EQ(fv.isFictitiousVar(0), true);
    EXPECT_EQ(fv.isFictitiousVar(1), true);
}

TEST(FictitiousVarsTest, ThreeVars_OneEssential) {
    TruthTable     t;
    FictitiousVars fv;
    t.build("a|(b&!b)|(c&!c)");
    fv.build(t);
    EXPECT_EQ(fv.isFictitiousVar(0), false);
    EXPECT_EQ(fv.isFictitiousVar(1), true);
    EXPECT_EQ(fv.isFictitiousVar(2), true);
}

TEST(FictitiousVarsTest, Print_DoesNotCrash) {
    TruthTable     t;
    FictitiousVars fv;
    t.build("a&b");
    fv.build(t);
    EXPECT_NO_THROW(fv.print());
}


// BooleanDiff


TEST(BooleanDiffTest, And_PartialA) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a&b");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(0, d);
    EXPECT_EQ(d[0], false);
    EXPECT_EQ(d[1], true);
    EXPECT_EQ(d[2], false);
    EXPECT_EQ(d[3], true);
}

TEST(BooleanDiffTest, And_PartialB) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a&b");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(1, d);
    EXPECT_EQ(d[0], false);
    EXPECT_EQ(d[1], false);
    EXPECT_EQ(d[2], true);
    EXPECT_EQ(d[3], true);
}

TEST(BooleanDiffTest, NestedFormula_PartialC) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("!(!a->!b)&c");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(2, d);
    EXPECT_EQ(d[0], false);
    EXPECT_EQ(d[1], false);
    EXPECT_EQ(d[2], true);
    EXPECT_EQ(d[3], true);
    EXPECT_EQ(d[4], false);
    EXPECT_EQ(d[5], false);
    EXPECT_EQ(d[6], false);
    EXPECT_EQ(d[7], false);
}

TEST(BooleanDiffTest, NestedFormula_PartialA) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("!(!a->!b)&c");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(0, d);
    // df/da = (00010001)
    EXPECT_EQ(d[3], true);
    EXPECT_EQ(d[0], false);
    EXPECT_EQ(d[7], true);
}

TEST(BooleanDiffTest, NestedFormula_PartialB) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("!(!a->!b)&c");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(1, d);
    // df/db = (01010000)
    EXPECT_EQ(d[1], true);
    EXPECT_EQ(d[3], true);
    EXPECT_EQ(d[5], false);
}

TEST(BooleanDiffTest, PrintPartialAll_DoesNotCrash) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a&b&c");
    bd.build(t);
    EXPECT_NO_THROW(bd.printPartialAll());
}

TEST(BooleanDiffTest, PrintAllMixed_DoesNotCrash) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a&b&c");
    bd.build(t);
    EXPECT_NO_THROW(bd.printAllMixed());
}

TEST(BooleanDiffTest, PrintMixed_TwoVars) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a&b&c");
    bd.build(t);
    int vars[] = {0, 1};
    EXPECT_NO_THROW(bd.printMixed(vars, 2));
}

TEST(BooleanDiffTest, Or_PartialA) {
    TruthTable  t;
    BooleanDiff bd;
    t.build("a|b");
    bd.build(t);
    bool d[MAX_TRUTH_TABLE_ROWS];
    bd.getPartialDerivative(0, d);
    // d(a|b)/da = !b
    EXPECT_EQ(d[0], true);
    EXPECT_EQ(d[1], false);
    EXPECT_EQ(d[2], true);
    EXPECT_EQ(d[3], false);
}


// Minimization


TEST(MinimizationTest, SingleMinterm_ResultCount) {
    TruthTable   t;
    Minimization m;
    t.build("!(!a->!b)&c");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_EQ(resultCount, 1);
}

TEST(MinimizationTest, SingleMinterm_CoversMinterm3) {
    TruthTable   t;
    Minimization m;
    t.build("!(!a->!b)&c");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_GT(resultCount, 0);
    EXPECT_EQ(result[0].getMintermCount(), 1);
}

TEST(MinimizationTest, TwoMinterms_ResultCount) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|!a&b&c");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_EQ(resultCount, 2);
}

TEST(MinimizationTest, Contradiction_ZeroImplicants) {
    TruthTable   t;
    Minimization m;
    t.build("a&!a");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_EQ(resultCount, 0);
}

TEST(MinimizationTest, Tautology_OneImplicant) {
    TruthTable   t;
    Minimization m;
    t.build("a|!a");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_EQ(resultCount, 1);
}

TEST(MinimizationTest, RunCalculation_DoesNotCrash) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|!a&b&c");
    EXPECT_NO_THROW(m.runCalculation(t));
}

TEST(MinimizationTest, RunCalculationTable_DoesNotCrash) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|!a&b&c");
    EXPECT_NO_THROW(m.runCalculationTable(t));
}

TEST(MinimizationTest, RunKarnaugh3_DoesNotCrash) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|c");
    EXPECT_NO_THROW(m.runKarnaugh(t));
}

TEST(MinimizationTest, RunKarnaugh4_DoesNotCrash) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|c&d");
    EXPECT_NO_THROW(m.runKarnaugh(t));
}

TEST(MinimizationTest, RunKarnaughFiveVars_DoesNotCrash) {
    TruthTable   t;
    Minimization m;
    t.build("a&b|c&d|e");
    EXPECT_NO_THROW(m.runKarnaugh(t));
}

TEST(MinimizationTest, FourVars_Gluing) {
    TruthTable   t;
    Minimization m;
    t.build("a&b&c|a&b&d|a&b&!c|a&b&!d");
    Implicant result[MAX_IMPLICANTS];
    int resultCount = 0;
    m.getPrimeResult(t, result, resultCount);
    EXPECT_GT(resultCount, 0);
}

TEST(MinimizationTest, Implicant_CoversMinterm) {
    Implicant imp;
    imp.setMask(0b11);
    imp.setValue(0b11);
    imp.addMinterm(3);
    EXPECT_EQ(imp.coversMinterm(3), true);
    EXPECT_EQ(imp.coversMinterm(0), false);
}

TEST(MinimizationTest, Implicant_Equals) {
    Implicant a, b;
    a.setMask(3);  a.setValue(3);
    b.setMask(3);  b.setValue(3);
    EXPECT_EQ(a.equals(b), true);
    b.setValue(2);
    EXPECT_EQ(a.equals(b), false);
}
