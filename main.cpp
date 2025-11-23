#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include "tokens.h" 
#include "ast.h"
#include "parser.h"
#include "scope_analyzer.h"
#include "typechecker.h"
#include "ir_gen.h"


// g++ -std=c++17 main.cpp lexer_regex.cpp IR.cpp -o main
// ./main scope_testing/scope_test.c
using namespace std;

// Forward declare the function from lexer_regex.cpp
 vector<Token> tokenize(const  string& source);
 string readFile(const  string& filename);


int main(int argc, char* argv[]) {
    if (argc != 2) {
         cerr << "Usage: " << argv[0] << " <source_file>" <<  endl;
        return 1;
    }

     string filename = argv[1];
    Program* ast_root = nullptr;
    ScopeAnalyzer* scope_analyzer = nullptr;

    try {
        // 1. Lexical Analysis
         cout << "1. Lexical Analysis..." <<  endl;
         string source_code = readFile(filename);
         vector<Token> tokens = tokenize(source_code);
         cout << "   Lexing complete." <<  endl;
        
        // 2. Parsing
         cout << "2. Parsing..." <<  endl;
        Parser parser(tokens);
        ast_root = parser.parse_program();
         cout << "   Parsing complete." <<  endl;
        
        // 3. Scope Analysis
         cout << "3. Scope Analysis..." <<  endl;
        scope_analyzer = new ScopeAnalyzer();
        scope_analyzer->analyze(ast_root);
         cout << "   Scope analysis complete." <<  endl;

        // 4. Type Checking
         cout << "4. Type Checking..." <<  endl;
        TypeChecker type_checker(scope_analyzer->global_scope);
        type_checker.check(ast_root);
         cout << "   Type checking complete." <<  endl;

        // 5. IR Generation
         cout << "5. QBE IR Generation..." <<  endl;
        IRGenerator ir_gen;
         string output_filename = filename.substr(0, filename.find_last_of('.')) + ".qbe";
        ir_gen.generate(ast_root, output_filename);
         cout << "   IR generation complete. Output written to " << output_filename <<  endl;

    } catch (const ParseError& e) {
         cerr << "\n[ERROR] Parse Error: " << e.what() <<  endl;
        delete ast_root;
        delete scope_analyzer;
        return 1;
    } catch (const ScopeError& e) {
         cerr << "\n[ERROR] Scope Error: " << e.what() <<  endl;
        delete ast_root;
        delete scope_analyzer;
        return 1;
    } catch (const TypeError& e) { 
         cerr << "\n[ERROR] Type Error: " << e.what() <<  endl;
        delete ast_root;
        delete scope_analyzer;
        return 1;
    } catch (const  exception& e) {
         cerr << "\n[ERROR] An unexpected error occurred: " << e.what() <<  endl;
        delete ast_root;
        delete scope_analyzer;
        return 1;
    }

    delete ast_root;
    delete scope_analyzer;

     cout << "\nCompilation successful." <<  endl;

    return 0;
}