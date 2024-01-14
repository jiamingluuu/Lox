#ifndef __PARSER_H
#define __PARSER_H

#include <vector>

#include "Expr.h"
#include "Token.h"
#include "Stmt.h"

class Parser {
public:
    Parser(const std::vector<Token> &tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    class ParserError : public std::runtime_error {
    public: 
        ParserError(): std::runtime_error("") {}
    };

    ParserError error(Token token, const std::string& message);

    const std::vector<Token> &tokens;
    int current = 0;

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> forStatement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStmt();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<Stmt> expressionStatement();
    std::shared_ptr<FunctionStmt> function(std::string kind);
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> orExpr();
    std::shared_ptr<Expr> andExpr();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee);
    std::shared_ptr<Expr> primary();

    void synchronize();

    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, std::string message);
    bool check(TokenType type);
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
};

#endif /* __PARSER_H */
