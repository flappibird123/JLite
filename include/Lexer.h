#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Token.h"
#include <variant>

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    static std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::variant<std::monostate, double, std::string> literal);
    char peek();
    char peekNext();
    bool match(char expected);
    void string();
    void number();
    void identifier();
    void scanToken();
};