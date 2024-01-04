#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <any>
#include <memory>

#include "Environment.h"
#include "Expr.h"
#include "Stmt.h"

class Interpreter : public ExprVisitor<std::any>, 
                    public StmtVisitor<void> {
public:
    void interprete(std::vector<std::shared_ptr<Stmt>> statements);

    std::any visit(std::shared_ptr<AssignExpr> expr) override;
    std::any visit(std::shared_ptr<BinaryExpr> expr) override;
    std::any visit(std::shared_ptr<LiteralExpr> expr) override;
    std::any visit(std::shared_ptr<GroupingExpr> expr) override;
    std::any visit(std::shared_ptr<UnaryExpr> expr) override;
    std::any visit(std::shared_ptr<VariableExpr> expr) override;

    void visit(std::shared_ptr<BlockStmt> stmt) override;
    void visit(std::shared_ptr<ExpressionStmt> stmt) override;
    void visit(std::shared_ptr<PrintStmt> stmt) override;
    void visit(std::shared_ptr<VarStmt> stmt) override;

private:
    static std::shared_ptr<Environment> environment;
    std::any evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);
    void executeBlock(
            std::vector<std::shared_ptr<Stmt>> statements, 
            std::shared_ptr<Environment> environment);
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);

    void checkNumberOperand(Token op, const std::any& operand);
    void checkNumberOperand(Token op, const std::any& left, 
            const std::any& right);

    std::string stringify(const std::any& object);
};

#endif /* __INTERPRETER_H */
