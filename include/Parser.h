#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    // Statement types
    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> classDeclaration();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> expressionStatement();

    // Expressions (Ordered by precedence)
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();

    // Helpers
    bool match(std::vector<TokenType> types);
    bool match(TokenType type);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);
};