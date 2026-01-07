#pragma once
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include "AST.h"

// Forward Decl
class Environment;
class LoxInstance;

// A Value in our language
struct Value {
    enum Type { NIL, BOOL, NUMBER, STRING, INSTANCE, FUNCTION } type;
    std::variant<std::monostate, bool, double, std::string, size_t> as; // size_t is heap address

    std::string toString() const;
};

// Heap Object Base
struct HeapObject {
    bool marked = false;
    virtual ~HeapObject() = default;
};

// A concrete instance of a class
struct InstanceObject : HeapObject {
    std::string className;
    std::unordered_map<std::string, Value> fields;
    InstanceObject(std::string name) : className(name) {}
};

// The Memory Manager
class Heap {
public:
    static std::unordered_map<size_t, HeapObject*> objects;
    static size_t nextId;
    static size_t bytesAllocated;
    static size_t nextGC;

    static size_t allocate(HeapObject* obj);
    static HeapObject* get(size_t addr);
    
    // Garbage Collection
    static void collectGarbage(Environment* roots);
    static void mark(Value val);
    static void markObject(HeapObject* obj);
    static void sweep();
};