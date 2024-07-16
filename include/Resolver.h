#pragma once

#include "Expr.h"
#include "Interpreter.h"
#include "Lox.h"
#include "Stmt.h"

class Resolver : public ExprVisitor, public StmtVisitor {
 public:
    Resolver(Interpreter &interpreter);
    void resolve(const std::vector<std::shared_ptr<Stmt>> &statements);

    void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
    void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
    void visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
    void visitIfStmt(std::shared_ptr<IfStmt> stmt) override;
    void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
    void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
    void visitVarStmt(std::shared_ptr<VarStmt> stmt) override;
    void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;

    std::any visitAssignExpr(std::shared_ptr<AssignExpr> expr) override;
    std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) override;
    std::any visitCallExpr(std::shared_ptr<CallExpr> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) override;
    std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) override;
    std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) override;

 private:
    enum class FunctionType {
        NONE,
        FUNCTION,
    };

    FunctionType currentFunction = FunctionType::NONE;
    std::vector<std::map<std::string, bool>> scopes;
    Interpreter &interpreter;

    void resolve(std::shared_ptr<Stmt> stmt);
    void resolve(std::shared_ptr<Expr> expr);
    void resolveFunction(std::shared_ptr<FunctionStmt> function, FunctionType type);
    void resolveLocal(std::shared_ptr<Expr> expr, const Token &name);
    void beginScope();
    void endScope();
    void declare(const Token &name);
    void define(const Token &name);
};
