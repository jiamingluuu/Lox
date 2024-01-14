#ifndef __AST_PRINTER_H
#define __AST_PRINTER_H

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

#include "../include/Expr.h"

class AstPrinter: public ExprVisitor<std::any> {
public:
    std::string print(std::shared_ptr<Expr> expr) {
        return std::any_cast<std::string>(expr->accept(*this));
    }

    std::any visit(std::shared_ptr<BinaryExpr> expr) override {
        return parenthesize(expr->op.lexeme, expr->left, expr->right);
    }

    std::any visit(std::shared_ptr<GroupingExpr> expr) override {
        return parenthesize("group", expr->expression);
    }

    std::any visit(std::shared_ptr<LiteralExpr> expr) override {
        auto& literal_type = expr->literal.type();

        if (literal_type == typeid(nullptr)) {
            return "nil";
        } else if (literal_type == typeid(std::string)) {
            return std::any_cast<std::string>(expr->literal);
        } else if (literal_type == typeid(double)) {
            return std::to_string(std::any_cast<double>(expr->literal));
        } else if (literal_type == typeid(bool)) {
            return std::any_cast<bool>(expr->literal) ? "true" : "false";
        }

        return "Error in visitLiteralExpr: literal type not recognized.";
    }

    std::any visit(std::shared_ptr<UnaryExpr> expr) override {
      return parenthesize(expr->op.lexeme, expr->right);
    }

private:
    template <class... E>
    std::string parenthesize(std::string_view name, E... expr) {
        assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

        std::ostringstream builder;

        builder << "(" << name;
        ((builder << " " << print(expr)), ...);
        builder << ")";

        return builder.str();
    }
};

#endif /* __AST_PRINTER_H */
