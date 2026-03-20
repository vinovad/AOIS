#include "boolean_function.h"

namespace BoolLogic {
    bool BooleanFunction::lookupVariable(
        char variableName,
        const char variables[MAX_VARIABLES],
        const bool values[MAX_VARIABLES],
        int variableCount
    ) const {
        for (int index = 0; index < variableCount; index++) {
            if (variables[index] == variableName) {
                return values[index];
            }
        }
        throw std::runtime_error("Неизвестная переменная в выражении");
    }

    bool BooleanFunction::applyBinaryOperator( NodeKind kind, bool leftValue, bool rightValue ) const {
        if (kind == NodeKind::OperatorAnd)  return leftValue && rightValue;
        if (kind == NodeKind::OperatorOr)   return leftValue || rightValue;
        if (kind == NodeKind::OperatorImpl) return !leftValue || rightValue;
        if (kind == NodeKind::OperatorEq)   return leftValue == rightValue;
        throw std::runtime_error("Неизвестный бинарный оператор");
    }

    bool BooleanFunction::evalNode(
        const AstNode* node,
        const char variables[MAX_VARIABLES],
        const bool values[MAX_VARIABLES],
        int variableCount
    ) const {
        if (node->getKind() == NodeKind::Variable) {
            return lookupVariable( 
                node->getVariable(), variables, values, variableCount
            );
        }
        if (node->getKind() == NodeKind::OperatorNot) {
            return !evalNode(node->getLeft(), variables, values, variableCount);
        }
        bool leftValue  = evalNode(node->getLeft(),  variables, values, variableCount);
        bool rightValue = evalNode(node->getRight(), variables, values, variableCount);
        return applyBinaryOperator(node->getKind(), leftValue, rightValue);
    }

    bool BooleanFunction::evaluate(
        const AstNode* root,
        const char variables[MAX_VARIABLES],
        const bool values[MAX_VARIABLES],
        int variableCount
    ) const {
        return evalNode(root, variables, values, variableCount);
    }

}