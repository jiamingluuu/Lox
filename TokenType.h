#ifndef __TOKENTYPE_H
#define __TOKENTYPE_H

#include <string>

enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, MINUS, PLUS, 
    SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, 
    LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    END_OF_FILE
};

inline std::string tokenTypeToString(TokenType type);

#endif /* __TOKENTYPE_H */ 
