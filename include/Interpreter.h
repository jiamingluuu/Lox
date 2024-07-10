#pragma once

#include <any>
#include <chrono>
#include <memory>

#include "Environment.h"
#include "Expr.h"
#include "LoxCallable.h"
#include "Stmt.h"

class Clock : public LoxCallable {
 public:
    int arity() override {
        return 0;
    }

    std::any call(Interpreter &interpreter, std::vector<std::any> arguments) override {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        return std::chrono::duration<double>{now}.count() / 1000.0;
    }

    std::string toString() override {
        return "<native fn>";
    }
};

class Interpreter : public ExprVisitor, public StmtVisitor {
 public:
    std::shared_ptr<Environment> globals{new Environment};
    Interpreter();
    void interpret(const std::vector<std::shared_ptr<Stmt>> &statements);
    std::any visitAssignExpr(std::shared_ptr<AssignExpr> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
    std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
    std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;
    void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
    void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
    void visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
    void visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
    void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
    void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;
    void visitVarStmt(std::shared_ptr<VarStmt> stmt) override;
    void executeBlock(const std::vector<std::shared_ptr<Stmt>> &statements, std::shared_ptr<Environment> environment);
    void resolve(std::shared_ptr<Expr> expr, int depth);

 private:
    std::shared_ptr<Environment> environment = globals;
    std::map<std::shared_ptr<Expr>, int> locals;
    std::any evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);
    bool isTruthy(const std::any &object);
    bool isEqual(const std::any &a, const std::any &b);
    std::any lookUpVariable(const Token &name, std::shared_ptr<Expr> expr);
    void checkNumberOperand(const Token &op, const std::any &operand);
    void checkNumberOperands(const Token &op, const std::any &left, const std::any &right);

    std::string stringify(const std::any &object);
};