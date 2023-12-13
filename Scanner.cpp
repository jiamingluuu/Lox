/*
 * Filename: Scanner.cpp
 */
#include "Scanner.h"
#include "TokenType.h"
#include "Lox.h"

std::map<std::string, TokenType> Scanner::keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"fun", TokenType::FUN},
    {"for", TokenType::FOR},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
};

Scanner::Scanner(const std::string &source) : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    while(!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "", line));
    return tokens;
}

void Scanner::scanToken() {
    char c = advance();

    switch (c) {
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                advance();
                blockComment();
            } else {
                addToken(SLASH);
            }
            break;

        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break; 

        case '!':
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;

        case ' ':
        case '\r':
        case '\t':
          // Ignore whitespace.
          break;

        case '\n':
          line++;
          break;
          
        case '"': string(); break;
        
        case 'o':
            if (match('r')) {
                addToken(OR);
            }
        break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                Lox::error(line, "Unexpected character.");
            }
            break;
    }
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        
    }
}

bool Scanner::isAtEnd() {
    return current >= source.length();
}

char Scanner::advance() {
    return source[current++];
}

void Scanner::addToken(TokenType type) {
    tokens.push_back(Token(type, "", line));
}

bool Scanner::match(char expected) {
    // consume current char
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;

    current++;
    return true;
}

char Scanner::peek() {
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') || 
            c == '_';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
