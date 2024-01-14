#include <iostream> 

#include "../include/Resolver.h"
#include "../include/Lox.h"

Resolver::Resolver(Interpreter &interpreter) 
    : interpreter(interpreter) {}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> &statements) {
    for (const std::shared_ptr<Stmt> &statement : statements) {
        resolve(statement);
    }
}

void Resolver::resolve(std::shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
}

void Resolver::resolve(std::shared_ptr<Expr> expr) {
    expr->accept(*this);
}

void Resolver::visit(std::shared_ptr<BlockStmt> stmt) {
    beginScope();
    resolve(stmt->statements);
    endScope();
}

void Resolver::visit(std::shared_ptr<ExpressionStmt> stmt) {
    resolve(stmt->expr);
}

void Resolver::visit(std::shared_ptr<FunctionStmt> stmt) {
    declare(stmt->name);
    define(stmt->name);

    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visit(std::shared_ptr<IfStmt> stmt) {
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch != nullptr) resolve(stmt->elseBranch);
}

void Resolver::visit(std::shared_ptr<PrintStmt> stmt) {
    resolve(stmt->expr);
}

void Resolver::visit(std::shared_ptr<ReturnStmt> stmt) {
    if (currentFunction == FunctionType::NONE) {
        Lox::error(stmt->keyword, "Can't return from top-level code.");
    }

    if (stmt->value != nullptr) {
        resolve(stmt->value);
    }
}

void Resolver::visit(std::shared_ptr<VarStmt> stmt) {
    declare(stmt->name);
    if (stmt->initializer != nullptr) {
        resolve(stmt->initializer);
    }
    define(stmt->name);
}

void Resolver::visit(std::shared_ptr<WhileStmt> stmt) {
    resolve(stmt->condition);
    resolve(stmt->body);
}

std::any Resolver::visit(std::shared_ptr<AssignExpr> expr) {
    resolve(expr->value);
    resolveLocal(expr, expr->name);
    return {};
}

std::any Resolver::visit(std::shared_ptr<BinaryExpr> expr) {
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

std::any Resolver::visit(std::shared_ptr<CallExpr> expr) {
    resolve(expr->callee);

    for (const std::shared_ptr<Expr> &argument : expr->arguments) {
        resolve(argument);
    }
    
    return {};
}

std::any Resolver::visit(std::shared_ptr<GroupingExpr> expr) {
    resolve(expr->expression);
    return {};
}

std::any Resolver::visit(std::shared_ptr<LiteralExpr> expr) {
    return {};
}

std::any Resolver::visit(std::shared_ptr<LogicalExpr> expr) {
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

std::any Resolver::visit(std::shared_ptr<UnaryExpr> expr) {
    resolve(expr->right);
    return {};
}

std::any Resolver::visit(std::shared_ptr<VariableExpr> expr) {
    if (!scopes.empty()) {
        std::map<std::string, bool>& scope = scopes.back();
        auto element = scope.find(expr->name.lexeme);
        if (element != scope.end() && element->second == false) {
            Lox::error(expr->name,
                "Can't read local variable in its own initializer.");
        }
    }
    
    resolveLocal(expr, expr->name);
    return {};
}

void Resolver::beginScope() {
    scopes.push_back(std::map<std::string, bool>{});
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token &name) {
    if (scopes.empty()) return;

    std::map<std::string, bool>& scope = scopes.back();
    if (scope.find(name.lexeme) != scope.end()) {
        Lox::error(name, "Already a variable with this name in this scope.");
    }

    scope[name.lexeme] = false;
}

void Resolver::define(const Token &name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme] = true;
}

void Resolver::resolveLocal(std::shared_ptr<Expr> expr, Token &name) {
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].find(name.lexeme) != scopes[i].end()) {
            interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(
        std::shared_ptr<FunctionStmt> function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const Token &param : function->parameters) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();
    currentFunction = enclosingFunction;
}