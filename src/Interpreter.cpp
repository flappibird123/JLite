#include "Interpreter.h"
#include <iostream>

// --- Environment Impl ---
void Environment::define(std::string name, Value value) {
    values[name] = value;
}

Value Environment::get(Token name) {
    if (values.count(name.lexeme)) return values[name.lexeme];
    if (enclosing) return enclosing->get(name);
    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(Token name, Value value) {
    if (values.count(name.lexeme)) {
        values[name.lexeme] = value;
        return;
    }
    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }
    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

// --- Interpreter Impl ---
Interpreter::Interpreter() {
    globals = new Environment();
    environment = globals;
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        for (const auto& stmt : statements) {
            execute(stmt);
        }
    } catch (std::runtime_error& e) {
        std::cerr << "Runtime Error: " << e.what() << "\n";
    }
}

// Trigger GC using current environment as Root
void Interpreter::triggerGC() {
    // 1. Mark Roots
    Environment* current = environment;
    while(current != nullptr) {
        for(auto& pair : current->values) {
            Heap::mark(pair.second);
        }
        current = current->enclosing;
    }
    // 2. Sweep
    Heap::sweep();
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
    if (auto s = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
        Value val = evaluate(s->expression);
        std::cout << val.toString() << "\n";
    }
    else if (auto s = std::dynamic_pointer_cast<VarStmt>(stmt)) {
        Value val = {Value::NIL};
        if (s->initializer) val = evaluate(s->initializer);
        environment->define(s->name.lexeme, val);
    }
    else if (auto s = std::dynamic_pointer_cast<ClassStmt>(stmt)) {
        classes[s->name.lexeme] = s;
    }
    else if (auto s = std::dynamic_pointer_cast<Block>(stmt)) {
        executeBlock(s->statements, new Environment(environment));
    }
    // ... Add If, While, Function implementations here
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, Environment* env) {
    Environment* previous = environment;
    environment = env;
    try {
        for (const auto& stmt : statements) execute(stmt);
    } catch(...) {
        environment = previous;
        throw;
    }
    environment = previous;
    // Don't delete env here in a real language without proper memory management for Envs, 
    // but for this simple GC demo, we let C++ handle env pointer, but values are managed by Heap.
}

Value Interpreter::evaluate(std::shared_ptr<Expr> expr) {
    if (auto e = std::dynamic_pointer_cast<Literal>(expr)) {
        if (std::holds_alternative<double>(e->value)) 
            return {Value::NUMBER, std::get<double>(e->value)};
        if (std::holds_alternative<std::string>(e->value)) 
            return {Value::STRING, std::get<std::string>(e->value)};
        if (std::holds_alternative<bool>(e->value)) // ADD THIS
            return {Value::BOOL, std::get<bool>(e->value)};
        return {Value::NIL};
    }
    else if (auto e = std::dynamic_pointer_cast<Variable>(expr)) {
        return environment->get(e->name);
    }
    else if (auto e = std::dynamic_pointer_cast<Assign>(expr)) {
        Value val = evaluate(e->value);
        environment->assign(e->name, val);
        return val;
    }
    else if (auto e = std::dynamic_pointer_cast<New>(expr)) {
        // Look up class definition
        if (classes.find(e->className.lexeme) == classes.end()) 
            throw std::runtime_error("Unknown class " + e->className.lexeme);
        
        // Allocate Instance
        InstanceObject* obj = new InstanceObject(e->className.lexeme);
        size_t addr = Heap::allocate(obj);

        // Check GC Threshold
        if (Heap::objects.size() > 5) triggerGC();

        return {Value::INSTANCE, addr};
    }
    else if (auto e = std::dynamic_pointer_cast<Get>(expr)) {
        Value objVal = evaluate(e->object);
        if (objVal.type != Value::INSTANCE) throw std::runtime_error("Only instances have properties.");
        
        HeapObject* ho = Heap::get(std::get<size_t>(objVal.as));
        InstanceObject* io = dynamic_cast<InstanceObject*>(ho);
        
        if (io->fields.count(e->name.lexeme)) {
            return io->fields[e->name.lexeme];
        }
        return {Value::NIL};
    }
    else if (auto e = std::dynamic_pointer_cast<Set>(expr)) {
        Value objVal = evaluate(e->object);
        if (objVal.type != Value::INSTANCE) throw std::runtime_error("Only instances have fields.");
        
        Value val = evaluate(e->value);
        HeapObject* ho = Heap::get(std::get<size_t>(objVal.as));
        InstanceObject* io = dynamic_cast<InstanceObject*>(ho);
        
        io->fields[e->name.lexeme] = val;
        return val;
    } 
    // Add this inside Interpreter::evaluate logic
    else if (auto e = std::dynamic_pointer_cast<Binary>(expr)) {
        Value left = evaluate(e->left);
        Value right = evaluate(e->right);

        switch (e->op.type) {
            case PLUS:
                if (left.type == Value::NUMBER && right.type == Value::NUMBER)
                    return {Value::NUMBER, std::get<double>(left.as) + std::get<double>(right.as)};
                if (left.type == Value::STRING && right.type == Value::STRING)
                    return {Value::STRING, std::get<std::string>(left.as) + std::get<std::string>(right.as)};
                break;
            case MINUS:
                return {Value::NUMBER, std::get<double>(left.as) - std::get<double>(right.as)};
            case STAR:
                return {Value::NUMBER, std::get<double>(left.as) * std::get<double>(right.as)};
            case SLASH:
                return {Value::NUMBER, std::get<double>(left.as) / std::get<double>(right.as)};
            case EQUAL_EQUAL:
                return {Value::BOOL, left.as == right.as};
            // Add other comparisons as needed...
            default:
                throw std::runtime_error("Unknown or unhandled operator.");
        }
    }
    // ... Add Binary, Unary logic here (omitted for space, standard math logic)
    return {Value::NIL};
}