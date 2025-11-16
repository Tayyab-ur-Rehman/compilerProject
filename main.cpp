#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "tokens.h" 
#include "ast.h"
#include "parser.h"
#include "lexer_regex.cpp" 
#include "scope_analyzer.h"
#include "typechecker.h" 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <source_file.c>" << endl;
        return 1;
    }

    string filename = argv[1];
    cout << "Parsing file: " << filename << endl;

    Program* ast_root = NULL; 
    Scope* global_scope = NULL; 

    try {
        cout << "\n1. lexical analysis" << endl;
        string source_code = readFile(filename);
        vector<Token> tokens = tokenize(source_code);
        cout << "   Lexing complete. " << tokens.size() << " tokens found." << endl;
        
        cout << "\n2 Syntactic Analysis (Parsing)" << endl;
        Parser parser(tokens);
        ast_root = parser.parse_program();
        cout << "   Parsing complete. AST generated." << endl;
        
        cout << "\n3.Scope analysis" << endl;
        ScopeAnalyzer scope_analyzer;
        scope_analyzer.analyze(ast_root);
        global_scope = scope_analyzer.global_scope;
        cout << "   Scope analysis complete. No redefinition or undeclared symbol errors found." << endl;

    
        cout << "\n4. Type Checking" << endl;
        TypeChecker type_checker(global_scope);
        type_checker.check(ast_root);
        cout << "   Type checking complete. No type errors found." << endl;

        cout << "\nAbstract Syntax Tree" << endl;
        if (ast_root) {
            ast_root->print(0);
        }

    }
    catch (const ParseError& e) {
        cerr << "\nPARSE ERROR " << endl;
        cerr << "Error: " << e.what() << endl;
        if(global_scope) delete global_scope;
        if(ast_root) delete ast_root;
        return 1;
    } 
    catch (const ScopeError& e) {
        cerr << "\nSCOPE ERROR " << endl;
        cerr << "Error: " << e.what() << endl;
        if(global_scope) delete global_scope;
        if(ast_root) delete ast_root;
        return 1;
    } 
    catch (const TypeError& e) { 
        cerr << "\nTYPE ERROR " << endl;
        cerr << "Error: " << e.what() << endl;
        if(global_scope) delete global_scope;
        if(ast_root) delete ast_root;
        return 1;
    }
    catch (const std::exception& e) {
        cerr << "\nGENERAL ERROR" << endl;
        cerr << "An unexpected error occurred: " << e.what() << endl;
        if(global_scope) delete global_scope;
        if(ast_root) delete ast_root;
        return 1;
    }

    delete global_scope;
    delete ast_root;

    cout << "\nCompilation successful" << endl;

    return 0;
}