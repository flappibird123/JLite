#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>

int main() {
    // Now you can write code as a string!
    std::string code = "var obj = new Node(); obj.val = 42; print obj.val;";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    return 0;
}