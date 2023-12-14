#include "Token.h"
#include "TokenType.h"

Token::Token(TokenType type, std::string lexeme, std::any literal, int line): 
    type(type), lexeme(std::move(lexeme)), literal(literal), line(line) {}

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << std::string("Token: ") <<  tokenTypeToString(token.type) 
       << std::string(" ")  << token.lexeme  << std::string(" ") 
       << std::to_string(token.line);

    return os;
}
