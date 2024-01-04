#include <memory>

#include "../include/Expr.h"
#include "../include/Lox.h"
#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser (std::vector<Token> tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Stmt> Parser::statement() {
    /*
     * statement      → exprStmt
     *                | ifStmt
     *                | printStmt
     *                | block ;
     */
    // if (match({IF)})) return ifStatement();
    if (match({PRINT})) return printStatement();
    if (match({LEFT_BRACE})) return std::make_shared<BlockStmt>(block());

    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::declaration() {
    /*
     * declaration    → varDecl
     *                | statement ;
     */
    try {
        if (match({VAR})) return varDeclaration();

        return statement();
    } catch (ParserError error) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::printStatement() {
    /*
     * printStmt      → "print" expression ";" ;
     */
    std::shared_ptr<Expr> value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    /*
     * varDecl        → "var" IDENTIFIER ( "=" expression )? ";" ;
     */
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({EQUAL})) {
        initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(std::move(name), initializer);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    /*
     * exprStmt       → expression ";" ;
     */
    std::shared_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    /*
     * block          → "{" declaration* "}"
     */
    std::vector<std::shared_ptr<Stmt>> statements{};

    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();

    if (match({EQUAL})) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();

        if (VariableExpr* e = dynamic_cast<VariableExpr*>(expr.get())) {
            Token name = e->name;
            return std::make_shared<AssignExpr>(std::move(name), value);
        }

        error(std::move(equals), "Invalid assignment target.");
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    /*
     * equality       → comparison ( ( "!=" | "==" ) comparison )* ;
     */

    std::shared_ptr<Expr> expr = comparison();

    while(match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, std::move(op), right);
    }
    
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    /* 
     * comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
     */

    std::shared_ptr<Expr> expr = term();

    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<BinaryExpr>(expr, std::move(op), right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();

    while (match({MINUS, PLUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();

    while (match({SLASH, STAR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    /*
     * unary    → ( "!" | "-" ) unary
     *          | primary ;
     */
    if (match({BANG, MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<UnaryExpr>(op, std::move(right));
    }

    return primary();
}

std::shared_ptr<Expr> Parser::primary() {
    /*
     * primary        → "true" | "false" | "nil"
     *                | NUMBER | STRING
     *                | "(" expression ")"
     *                | IDENTIFIER ;
     */
    if (match({FALSE})) return std::make_shared<LiteralExpr>(false);
    if (match({TRUE})) return std::make_shared<LiteralExpr>(true);
    if (match({NIL})) return std::make_shared<LiteralExpr>(nullptr);

    if (match({NUMBER, STRING})) {
        return std::make_shared<LiteralExpr>(previous().literal);
    }

    if (match({IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous());
    }

    if (match({LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression");
        return std::make_shared<GroupingExpr>(std::move(expr));
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

        // switch (peek().type) {
        //     case CLASS:
        //     case FUN:
        //     case VAR:
        //     case FOR:
        //     case IF:
        //     case WHILE:
        //     case PRINT:
        //     case RETURN:
        //         return;
        // }

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
