#ifndef __RESOLVER_H
#define __RESOLVER_H

#include "Lox.h"
#include "Expr.h"
#include "Interpreter.h"
#include "Stmt.h"

class Resolver : public ExprVisitor<std::any>, public StmtVisitor<void> {
public:
    Resolver(Interpreter &interpreter);
    void resolve(std::vector<std::shared_ptr<Stmt>> &statements);

    void visit(std::shared_ptr<BlockStmt> stmt) override;
    void visit(std::shared_ptr<ExpressionStmt> stmt) override;
    void visit(std::shared_ptr<FunctionStmt> stmt) override;
    void visit(std::shared_ptr<IfStmt> stmt) override;
    void visit(std::shared_ptr<PrintStmt> stmt) override;
    void visit(std::shared_ptr<ReturnStmt> stmt) override;
    void visit(std::shared_ptr<VarStmt> stmt) override;
    void visit(std::shared_ptr<WhileStmt> stmt) override;
    
    std::any visit(std::shared_ptr<AssignExpr> expr) override;
    std::any visit(std::shared_ptr<BinaryExpr> expr) override;
    std::any visit(std::shared_ptr<CallExpr> expr) override;
    std::any visit(std::shared_ptr<GroupingExpr> expr) override;
    std::any visit(std::shared_ptr<LiteralExpr> expr) override;
    std::any visit(std::shared_ptr<LogicalExpr> expr) override;
    std::any visit(std::shared_ptr<UnaryExpr> expr) override;
    std::any visit(std::shared_ptr<VariableExpr> expr) override;

private: 
    enum class FunctionType {
        NONE,
        FUNCTION,
    };
    
    FunctionType currentFunction = FunctionType::NONE;
    std::vector<std::map<std::string, bool>> scopes;
    Interpreter& interpreter;

    void resolve(std::shared_ptr<Stmt> stmt);
    void resolve(std::shared_ptr<Expr> expr);
    void resolveFunction(
        std::shared_ptr<FunctionStmt> function, 
        FunctionType type);
    void resolveLocal(std::shared_ptr<Expr> expr, Token &name);
    void beginScope();
    void endScope();
    void declare(const Token &name);
    void define(const Token &name);
};

#endif /* __RESOLVER_H */