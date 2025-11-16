#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "ast.h"

using namespace std;

enum class ScopeErrorType {
    UndeclaredVariableAccessed,
    UndefinedFunctionCalled,
    VariableRedefinition,
    FunctionRedefinition,
};

class ScopeError : public std::runtime_error {
public:
    ScopeErrorType type;
    ScopeError(ScopeErrorType t, const string& message) : runtime_error(message), type(t) {}
};

enum SymbolKind {
    VARIABLE,
    FUNCTION
};

struct Symbol {
    string name;
    string type_name;
    SymbolKind kind;
    int definition_line;
     
    vector<Parameter> params; 

    Symbol(string n, string t, SymbolKind k, int line) 
        : name(n), type_name(t), kind(k), definition_line(line) {}
};

struct Scope {
    map<string, Symbol*> symbols;
    Scope* parent;
    map<const void*, Scope*> children_scopes; 

    Scope(Scope* p) : parent(p) {}

    ~Scope() {
        for (auto& it : symbols) delete it.second;
        for (auto& it : children_scopes) delete it.second;
    }
};

class ScopeAnalyzer {
public:
    Scope* global_scope;

    ScopeAnalyzer() {
        global_scope = new Scope(NULL);
        current_scope = global_scope;
    }

    void analyze(Program* program_node) {
        visit(program_node);
    }

private:
    Scope* current_scope;

    void enter_scope(const void* node_key) {
        Scope* new_scope = new Scope(current_scope);
        current_scope->children_scopes[node_key] = new_scope; 
        current_scope = new_scope;
    }

    void exit_scope() {
        current_scope = current_scope->parent;
    }

    void add_symbol(Symbol* symbol) {
        if (current_scope->symbols.count(symbol->name)) {
            ScopeErrorType err_type = (symbol->kind == FUNCTION) ? 
                ScopeErrorType::FunctionRedefinition : 
                ScopeErrorType::VariableRedefinition;

            string message = (symbol->kind == FUNCTION ? "Function '" : "Variable '") + 
                             symbol->name + "' redefined on line " + to_string(symbol->definition_line) +
                             ". Previously defined on line " + to_string(current_scope->symbols[symbol->name]->definition_line) + ".";

            throw ScopeError(err_type, message);
        }
        current_scope->symbols[symbol->name] = symbol;
    }

    Symbol* find_symbol(const string& name, bool is_function_call) {
        Scope* scope = current_scope;
        while (scope) {
            if (scope->symbols.count(name)) {
                Symbol* sym = scope->symbols[name];
                if (is_function_call && sym->kind != FUNCTION) {
                    scope = scope->parent;
                    continue;
                }
                return sym;
            }
            scope = scope->parent;
        }
        return NULL;
    }
    
    void visit(Program* node) {
        for (auto f : node->functions){
            Symbol* func_sym = new Symbol(f->name, f->returnType, FUNCTION, f->line);
            func_sym->params = f->params;
            add_symbol(func_sym);
        }
        for (auto g : node->globals) visit(g);
        for (auto f : node->functions) visit(f);
    }

    void visit(FunctionDeclaration* node) {
        enter_scope(node);
        for (const auto& param : node->params) {
            add_symbol(new Symbol(param.name, param.type, VARIABLE, param.line));
        }
        visit(node->body);
        exit_scope();
    }

    void visit(BlockStatement* node) {
        enter_scope(node);
        for (auto s : node->statements) visit(s);
        exit_scope();
    }

    void visit(Statement* node) {
        if (!node) return;
        if (auto p = dynamic_cast<BlockStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<VariableDeclarationStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<ExpressionStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<IfStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<WhileStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<ForStatement*>(node)) visit(p);
        else if (auto p = dynamic_cast<ReturnStatement*>(node)) visit(p);
    }

    void visit(VariableDeclarationStatement* node) {
        if (node->initializer) visit(node->initializer);
        add_symbol(new Symbol(node->name, node->type, VARIABLE, node->line));
    }

    void visit(ExpressionStatement* node) {
        visit(node->expression);
    }
    
    void visit(IfStatement* node) {
        visit(node->condition);
        visit(node->thenBranch);
        if (node->elseBranch) visit(node->elseBranch);
    }
    
    void visit(WhileStatement* node) {
        visit(node->condition);
        visit(node->body);
    }
    
    void visit(ForStatement* node) {
        enter_scope(node);
        if(node->initializer) visit(node->initializer);
        if(node->condition) visit(node->condition);
        if(node->increment) visit(node->increment);
        visit(node->body);
        exit_scope();
    }
    
    void visit(ReturnStatement* node) {
        if (node->returnValue) visit(node->returnValue);
    }
    
    void visit(Expression* node) {
        if (!node) return;
        if (auto p = dynamic_cast<BinaryOperation*>(node)) visit(p);
        else if (auto p = dynamic_cast<Assignment*>(node)) visit(p);
        else if (auto p = dynamic_cast<Identifier*>(node)) visit(p);
        else if (auto p = dynamic_cast<FunctionCall*>(node)) visit(p);
    }

    void visit(BinaryOperation* node) {
        visit(node->left);
        visit(node->right);
    }
    
    void visit(Assignment* node) {
        visit(node->identifier);
        visit(node->value);
    }

    void visit(Identifier* node) {
        Symbol* sym = find_symbol(node->name, false);
        if (!sym) {
            string message = "Undeclared variable '" + node->name + "' used on line " + to_string(node->line) + ".";
            throw ScopeError(ScopeErrorType::UndeclaredVariableAccessed, message);
        }
    }

    void visit(FunctionCall* node) {
        Symbol* sym = find_symbol(node->callee, true);
        if (!sym) {
            string message = "Call to undefined function '" + node->callee + "' on line " + to_string(node->line) + ".";
            throw ScopeError(ScopeErrorType::UndefinedFunctionCalled, message);
        }
        for(auto& arg : node->arguments) visit(arg);
    }
};