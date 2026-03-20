#pragma once
#include <stdexcept>

namespace BoolLogic {

    const int  MAX_FORMULA_LENGTH = 256;
    const int  MAX_VARIABLES = 5;
    const int  MAX_AST_NODES = 512;

    const char VAR_MIN = 'a';
    const char VAR_MAX = 'e';
    const char OP_NOT = '!';
    const char OP_AND = '&';
    const char OP_OR = '|';
    const char OP_IMPL_DASH  = '-';
    const char OP_IMPL_ARROW = '>';
    const char OP_EQ = '~';
    const char PAREN_OPEN  = '(';
    const char PAREN_CLOSE = ')';
    const char CHAR_SPACE = ' ';
    const char CHAR_NULL = '\0';

    enum class NodeKind {
        Variable,
        OperatorNot,
        OperatorAnd,
        OperatorOr,
        OperatorImpl,
        OperatorEq
    };

    class AstNode {
    private:
        NodeKind nodeKind;
        char variableName;
        AstNode* leftChild;
        AstNode* rightChild;

    public:
        AstNode();

        void setKind(NodeKind kind);
        void setVariable(char variable);
        void setLeft(AstNode* node);
        void setRight(AstNode* node);

        NodeKind getKind() const;
        char getVariable() const;
        AstNode* getLeft() const;
        AstNode* getRight() const;

        bool isLeaf() const;
        bool isBinary() const;

    
    };

    class Parser {

    private:
        const char* source;
        int position;
        AstNode nodePool[MAX_AST_NODES];
        int poolSize;
        char foundVars[MAX_VARIABLES];
        int foundVarCount;

        AstNode* allocateNode();
        void  skipSpaces();
        char peekChar();
        char consumeChar();
        bool matchImplicationArrow();

        AstNode* parseEquality();
        AstNode* parseImplication();
        AstNode* parseOr();
        AstNode* parseAnd();
        AstNode* parseNot();
        AstNode* parseAtom();

        void registerVariable(char variable);
        bool isVariableKnown(char variable) const;
        
    public:
        Parser();
        AstNode* parse(const char* formula);
        int  getVariableCount() const;
        void getVariables(char outVars[MAX_VARIABLES]) const;
    };

}