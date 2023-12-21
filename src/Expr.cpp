#include "../include/Expr.h"

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
    : left{std::move(left)}, op{std::move(op)}, right{std::move(right)}
{
    assert(this->left != nullptr);
    assert(this->right != nullptr);
}

std::any BinaryExpr::accept(ExprVisitor<std::any> &visitor)
{
    return visitor.visit(*this);
}

UnaryExpr::UnaryExpr(Token op, std::unique_ptr<Expr> right)
    : op{std::move(op)}, right{std::move(right)}
{
    assert(this->right != nullptr);
}

std::any UnaryExpr::accept(ExprVisitor<std::any>& visitor)
{
    return visitor.visit(*this);
}
