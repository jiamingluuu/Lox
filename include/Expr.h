#ifndef __EXPR_H
#define __EXPR_H

#include <vector>

#include "ExprVisitor.h"
#include "Token.h"

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) 
        : left(std::move(left)), op(op), right(std::move(right))
    {
        assert(this->left != nullptr);
        assert(this->right != nullptr);
    }

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(*this); }
};

class GroupingExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;

    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression))
    {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(*this); }
};

class LiteralExpr : public Expr {
public: 
    std::any literal;

    LiteralExpr(std::any literal)
        : literal(std::move(literal))
    {}

    std::any accept(ExprVisitor<std::any>& visitor) override
    { return visitor.visit(*this); }
};

class UnaryExpr : public Expr {
public: 
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right))
    {
        assert(this->right != nullptr);
    }
    
    std::any accept(ExprVisitor<std::any>& visitor) override
    { return visitor.visit(*this); }
};

#endif /* __EXPR_H */
