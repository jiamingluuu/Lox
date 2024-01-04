#ifndef __STMT_H
#define __STMT_H

#include <algorithm>
#include <any>

#include "Expr.h"

class BlockStmt;
class IfStmt;
class ExpressionStmt;
class PrintStmt;
class VarStmt;

template<typename T>
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;

    virtual T visit(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual T visit(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual T visit(std::shared_ptr<IfStmt> stmt) = 0; 
    virtual T visit(std::shared_ptr<PrintStmt> stmt) = 0;
    virtual T visit(std::shared_ptr<VarStmt> stmt) = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor<void> &visitor) = 0;
};

class BlockStmt : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor<void> &visitor) override { 
        visitor.visit(std::make_shared<BlockStmt>(*this)); 
        return {};
    }
};

class ExpressionStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;

    ExpressionStmt(std::shared_ptr<Expr> expr)
        : expr(std::move(expr))
    {
        assert(expr != nullptr);
    }

    std::any accept(StmtVisitor<void> &visitor) override { 
        visitor.visit(std::make_shared<ExpressionStmt>(*this)); 
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

    std::any accept(StmtVisitor<void> &visitor) override { 
        visitor.visit(std::make_shared<IfStmt>(*this)); 
        return {};
    }
};

class PrintStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;

    PrintStmt(std::shared_ptr<Expr> expr)
        : expr(std::move(expr)) {}

    std::any accept(StmtVisitor<void> &visitor) override { 
        visitor.visit(std::make_shared<PrintStmt>(*this)); 
        return {};
    }
};

class VarStmt : public Stmt {
public:
    Token name;
    std::shared_ptr<Expr> initializer;

    VarStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor<void> &visitor) override { 
        visitor.visit(std::make_shared<VarStmt>(*this)); 
        return {};
    }
};

#endif /* __STMT_H */
