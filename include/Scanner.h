#pragma once

#include <any>
#include <map>
#include <string>
#include <vector>

#include "Token.h"

class Scanner {
 public:
    Scanner(const std::string& source);
    std::vector<Token> scanTokens();

 private:
    std::string source;
    std::vector<Token> tokens;
    const static std::map<std::string, TokenType> keywords;
    int start = 0;
    int current = 0;
    int line = 1;
    void scanToken();
    void addToken();
    void string();
    void blockComment();
    void number();
    void identifier();
    bool isAtEnd();
    char advance();
    void addToken(TokenType type, std::any literal);
    void addToken(TokenType type);
    bool match(char expected);
    char peek();
    char peekNext();
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};