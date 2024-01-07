#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <any>
#include <chrono>
#include <memory>

#include "Environment.h"
#include "Expr.h"
#include "LoxCallable.h"
#include "Stmt.h"

class Clock : public LoxCallable {
public:
    int arity() override { return 0; }

    std::any call(
            Interpreter& interpreter, 
            std::vector<std::any> arguments) override {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration<double>{now}.count() / 1000.0;
    }

    std::string toString() override { 
        return "<native fn>"; 
    }
};

class Interpreter : public ExprVisitor<std::any>, 
                    public StmtVisitor<void> {
public:
    std::shared_ptr<Environment> globals{new Environment};
    Interpreter() {
        globals->define("clock", std::shared_ptr<Clock>{});
    }
    void interprete(std::vector<std::shared_ptr<Stmt>> statements);

    std::any visit(std::shared_ptr<AssignExpr> expr) override;
    std::any visit(std::shared_ptr<BinaryExpr> expr) override;
    std::any visit(std::shared_ptr<CallExpr> expr) override;
    std::any visit(std::shared_ptr<LiteralExpr> expr) override;
    std::any visit(std::shared_ptr<LogicalExpr> expr) override;
    std::any visit(std::shared_ptr<GroupingExpr> expr) override;
    std::any visit(std::shared_ptr<UnaryExpr> expr) override;
    std::any visit(std::shared_ptr<VariableExpr> expr) override;

    void visit(std::shared_ptr<BlockStmt> stmt) override;
    void visit(std::shared_ptr<ExpressionStmt> stmt) override;
    void visit(std::shared_ptr<FunctionStmt> stmt) override;
    void visit(std::shared_ptr<IfStmt> stmt) override;
    void visit(std::shared_ptr<PrintStmt> stmt) override;
    void visit(std::shared_ptr<ReturnStmt> stmt) override;
    void visit(std::shared_ptr<WhileStmt> stmt) override;
    void visit(std::shared_ptr<VarStmt> stmt) override;

    void executeBlock(
            std::vector<std::shared_ptr<Stmt>> statements, 
            std::shared_ptr<Environment> environment);

private:
    std::shared_ptr<Environment> environment = globals;
    std::any evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);

    void checkNumberOperand(Token op, const std::any& operand);
    void checkNumberOperand(Token op, const std::any& left, 
            const std::any& right);

    std::string stringify(const std::any& object);
};

#endif /* __INTERPRETER_H */
