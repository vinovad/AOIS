#pragma once
#include "parser.h"

namespace BoolLogic {

    class BooleanFunction {
    
    private:
        bool evalNode(
            const AstNode* node,
            const char variables[MAX_VARIABLES],
            const bool values[MAX_VARIABLES],
            int variableCount
        ) const;

        bool lookupVariable(
            char variableName,
            const char variables[MAX_VARIABLES],
            const bool values[MAX_VARIABLES],
            int variableCount
        ) const;

        bool applyBinaryOperator(
            NodeKind kind,
            bool leftValue,
            bool rightValue
        ) const;
    
    public:
        bool evaluate(
            const AstNode* root,
            const char  variables[MAX_VARIABLES],
            const bool values[MAX_VARIABLES],
            int variableCount
        ) const;

    };

}