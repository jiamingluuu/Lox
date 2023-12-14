#ifndef __TOKEN_H
#define __TOKEN_H

#include <any>
#include <string>

#include "TokenType.h"

struct Token {
public:
    Token(TokenType type, std::string lexeme, std::any literal, int line);
    friend std::ostream& operator<<(std::ostream& os, const Token& token);

private:
    TokenType type;
    std::string lexeme;
    std::any literal;
    int line;
};

#endif
