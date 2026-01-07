#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if (!file) {  // check if file opened successfully
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string fileContents = buffer.str();

    std::string code = fileContents;
    
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    return 0;
}