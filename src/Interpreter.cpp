#include <iostream>

#include "../include/Environment.h"
#include "../include/Interpreter.h"
#include "../include/Lox.h"
#include "../include/LoxCallable.h"
#include "../include/LoxFunction.h"
#include "../include/LoxReturn.h"
#include "../include/RuntimeError.h"

void Interpreter::interpret(
        const std::vector<std::shared_ptr<Stmt>> &statements) {
    try {
        for (const std::shared_ptr<Stmt> &statement : statements) {
            execute(statement);
        }
    } catch (RuntimeError error) {
        Lox::runtimeError(error);
    }
}

std::any Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
}

void Interpreter::executeBlock(
        const std::vector<std::shared_ptr<Stmt>> &statements,
        std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = environment;
        for (const std::shared_ptr<Stmt> &statement : statements) {
            execute(statement);
        }
    } catch(...) {
        this->environment = previous;
        throw;
    }

    this->environment = previous;
}

void Interpreter::resolve(std::shared_ptr<Expr> expr, int depth) {
    locals[expr] = depth;
}

void Interpreter::visit(std::shared_ptr<BlockStmt> stmt) {
    executeBlock(stmt->statements, std::make_shared<Environment>(environment));
}

void Interpreter::visit(std::shared_ptr<ExpressionStmt> stmt) {
    evaluate(stmt->expr);
}

void Interpreter::visit(std::shared_ptr<FunctionStmt> stmt) {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(
            stmt, environment);
    environment->define(stmt->name.lexeme, function);
}

void Interpreter::visit(std::shared_ptr<IfStmt> stmt) {
    if (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt-> elseBranch != nullptr) {
        execute(stmt->elseBranch);
    }
}

void Interpreter::visit(std::shared_ptr<PrintStmt> stmt) {
    std::any value = evaluate(stmt->expr);
    std::cout << stringify(value) << "\n";
}

void Interpreter::visit(std::shared_ptr<ReturnStmt> stmt) {
    std::any value = nullptr;
    if (stmt->value != nullptr) value = evaluate(stmt->value);
    
    throw LoxReturn{value};
}

void Interpreter::visit(std::shared_ptr<WhileStmt> stmt) {
    while (isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
}

void Interpreter::visit(std::shared_ptr<VarStmt> stmt) {
    std::any value = nullptr;
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }

    environment->define(stmt->name.lexeme, std::move(value));
}

std::any Interpreter::visit(std::shared_ptr<AssignExpr> expr) {
    std::any value = evaluate(expr->value);
    
    auto element = locals.find(expr);
    if (element != locals.end()) {
        int distance = element->second;
        environment->assignAt(distance, expr->name, value);
    } else {
        globals->assign(expr->name, value);
    }

    return value;
}

std::any Interpreter::visit(std::shared_ptr<BinaryExpr> expr) {
    std::any left = evaluate(expr->left);
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
        case GREATER:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right); 
        case GREATER_EQUAL:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right); 
        case LESS:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right); 
        case LESS_EQUAL:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right); 
        case MINUS:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right); 
        case PLUS:
            if ((left.type() == typeid(double)) && 
                    (right.type() == typeid(double))) {

                return std::any_cast<double>(left) 
                    + std::any_cast<double>(right); 
            }

            if ((left.type() == typeid(std::string)) && 
                    (right.type() == typeid(std::string))) {
                
                return std::any_cast<std::string>(left) 
                    + std::any_cast<std::string>(right); 
            }

            throw RuntimeError(expr->op, 
                    "Operands must be two numbers or two strings.");
        case BANG_EQUAL:
            checkNumberOperand(expr->op, left, right);
            return !isEqual(left, right);
        case EQUAL:
            checkNumberOperand(expr->op, left, right);
            return isEqual(left, right);
        case SLASH:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right); 
        case STAR:
            checkNumberOperand(expr->op, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right); 
        default:
            return {};
    }

    return {};
}

std::any Interpreter::visit(std::shared_ptr<CallExpr> expr) {
    std::any callee = evaluate(expr->callee);

    std::vector<std::any>  arguments;
    for (const std::shared_ptr<Expr> &argument : expr->arguments) {
        arguments.push_back(evaluate(argument));
    }

    std::shared_ptr<LoxCallable> function;
    if (callee.type() != typeid(std::shared_ptr<LoxFunction>)) {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    } 

    function = std::any_cast<std::shared_ptr<LoxFunction>>(callee);

    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr->paren,
                "Expected " + std::to_string(function->arity()) + 
                " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, arguments);
}

std::any Interpreter::visit(std::shared_ptr<GroupingExpr> expr) {
    return evaluate(expr->expression);
}

std::any Interpreter::visit(std::shared_ptr<LiteralExpr> expr) {
    return expr->literal;
}

std::any Interpreter::visit(std::shared_ptr<LogicalExpr> expr) {
    std::any left = evaluate(expr->left);

    if (expr->op.type == OR) {
        if (isTruthy(left)) return left;
        else if (!isTruthy(left)) return left;
    }

    return evaluate(expr->right);
}

std::any Interpreter::visit(std::shared_ptr<UnaryExpr> expr) {
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
        case BANG:
            return !isTruthy(right);
        case MINUS:
            checkNumberOperand(expr->op, right);
            return -std::any_cast<double>(right);
        default:
            return std::any{};
    }
}

std::any Interpreter::visit(std::shared_ptr<VariableExpr> expr) {
    return lookUpVariable(expr->name, expr);
}

void Interpreter::checkNumberOperand(
        const Token &op, 
        const std::any &operand) {
    if (operand.type() == typeid(double)) return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperand(
        const Token &op, 
        const std::any &left, 
        const std::any &right) {
    if ((left.type() == typeid(double)) && (right.type() == typeid(double))) 
        return;
    throw RuntimeError(op, "Operands must be numbers.");
}

bool Interpreter::isTruthy(const std::any &object) {
    if (!object.has_value()) return false;
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
    return true;
}

bool Interpreter::isEqual(const std::any &a, const std::any &b) {
    if (a.has_value() && b.has_value()) return true;
    if (a.has_value()) return false;

    if (a.type() != b.type()) return false;
    
    if (a.type() == typeid(bool)) 
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);

    if (a.type() == typeid(double)) 
        return std::any_cast<double>(a) == std::any_cast<double>(b);

    if (a.type() == typeid(std::string)) 
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);

    return false;
}

std::any Interpreter::lookUpVariable(
        const Token &name, 
        std::shared_ptr<Expr> expr) {
    for (auto [key, value] : locals) {
        std::cout << std::any_cast<int>(value) << " ";
    }
    std::cout << '\n';
    auto element = locals.find(expr);
    if (element != locals.end()) {
        int distance = element->second;
        return environment->getAt(distance, name.lexeme);
    } else {
        return globals->get(name);
    }
}

std::string Interpreter::stringify(std::any object) {
    if (!object.has_value()) return "nil";

    if (object.type() == typeid(double)) {
        std::string text = std::to_string(std::any_cast<double>(object));
        int len = text.length();
        if (len >= 2 && text[len-2] == '0' && text[len-1] == '.') {
            text = text.substr(0, len - 2);
        }
        return text;
    }

    if (object.type() == typeid(std::string))
        return std::any_cast<std::string>(object);

    if (object.type() == typeid(bool))
        return std::any_cast<bool>(object) ? "true" : "false";
    
    if (object.type() == typeid(std::shared_ptr<LoxFunction>))
        return std::any_cast<std::shared_ptr<LoxFunction>>(object)->toString();

    return "Error in stringify: object type not recognized.";
}