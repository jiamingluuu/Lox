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

    virtual T visit(std::shared_ptr<BinaryExpr> expr) = 0;
    virtual T visit(std::shared_ptr<GroupingExpr> expr) = 0;
    virtual T visit(std::shared_ptr<LiteralExpr> expr) = 0;
    virtual T visit(std::shared_ptr<UnaryExpr> expr) = 0;
};

#endif /* __VISITOR_H */
