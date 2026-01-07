#pragma once
#include <string>
#include <variant>

enum TokenType {
    // Single-char
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    
    // One or two char
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords (Java-like)
    CLASS, ELSE, FALSE, FUN, IF, NIL, PRINT, RETURN, 
    SUPER, THIS, TRUE, VAR, WHILE, NEW, EXTENDS,
    
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, double, std::string, bool> literal;
    int line;

    Token(TokenType type, std::string lexeme, 
          std::variant<std::monostate, double, std::string, bool> literal, 
          int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
};