#include <iostream>
#include <any>

#include "../include/Interpreter.h"
#include "../include/Lox.h"
#include "../include/RuntimeError.h"

void Interpreter::interprete(std::shared_ptr<Expr> expr) {
    try {
        std::any value = evaluate(expr);
        std::cout << stringify(value) << "\n";
    } catch (RuntimeError error) {
        Lox::runtimeError(error);
    }
}

std::any Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
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

                return std::any_cast<double>(left) + std::any_cast<double>(right); 
            }

            if ((left.type() == typeid(std::string)) && 
                    (right.type() == typeid(std::string))) {
                
                return std::any_cast<std::string>(left) 
                    + std::any_cast<std::string>(right); 
            }

            // if ((left.type() == typeid(std::string)) || 
            //         (right.type() == typeid(std::string))) {
            //     return std::any_cast<std::string>(left) 
            //         + std::any_cast<std::string>(right); 
            // }

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

std::any Interpreter::visit(std::shared_ptr<LiteralExpr> expr) {
    return expr->literal;
}

std::any Interpreter::visit(std::shared_ptr<GroupingExpr> expr) {
    return evaluate(expr->expression);
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


void Interpreter::checkNumberOperand(Token op, const std::any& operand) {
    if (operand.type() == typeid(double)) return;
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperand(Token op, const std::any& left, 
        const std::any& right) {
    if ((left.type() == typeid(double)) && (right.type() == typeid(double))) 
        return;
    throw RuntimeError(op, "Operands must be numbers.");
}

bool Interpreter::isTruthy(const std::any& object) {
    if (!object.has_value()) return false;
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);
    return true;
}

bool Interpreter::isEqual(const std::any& a, const std::any& b) {
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

std::string Interpreter::stringify(const std::any& object) {
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

    return "Error in stringify: object type not recognized.";
}
