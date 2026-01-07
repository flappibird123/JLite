#include "Lexer.h"
#include <iostream>

std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"class", CLASS}, {"else", ELSE}, {"false", FALSE},
    {"if", IF}, {"null", NIL}, {"print", PRINT},
    {"return", RETURN}, {"super", SUPER}, {"this", THIS},
    {"true", TRUE}, {"var", VAR}, {"while", WHILE}, {"new", NEW}
};

Lexer::Lexer(std::string source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({END_OF_FILE, "", {}, line});
    return tokens;
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
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
        case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
        case '=': addToken(match('=') ? EQUAL_EQUAL : EQUAL); break;
        case '<': addToken(match('=') ? LESS_EQUAL : LESS); break;
        case '>': addToken(match('=') ? GREATER_EQUAL : GREATER); break;
        case '/': 
            if (match('/')) while (peek() != '\n' && !isAtEnd()) advance();
            else addToken(SLASH);
            break;
        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '"': string(); break;
        default:
            if (isdigit(c)) number();
            else if (isalpha(c) || c == '_') identifier();
            else std::cerr << "Unexpected character at line " << line << "\n";
            break;
    }
}

bool Lexer::isAtEnd() { return current >= source.length(); }

char Lexer::advance() { return source[current++]; }

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::addToken(TokenType type) { addToken(type, {}); }

void Lexer::addToken(TokenType type, std::variant<std::monostate, double, std::string> literal) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, literal, line});
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : IDENTIFIER;
    addToken(type);
}

void Lexer::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(NUMBER, std::stod(source.substr(start, current - start)));
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) {
        std::cerr << "Unterminated string at line " << line << "\n";
        return;
    }
    advance(); // The closing "
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(STRING, value);
}