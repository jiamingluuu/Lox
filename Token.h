#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>

#include "TokenType.h"

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    
    Token(TokenType type, const std::string& lexeme, int line);
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

#endif
