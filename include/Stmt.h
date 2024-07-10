#ifndef __STMT_H
#define __STMT_H

#include <algorithm>
#include <any>

#include "Expr.h"

class BlockStmt;
class ExpressionStmt;
class FunctionStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class WhileStmt;
class VarStmt;

class StmtVisitor {
public:
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

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor &visitor) = 0;
};

class BlockStmt : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitBlockStmt(std::make_shared<BlockStmt>(*this)); 
        return {};
    }
};

class ExpressionStmt : public Stmt {
public:
    std::shared_ptr<Expr> expression;

    ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression))
    {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitExpressionStmt(std::make_shared<ExpressionStmt>(*this)); 
        return {};
    }
};

class FunctionStmt : public Stmt {
public:
    Token name;
    std::vector<Token> parameters;
    std::vector<std::shared_ptr<Stmt>> body;

    FunctionStmt(
        Token name, 
        std::vector<Token> parameters, 
        std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)),
          parameters(std::move(parameters)),
          body(std::move(body)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitFunctionStmt(std::make_shared<FunctionStmt>(*this)); 
        return {};
    }
};

class IfStmt : public Stmt {
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;

    IfStmt(std::shared_ptr<Expr> condition, 
           std::shared_ptr<Stmt> thenBranch,
           std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), 
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitIfStmt(std::make_shared<IfStmt>(*this)); 
        return {};
    }
};

class PrintStmt : public Stmt {
public:
    std::shared_ptr<Expr> expression;

    PrintStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitPrintStmt(std::make_shared<PrintStmt>(*this)); 
        return {};
    }
};

class ReturnStmt : public Stmt {
public:
    Token keyword;
    std::shared_ptr<Expr> value;

    ReturnStmt(Token keyword, std::shared_ptr<Expr> value) 
    : keyword(std::move(keyword)), value(std::move(value)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitReturnStmt(std::make_shared<ReturnStmt>(*this)); 
        return {};
    }
};

class WhileStmt : public Stmt {
public: 
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitWhileStmt(std::make_shared<WhileStmt>(*this)); 
        return {};
    }
};

class VarStmt : public Stmt {
public:
    Token name;
    std::shared_ptr<Expr> initializer;

    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor &visitor) override { 
        visitor.visitVarStmt(std::make_shared<VarStmt>(*this)); 
        return {};
    }
};

#endif /* __STMT_H */
