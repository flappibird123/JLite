#pragma once
#include "AST.h"
#include "Runtime.h"
#include <unordered_map>

class Environment {
public:
    Environment* enclosing;
    std::unordered_map<std::string, Value> values;

    Environment(Environment* enclosing = nullptr) : enclosing(enclosing) {}

    void define(std::string name, Value value);
    void assign(Token name, Value value);
    Value get(Token name);
};

class Interpreter {
public:
    Environment* globals;
    Environment* environment;
    std::unordered_map<std::string, std::shared_ptr<ClassStmt>> classes;

    Interpreter();
    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    
    // Visitor methods for evaluating AST
    Value evaluate(std::shared_ptr<Expr> expr);
    void execute(std::shared_ptr<Stmt> stmt);

    // Helpers
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, Environment* env);
    void triggerGC();
};