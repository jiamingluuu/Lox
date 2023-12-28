#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <any>
#include <memory>

#include "Expr.h"
#include "ExprVisitor.h"

class Interpreter : public ExprVisitor<std::any> {
public:
    void interprete(std::shared_ptr<Expr> expr);
    std::any visit(std::shared_ptr<BinaryExpr> expr) override;
    std::any visit(std::shared_ptr<LiteralExpr> expr) override;
    std::any visit(std::shared_ptr<GroupingExpr> expr) override;
    std::any visit(std::shared_ptr<UnaryExpr> expr) override;

private:
    std::any evaluate(std::shared_ptr<Expr> expr);
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);

    void checkNumberOperand(Token op, const std::any& operand);
    void checkNumberOperand(Token op, const std::any& left, 
            const std::any& right);

    std::string stringify(const std::any& object);
};

#endif /* __INTERPRETER_H */
