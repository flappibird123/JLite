#include "Parser.h"

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match(CLASS)) return classDeclaration();
    if (match(VAR)) return varDeclaration();
    return statement();
}

std::shared_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(IDENTIFIER, "Expect class name.");
    consume(LEFT_BRACE, "Expect '{' before class body.");
    std::vector<std::shared_ptr<Function>> methods;
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        // Simple logic: every identifier in a class is treated as a method for now
        // A full impl would parse parameters and bodies
    }
    consume(RIGHT_BRACE, "Expect '}' after class body.");
    return std::make_shared<ClassStmt>(name, methods);
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Expr> initializer = nullptr;
    if (match(EQUAL)) initializer = expression();
    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(name, initializer);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(PRINT)) return printStatement();
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::printStatement() {
    std::shared_ptr<Expr> value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return std::make_shared<PrintStmt>(value);
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();
    if (match(EQUAL)) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();
        if (auto v = std::dynamic_pointer_cast<Variable>(expr)) {
            return std::make_shared<Assign>(v->name, value);
        } else if (auto g = std::dynamic_pointer_cast<Get>(expr)) {
            return std::make_shared<Set>(g->object, g->name, value);
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();
    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = term();
    while (match({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();
    while (match({MINUS, PLUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();
    while (match({SLASH, STAR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match({BANG, MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Binary>(nullptr, op, right); // Simplified
    }
    return call();
}

std::shared_ptr<Expr> Parser::call() {
    std::shared_ptr<Expr> expr = primary();
    while (true) {
        if (match(DOT)) {
            Token name = consume(IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_shared<Get>(expr, name);
        } else {
            break;
        }
    }
    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(FALSE)) return std::make_shared<Literal>(false);
    if (match(TRUE)) return std::make_shared<Literal>(true);
    if (match(NIL)) return std::make_shared<Literal>(std::monostate{});
    if (match(NUMBER) || match(STRING)) return std::make_shared<Literal>(previous().literal);
    
    if (match(NEW)) {
        Token name = consume(IDENTIFIER, "Expect class name.");
        consume(LEFT_PAREN, "Expect '(' after class name.");
        consume(RIGHT_PAREN, "Expect ')' after arguments.");
        return std::make_shared<New>(name);
    }

    if (match(IDENTIFIER)) return std::make_shared<Variable>(previous());

    throw std::runtime_error("Expect expression.");
}

// Helpers
bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) { advance(); return true; }
    }
    return false;
}
bool Parser::match(TokenType type) { return match(std::vector<TokenType>{type}); }
bool Parser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::isAtEnd() { return peek().type == END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }
Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}