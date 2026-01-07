# JLite

JLite is a lightweight, dynamically typed, object-oriented programming language implemented in C++17. It features a Java-inspired syntax, a recursive descent parser, and a custom mark-and-sweep garbage collector.

## Features

- **Java-like Syntax**: Familiar curly-brace blocks, classes, and variable declarations.
- **Dynamic Typing**: Variables can hold any data type and change at runtime.
- **Object-Oriented**: Support for classes and dynamic property assignment.
- **Automatic Memory Management**: Custom Mark-and-Sweep Garbage Collector.

## Getting Started

### Prerequisites
- **Compiler**: Clang (Apple Silicon/macOS) or GCC 9+ (Linux).
- **Build System**: CMake 3.10+.

### Installation & Build
1. Clone the repository:
   ```bash
   git clone https://github.com/flappibird123/JLite.git
   ```
2. Build the project:
   ```bash
     mkdir build && cd build
     cmake ..
     make
  ```
3. Run the interpreter:
    ```bash
        ./jlite filename.jlite
    ```

## Language guide

1. Variables and types
    Variables are declared with the `var` keyword




## Future improvements
    - Function support
    - Import support
    - Full Readme :(