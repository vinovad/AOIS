#include "parser.h"

namespace BoolLogic {
    AstNode::AstNode()
        : nodeKind(NodeKind::Variable),
          variableName(CHAR_NULL),
          leftChild(nullptr),
          rightChild(nullptr) {
    }

    void AstNode::setKind(NodeKind kind)  { nodeKind  = kind; }
    void AstNode::setVariable(char variable) { variableName = variable; }
    void AstNode::setLeft(AstNode* node) { leftChild  = node; }
    void AstNode::setRight(AstNode* node) { rightChild = node; }

    NodeKind AstNode::getKind() const { return nodeKind; }
    char AstNode::getVariable() const { return variableName; }
    AstNode* AstNode::getLeft() const { return leftChild; }
    AstNode* AstNode::getRight() const { return rightChild; }

    bool AstNode::isLeaf() const {
        return nodeKind == NodeKind::Variable;
    }

    bool AstNode::isBinary() const {
        return nodeKind == NodeKind::OperatorAnd
            || nodeKind == NodeKind::OperatorOr
            || nodeKind == NodeKind::OperatorImpl
            || nodeKind == NodeKind::OperatorEq;
    }

    Parser::Parser() : source(nullptr), position(0), poolSize(0), foundVarCount(0) {}

    AstNode* Parser::allocateNode() {
        if (poolSize >= MAX_AST_NODES) {
            throw std::runtime_error("AST node pool exceeded");
        }
        nodePool[poolSize] = AstNode();
        return &nodePool[poolSize++];
    }

    void Parser::skipSpaces() {
        while (source[position] == CHAR_SPACE) {
            position++;
        }
    }

    char Parser::peekChar() {
        skipSpaces();
        return source[position];
    }

    char Parser::consumeChar() {
        skipSpaces();
        return source[position++];
    }

    bool Parser::matchImplicationArrow() {
        skipSpaces();
        bool isDash  = source[position] == OP_IMPL_DASH;
        bool isArrow = source[position + 1] == OP_IMPL_ARROW;
        if (isDash && isArrow) {
            position += 2;
            return true;
        }
        return false;
    }

    void Parser::registerVariable(char variable) {
        if (isVariableKnown(variable)) {
            return;
        }
        if (foundVarCount < MAX_VARIABLES) {
            foundVars[foundVarCount++] = variable;
        }
    }

    bool Parser::isVariableKnown(char variable) const {
        for (int index = 0; index < foundVarCount; index++) {
            if (foundVars[index] == variable) {
                return true;
            }
        }
        return false;
    }

    AstNode* Parser::parse(const char* formula) {
        source = formula;
        position = 0;
        poolSize = 0;
        foundVarCount = 0;

        AstNode* root = parseEquality();
        skipSpaces();

        if (source[position] != CHAR_NULL) {
            throw std::runtime_error("Неожиданный символ в формуле");
        }
        return root;
    }

    AstNode* Parser::parseEquality() {
        AstNode* leftNode = parseImplication();

        while (peekChar() == OP_EQ) {
            consumeChar();
            AstNode* rightNode = parseImplication();
            AstNode* eqNode = allocateNode();
            eqNode->setKind(NodeKind::OperatorEq);
            eqNode->setLeft(leftNode);
            eqNode->setRight(rightNode);
            leftNode = eqNode;
        }
        return leftNode;
    }

    AstNode* Parser::parseImplication() {
        AstNode* leftNode = parseOr();

        if (!matchImplicationArrow()) {
            return leftNode;
        }
        AstNode* rightNode = parseImplication();
        AstNode* implNode  = allocateNode();
        implNode->setKind(NodeKind::OperatorImpl);
        implNode->setLeft(leftNode);
        implNode->setRight(rightNode);
        return implNode;
    }

    AstNode* Parser::parseOr() {
        AstNode* leftNode = parseAnd();

        while (peekChar() == OP_OR) {
            consumeChar();
            AstNode* rightNode = parseAnd();
            AstNode* orNode = allocateNode();
            orNode->setKind(NodeKind::OperatorOr);
            orNode->setLeft(leftNode);
            orNode->setRight(rightNode);
            leftNode = orNode;
        }
        return leftNode;
    }

    AstNode* Parser::parseAnd() {
        AstNode* leftNode = parseNot();

        while (peekChar() == OP_AND) {
            consumeChar();
            AstNode* rightNode = parseNot();
            AstNode* andNode = allocateNode();
            andNode->setKind(NodeKind::OperatorAnd);
            andNode->setLeft(leftNode);
            andNode->setRight(rightNode);
            leftNode = andNode;
        }
        return leftNode;
    }

    AstNode* Parser::parseNot() {
        if (peekChar() != OP_NOT) {
            return parseAtom();
        }
        consumeChar();
        AstNode* operand = parseNot();
        AstNode* notNode = allocateNode();
        notNode->setKind(NodeKind::OperatorNot);
        notNode->setLeft(operand);
        return notNode;
    }

    AstNode* Parser::parseAtom() {
        char current = peekChar();

        if (current == PAREN_OPEN) {
            consumeChar();
            AstNode* inner = parseEquality();
            if (peekChar() != PAREN_CLOSE) {
                throw std::runtime_error("Ожидается закрывающая скобка");
            }
            consumeChar();
            return inner;
        }

        if (current >= VAR_MIN && current <= VAR_MAX) {
            consumeChar();
            registerVariable(current);
            AstNode* varNode = allocateNode();
            varNode->setKind(NodeKind::Variable);
            varNode->setVariable(current);
            return varNode;
        }

        throw std::runtime_error("Неожиданный символ в формуле");
    }

    int Parser::getVariableCount() const {
        return foundVarCount;
    }

    void Parser::getVariables(char outVars[MAX_VARIABLES]) const {
        for (int index = 0; index < foundVarCount; index++) {
            outVars[index] = foundVars[index];
        }
    }


}