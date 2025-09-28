#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "tokens.h" 
#include "ast.h"
#include "parser.h"
#include "lexer_regex.cpp" 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <source_file.c>" << endl;
        return 1;
    }

    string filename = argv[1];
    cout << " Parsing file: " << filename << " " << endl;

    Program* ast_root = NULL; 

    try {

        cout << "\nLexical Analysis " << endl;
        string source_code = readFile(filename);
        vector<Token> tokens = tokenize(source_code);
        cout << "Lexing complete. " << tokens.size() << " tokens found." << endl;
        cout << "\nSyntactic Analysis (Parsing) " << endl;
        Parser parser(tokens);
        ast_root = parser.parse_program();
        cout << "Parsing complete. AST generated." << endl;
        cout << "\nAbstract Syntax Tree Output " << endl;
        if (ast_root) {
            ast_root->print(0);
        } else {
            cout << "AST is null. This should not happen for valid programs." << endl;
        }

    } catch (const ParseError& e) {
        cerr << "\n PARSE ERROR " << endl;
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (const std::exception& e) {
        cerr << "\n GENERAL ERROR " << endl;
        cerr << "An unexpected error occurred: " << e.what() << endl;
        return 1;
    }

    delete ast_root;

    cout << "\n Compilation successful (Parsing Stage) " << endl;

    return 0;
}