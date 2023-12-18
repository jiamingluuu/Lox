#ifndef __EXPR_H
#define __EXPR_H

#include <any>

#include "ExprVisitor.h"
class Expr {
    virtual ~Expr() = default;

    virtual std::any accept(ExprVisitor<std::any>& visitor) const = 0;
};

#endif /* __EXPR_H */
