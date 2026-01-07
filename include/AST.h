#pragma once
#include "Token.h"
#include <memory>
#include <vector>

// Forward declarations
struct Expr;
struct Stmt;

// --- Expressions ---
struct Expr {
    virtual ~Expr() = default;
};

struct Binary : Expr {
    std::shared_ptr<Expr> left, right;
    Token op;
    Binary(std::shared_ptr<Expr> l, Token o, std::shared_ptr<Expr> r) : left(l), op(o), right(r) {}
};

struct Literal : Expr {
    std::variant<std::monostate, double, std::string, bool> value;
    
    Literal(std::variant<std::monostate, double, std::string, bool> v) : value(v) {}
};

struct Variable : Expr {
    Token name;
    Variable(Token n) : name(n) {}
};

struct Assign : Expr {
    Token name;
    std::shared_ptr<Expr> value;
    Assign(Token n, std::shared_ptr<Expr> v) : name(n), value(v) {}
};

struct New : Expr {
    Token className;
    New(Token name) : className(name) {} 
};

struct Get : Expr {
    std::shared_ptr<Expr> object;
    Token name;
    Get(std::shared_ptr<Expr> obj, Token n) : object(obj), name(n) {}
};

struct Set : Expr {
    std::shared_ptr<Expr> object;
    Token name;
    std::shared_ptr<Expr> value;
    Set(std::shared_ptr<Expr> obj, Token n, std::shared_ptr<Expr> v) : object(obj), name(n), value(v) {}
};

struct Call : Expr {
    std::shared_ptr<Expr> callee;
    std::vector<std::shared_ptr<Expr>> arguments;
    Call(std::shared_ptr<Expr> c, std::vector<std::shared_ptr<Expr>> args) : callee(c), arguments(args) {}
};

// --- Statements ---
struct Stmt {
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt {
    std::shared_ptr<Expr> expression;
    ExpressionStmt(std::shared_ptr<Expr> e) : expression(e) {}
};

struct PrintStmt : Stmt {
    std::shared_ptr<Expr> expression;
    PrintStmt(std::shared_ptr<Expr> e) : expression(e) {}
};

struct VarStmt : Stmt {
    Token name;
    std::shared_ptr<Expr> initializer;
    VarStmt(Token n, std::shared_ptr<Expr> i) : name(n), initializer(i) {}
};

struct Block : Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;
    Block(std::vector<std::shared_ptr<Stmt>> s) : statements(s) {}
};

struct Function : Stmt {
    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;
    Function(Token n, std::vector<Token> p, std::vector<std::shared_ptr<Stmt>> b) : name(n), params(p), body(b) {}
};

struct ClassStmt : Stmt {
    Token name;
    std::vector<std::shared_ptr<Function>> methods;
    ClassStmt(Token n, std::vector<std::shared_ptr<Function>> m) : name(n), methods(m) {}
};