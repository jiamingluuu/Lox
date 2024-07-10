#include "../include/Scanner.h"

#include "../include/Lox.h"
#include "../include/Token.h"

const std::map<std::string, TokenType> Scanner::keywords = {
    {"and", AND},   {"class", CLASS}, {"else", ELSE}, {"false", FALSE}, {"fun", FUN},       {"for", FOR},
    {"if", IF},     {"nil", NIL},     {"or", OR},     {"print", PRINT}, {"return", RETURN}, {"super", SUPER},
    {"this", THIS}, {"true", TRUE},   {"var", VAR},   {"while", WHILE},
};

Scanner::Scanner(const std::string &source) : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(TokenType::END_OF_FILE, "", std::any{}, line));
    return tokens;
}

void Scanner::scanToken() {
    char c = advance();

    switch (c) {
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else if (match('*')) {
                advance();
                blockComment();
            } else {
                addToken(SLASH);
            }
            break;

        case '(':
            addToken(LEFT_PAREN);
            break;
        case ')':
            addToken(RIGHT_PAREN);
            break;
        case '{':
            addToken(LEFT_BRACE);
            break;
        case '}':
            addToken(RIGHT_BRACE);
            break;
        case ',':
            addToken(COMMA);
            break;
        case '.':
            addToken(DOT);
            break;
        case '-':
            addToken(MINUS);
            break;
        case '+':
            addToken(PLUS);
            break;
        case ';':
            addToken(SEMICOLON);
            break;
        case '*':
            addToken(STAR);
            break;

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
        case '"':
            string();
            break;
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
        if (peek() == '\n') {
            line++;
        }
        advance();
    }
    if (isAtEnd()) {
        Lox::error(line, "Unterminated string.");
        return;
    }

    // consume the enclosing '"' char
    advance();

    // add the value inside the "" char
    std::string value = source.substr(start + 1, current - 2 - start);
    addToken(TokenType::STRING, value);
}

void Scanner::blockComment() {
    while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }
    if (isAtEnd()) {
        Lox::error(line, "Unterminated string.");
        return;
    }

    // consume the enclosing '*/' char
    advance();
    advance();
}

void Scanner::number() {
    while (isDigit(peek())) {
        advance();
    }
    if (peek() == '.' && isDigit(peekNext())) {
        // consume the decimal point to prevent from parsing something like
        //  123.add(567)
        advance();

        while (isDigit(peek())) {
            advance();
        }
    }

    addToken(TokenType::NUMBER, std::stod(source.substr(start, current)));
}

void Scanner::identifier() {
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type;
    auto match = keywords.find(text);
    if (match == keywords.end()) {
        type = IDENTIFIER;
    } else {
        type = match->second;
    }

    addToken(type);
}

bool Scanner::isAtEnd() {
    return current >= source.length();
}

char Scanner::advance() {
    return source[current++];
}

void Scanner::addToken(TokenType type, std::any literal) {
    tokens.push_back(Token(type, source.substr(start, current - start), literal, line));
}

void Scanner::addToken(TokenType type) {
    tokens.push_back(Token(type, source.substr(start, current - start), std::any{}, line));
}

bool Scanner::match(char expected) {
    // consume current char
    if (isAtEnd()) {
        return false;
    }
    if (source[current] != expected) {
        return false;
    }

    current++;
    return true;
}

char Scanner::peek() {
    return source[current];
}

char Scanner::peekNext() {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
