#ifndef __EXPR_H
#define __EXPR_H

#include <vector>

#include "ExprVisitor.h"
#include "Token.h"

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any> &visitor) const = 0;
};

class BinaryExpr : public Expr {
public:
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) 
        : left(std::move(left)), op(op), right(std::move(right))
    {
        assert(this->left != nullptr);
        assert(this->right != nullptr);
    }

    std::any accept(ExprVisitor<std::any>& visitor) const override
    { return visitor.visit(std::make_shared<BinaryExpr>(*this)); }
};

class GroupingExpr : public Expr {
public:
    std::shared_ptr<Expr> expression;

    GroupingExpr(std::shared_ptr<Expr> expression)
        : expression(std::move(expression))
    {}

    std::any accept(ExprVisitor<std::any>& visitor) const override
    { return visitor.visit(std::make_shared<GroupingExpr>(*this)); }
};

class LiteralExpr : public Expr {
public: 
    std::any literal;

    LiteralExpr(std::any literal)
        : literal(std::move(literal))
    {}

    std::any accept(ExprVisitor<std::any>& visitor) const override
    { return visitor.visit(std::make_shared<LiteralExpr>(*this)); }
};

class UnaryExpr : public Expr {
public: 
    Token op;
    std::shared_ptr<Expr> right;

    UnaryExpr(Token op, std::shared_ptr<Expr> right)
        : op(op), right(std::move(right))
    {
        assert(this->right != nullptr);
    }
    
    std::any accept(ExprVisitor<std::any>& visitor) const override
    { return visitor.visit(std::make_shared<UnaryExpr>(*this)); }
};

#endif /* __EXPR_H */
