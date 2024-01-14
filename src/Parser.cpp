#include <memory>
#include <iostream>

#include "../include/Expr.h"
#include "../include/Lox.h"
#include "../include/Parser.h"
#include "../include/Token.h"

Parser::Parser (const std::vector<Token> &tokens) : tokens(tokens) {}

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
    // statement      → exprStmt
    //                | forStmt
    //                | ifStmt
    //                | printStmt
    //                | returnStmt
    //                | whileStmt
    //                | block ;
    if (match({FOR})) return forStatement();
    if (match({IF})) return ifStatement();
    if (match({PRINT})) return printStatement();
    if (match({RETURN})) return returnStatement();
    if (match({WHILE})) return whileStmt();
    if (match({LEFT_BRACE})) return std::make_shared<BlockStmt>(block());

    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::declaration() {
    // declaration    → funDecl
    //                | varDecl
    //                | statement ;
    try {
        if (match({FUN})) return function("function");
        if (match({VAR})) return varDeclaration();

        return statement();
    } catch (ParserError error) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::forStatement() {
    // forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
    //                  expression? ";"
    //                  expression? ")" statement ;
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if (match({SEMICOLON})) {
        initializer = nullptr;
    } else if (match({VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    std::shared_ptr<Expr> condition = nullptr;
    if (!check(SEMICOLON)) {
        condition = expression();
    }
    consume(SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if (!check(RIGHT_PAREN)) {
        increment = expression();
    }

    consume(RIGHT_PAREN, "Expect ')' after for clauses.");
    std::shared_ptr<Stmt> body = statement();

    if (increment != nullptr) {
        body = std::make_shared<BlockStmt>(
            std::vector<std::shared_ptr<Stmt>>{
                body,
                std::make_shared<ExpressionStmt>(increment)});
    }

    if (condition == nullptr) {
        condition = std::make_shared<LiteralExpr>(true);
    }
    body = std::make_shared<WhileStmt>(condition, body);

    if (initializer != nullptr) {
        body = std::make_shared<BlockStmt>(std::vector<std::shared_ptr<Stmt>>{
                initializer, body});
    }

    return body;
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match({ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::printStatement() {
    // printStmt      → "print" expression ";" ;
    std::shared_ptr<Expr> value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    // returnStmt     → "return" expression? ";" ;
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(SEMICOLON)) {
        value = expression();
    }
    
    consume(SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<ReturnStmt>(keyword, value);
}

std::shared_ptr<Stmt> Parser::whileStmt() {
    // whileStmt      → "while" "(" expression ")" statement ;
    consume(LEFT_PAREN, "Expect '(' after 'while'");
    std::shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");
    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<WhileStmt>(condition, body);
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    // varDecl        → "var" IDENTIFIER ( "=" expression )? ";" ;
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({EQUAL})) {
        initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(std::move(name), initializer);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    // exprStmt       → expression ";" ;
    std::shared_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    // block          → "{" declaration* "}" ;
    std::vector<std::shared_ptr<Stmt>> statements{};

    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<FunctionStmt> Parser::function(std::string kind) {
    // funDecl        → "fun" function ;
    // function       → IDENTIFIER "(" parameters? ")" block ;
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters{};
    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(IDENTIFIER, "Expect parameter name"));
        } while (match({COMMA}));
    }
    consume(RIGHT_PAREN, "Expect ')' after " + kind + " name.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_shared<FunctionStmt>(name, parameters, body);
}

std::shared_ptr<Expr> Parser::assignment() {
    // assignment     → IDENTIFIER "=" assignment
    //                | logic_or ;
    std::shared_ptr<Expr> expr = orExpr();
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

std::shared_ptr<Expr> Parser::orExpr() {
    // logic_or       → logic_and ( "or" logic_and )* ;
    std::shared_ptr<Expr> expr = andExpr();
    
    while (match({OR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = andExpr();
        expr = std::make_shared<LogicalExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::andExpr() {
    // logic_and      → equality ( "and" equality )* ;
    std::shared_ptr<Expr> expr = equality();
    
    while (match({AND})) {
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        expr = std::make_shared<LogicalExpr>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    // equality       → comparison ( ( "!=" | "==" ) comparison )* ;
    std::shared_ptr<Expr> expr = comparison();

    while(match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, std::move(op), right);
    }
    
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    // comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
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
    // unary    → ( "!" | "-" ) unary | call ;
    //          | primary ;
    if (match({BANG, MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<UnaryExpr>(op, std::move(right));
    }

    return call();
}

std::shared_ptr<Expr> Parser::call() {
    std::shared_ptr<Expr> expr = primary();

    while (true) {
        if (match({LEFT_PAREN})) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee) {
    std::vector<std::shared_ptr<Expr>> arguments{};

    if (!check(RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "con't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({COMMA}));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguements.");
    return std::make_shared<CallExpr>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::primary() {
    // primary        → "true" | "false" | "nil"
    //                | NUMBER | STRING
    //                | "(" expression ")"
    //                | IDENTIFIER ;
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

Token Parser::consume(TokenType type, std::string message) {
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
