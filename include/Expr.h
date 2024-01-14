#ifndef __EXPR_H
#define __EXPR_H

#include <any>
#include <vector>

#include "Token.h"

class AssignExpr;
class BinaryExpr;
class CallExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class UnaryExpr;
class VariableExpr;

template<typename T>
class ExprVisitor {
public:
    virtual T visit(std::shared_ptr<AssignExpr> expr) = 0;
    virtual T visit(std::shared_ptr<BinaryExpr> expr) = 0;
    virtual T visit(std::shared_ptr<CallExpr> expr) = 0;
    virtual T visit(std::shared_ptr<GroupingExpr> expr) = 0;
    virtual T visit(std::shared_ptr<LiteralExpr> expr) = 0;
    virtual T visit(std::shared_ptr<LogicalExpr> expr) = 0;
    virtual T visit(std::shared_ptr<UnaryExpr> expr) = 0;
    virtual T visit(std::shared_ptr<VariableExpr> expr) = 0;
    virtual ~ExprVisitor() = default;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any> &visitor) = 0;
};

class AssignExpr : public Expr {
public: 
    Token name;
    std::shared_ptr<Expr> value;

    AssignExpr(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<AssignExpr>(*this)); }
};

class BinaryExpr : public Expr {
public:
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    BinaryExpr(
            std::shared_ptr<Expr> left, 
            Token op, 
            std::shared_ptr<Expr> right) 
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<BinaryExpr>(*this)); }
};

class CallExpr: public Expr {
public:
    std::shared_ptr<Expr> callee;
    Token paren;
    std::vector<std::shared_ptr<Expr>> arguments;

    CallExpr(
        std::shared_ptr<Expr> callee,
        Token paren,
        std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), 
          paren(std::move(paren)), 
          arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<CallExpr>(*this)); }
};

class GroupingExpr : public Expr {
public:
    std::shared_ptr<Expr> expression;

    GroupingExpr(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<GroupingExpr>(*this)); }
};

class LiteralExpr : public Expr {
public: 
    std::any literal;

    LiteralExpr(std::any literal)
        : literal(std::move(literal)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<LiteralExpr>(*this)); }
};

class LogicalExpr : public Expr {
public: 
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    LogicalExpr(
            std::shared_ptr<Expr> left, 
            Token op, 
            std::shared_ptr<Expr> right) 
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<LogicalExpr>(*this)); }
};

class UnaryExpr : public Expr {
public: 
    Token op;
    std::shared_ptr<Expr> right;

    UnaryExpr(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    
    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<UnaryExpr>(*this)); }
};

class VariableExpr : public Expr {
public:
    Token name;

    VariableExpr(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor<std::any> &visitor) override
    { return visitor.visit(std::make_shared<VariableExpr>(*this)); }
};

#endif /* __EXPR_H */
