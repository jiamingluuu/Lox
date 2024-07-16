#pragma once

#include <algorithm>
#include <any>

#include "Expr.h"

struct BlockStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct PrintStmt;
struct ReturnStmt;
struct WhileStmt;
struct VarStmt;

struct StmtVisitor {
    virtual ~StmtVisitor() = default;

    virtual void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual void visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) = 0;
    virtual void visitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;
    virtual void visitPrintStmt(std::shared_ptr<PrintStmt> stmt) = 0;
    virtual void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;
    virtual void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual void visitVarStmt(std::shared_ptr<VarStmt> stmt) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor &visitor) = 0;
};

struct BlockStmt : public Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitBlockStmt(std::make_shared<BlockStmt>(*this));
    }
};

struct ExpressionStmt : public Stmt {
    std::shared_ptr<Expr> expression;

    ExpressionStmt(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitExpressionStmt(std::make_shared<ExpressionStmt>(*this));
    }
};

struct FunctionStmt : public Stmt {
    Token name;
    std::vector<Token> parameters;
    std::vector<std::shared_ptr<Stmt>> body;

    FunctionStmt(Token name, std::vector<Token> parameters, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitFunctionStmt(std::make_shared<FunctionStmt>(*this));
    }
};

struct IfStmt : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;

    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitIfStmt(std::make_shared<IfStmt>(*this));
    }
};

struct PrintStmt : public Stmt {
    std::shared_ptr<Expr> expression;

    PrintStmt(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitPrintStmt(std::make_shared<PrintStmt>(*this));
    }
};

struct ReturnStmt : public Stmt {
    Token keyword;
    std::shared_ptr<Expr> value;

    ReturnStmt(Token keyword, std::shared_ptr<Expr> value) : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitReturnStmt(std::make_shared<ReturnStmt>(*this));
    }
};

struct WhileStmt : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitWhileStmt(std::make_shared<WhileStmt>(*this));
    }
};

struct VarStmt : public Stmt {
    Token name;
    std::shared_ptr<Expr> initializer;

    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor &visitor) override {
        visitor.visitVarStmt(std::make_shared<VarStmt>(*this));
    }
};
