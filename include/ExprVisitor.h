#ifndef __VISITOR_H
#define __VISITOR_H

#include <any>

class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

template<typename T>
class ExprVisitor {
public:
    ExprVisitor() = default;

    virtual T visit(const BinaryExpr& expr) = 0;
    virtual T visit(const GroupingExpr& expr) = 0;
    virtual T visit(const LiteralExpr& expr) = 0;
    virtual T visit(const UnaryExpr& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor<std::any> &visitor) = 0;
};

#endif /* __VISITOR_H */
