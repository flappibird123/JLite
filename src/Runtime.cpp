#include "Runtime.h"
#include <iostream>

std::unordered_map<size_t, HeapObject*> Heap::objects;
size_t Heap::nextId = 1;
size_t Heap::bytesAllocated = 0;
size_t Heap::nextGC = 1024 * 1024; // 1MB threshold

std::string Value::toString() const {
    if (type == NIL) return "null";
    if (type == BOOL) return std::get<bool>(as) ? "true" : "false";
    if (type == NUMBER) {
        double d = std::get<double>(as);
        // Clean trailing zeros
        std::string s = std::to_string(d);
        return s.substr(0, s.find_last_not_of('0') + 1); 
    }
    if (type == STRING) return std::get<std::string>(as);
    if (type == INSTANCE) return "Instance@" + std::to_string(std::get<size_t>(as));
    return "";
}

size_t Heap::allocate(HeapObject* obj) {
    // Hardcoded low limit to force GC for demonstration
    if (objects.size() > 10) { 
        // In real impl, we'd need access to the Interpreter's current environment here
        // For now, we assume explicit GC calls or pass environment context during alloc.
        std::cout << "[System] Heap pressure. (GC would trigger here)\n";
    }
    
    size_t id = nextId++;
    objects[id] = obj;
    return id;
}

HeapObject* Heap::get(size_t addr) {
    if (objects.find(addr) != objects.end()) return objects.at(addr);
    throw std::runtime_error("Segmentation Fault: Accessing freed memory.");
}

void Heap::mark(Value val) {
    if (val.type == Value::INSTANCE) {
        size_t addr = std::get<size_t>(val.as);
        if (objects.find(addr) != objects.end()) {
            markObject(objects[addr]);
        }
    }
}

void Heap::markObject(HeapObject* obj) {
    if (obj == nullptr || obj->marked) return;
    obj->marked = true;

    // If it's an instance, traverse its fields
    if (auto* inst = dynamic_cast<InstanceObject*>(obj)) {
        for (auto& pair : inst->fields) {
            mark(pair.second);
        }
    }
}

void Heap::sweep() {
    auto it = objects.begin();
    while (it != objects.end()) {
        if (!it->second->marked) {
            delete it->second;
            it = objects.erase(it);
        } else {
            it->second->marked = false; // Reset for next cycle
            ++it;
        }
    }
}

void Heap::collectGarbage(Environment* env) {
    std::cout << "-- GC BEGIN --\n";
    // 1. Mark Roots (We need to implement Environment traversal)
    // See Interpreter.cpp for the actual implementation linking Env -> Mark
    
    // 2. Sweep
    sweep();
    std::cout << "-- GC END. Objects remaining: " << objects.size() << " --\n";
}