#include <memory>

#include "../include/Expr.h"
#include "../include/Lox.h"
#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser (std::vector<Token> tokens) : tokens(tokens) {}

std::unique_ptr<Expr> Parser::parse() {
    try {
        return expression();
    } catch (ParserError error) {
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    /*
     * equality       → comparison ( ( "!=" | "==" ) comparison )* ;
     */

    std::unique_ptr<Expr> expr = comparison();

    while(match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    /* 
     * comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
     */

    std::unique_ptr<Expr> expr = term();

    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({MINUS, PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    /*
     * unary    → ( "!" | "-" ) unary
     *          | primary ;
     */
    if (match({BANG, MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return call();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({FALSE})) return std::make_unique<LiteralExpr>(false);
    if (match({TRUE})) return std::make_unique<LiteralExpr>(true);
    if (match({NIL})) return std::make_unique<LiteralExpr>(nullptr);

    if (match({NUMBER, STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match({LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();

    throw error(peek(), message);
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == END_OF_FILE;
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == SEMICOLON) return;

        switch (peek().type) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINT:
            case RETURN:
                return;
        }

        advance();
    }
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Parser::ParserError Parser::error(Token token, const std::string& message) {
    Lox::error(token, message);
    return ParserError{};
}
