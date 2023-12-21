#ifndef __PARSER_H
#define __PARSER_H

#include <vector>

#include "Expr.h"
#include "Token.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::unique_ptr<Expr> parse();

private:
    class ParserError : public std::runtime_error {
    public: 
        ParserError(): std::runtime_error("") {}
    };
    ParserError error(Token token, const std::string& message);

    std::vector<Token> tokens;
    int current = 0;

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();
    void synchronize();

    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
};

#endif /* __PARSER_H */
