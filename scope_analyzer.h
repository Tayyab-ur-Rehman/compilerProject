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
    string mangled_name; // Unique name for qbe
    string type_name;  
    SymbolKind kind;
    int definition_line;
     
    vector<Parameter> params; 

    Symbol(string n, string t, SymbolKind k, int line) 
        : name(n), type_name(t), kind(k), definition_line(line) {
        mangled_name = n; // Default to name, updated for functions later
    }
};

struct Scope {
    multimap<string, Symbol*> symbols; //had to update this to allow rediffinations for functions names
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

    // to mekae unique signature for comparison
    string get_signature(const string& name, const vector<Parameter>& params) {
        string sig = name + "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) sig += ",";
            sig += params[i].type;
        }
        sig += ")";
        return sig;
    }

    // make uniqe labe for qbe
    string get_mangled_name(const string& name, const vector<Parameter>& params) {
        if (name == "main") return "main";
        string sig = name;
        for (const auto& p : params) {
            sig += "_" + p.type;
        }
        return sig;
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
        auto range = current_scope->symbols.equal_range(symbol->name);
        for (auto it = range.first; it != range.second; ++it) {
            Symbol* existing = it->second;

            if (symbol->kind == VARIABLE || existing->kind == VARIABLE) {
                string message = "Variable '" + symbol->name + "' redefined on line " + to_string(symbol->definition_line);
                throw ScopeError(ScopeErrorType::VariableRedefinition, message);
            }

            if (symbol->kind == FUNCTION && existing->kind == FUNCTION) {
                string sig_new = get_signature(symbol->name, symbol->params);
                string sig_old = get_signature(existing->name, existing->params);
                
                if (sig_new == sig_old) {
                    string message = "Function '" + sig_new + "' redefined on line " + to_string(symbol->definition_line);
                    throw ScopeError(ScopeErrorType::FunctionRedefinition, message);
                }
            }
        }
        
        current_scope->symbols.insert({symbol->name, symbol});
    }
    Symbol* find_variable_symbol(const string& name) {
        Scope* scope = current_scope;
        while (scope) {
            auto it = scope->symbols.find(name);
            if (it != scope->symbols.end()) {
                if (it->second->kind == VARIABLE) return it->second;
            }
            scope = scope->parent;
        }
        return NULL;
    }
    bool function_exists(const string& name) {
        Scope* scope = current_scope;
        while (scope) {
            if (scope->symbols.count(name)) return true;
            scope = scope->parent;
        }
        return false;
    }
    
    void visit(Program* node) {
        for (auto f : node->functions){
            f->resolved_return_type = f->returnType;
            for (auto& param : f->params) {
                param.resolved_type = param.type;
            }
            Symbol* func_sym = new Symbol(f->name, f->returnType, FUNCTION, f->line);
            func_sym->params = f->params;
            func_sym->mangled_name = get_mangled_name(f->name, f->params);
            if (f->name != "main") {
                f->name = func_sym->mangled_name;
            }

            add_symbol(func_sym);
        }
        for (auto g : node->globals) visit(g);
        for (auto f : node->functions) visit(f);
    }

    void visit(FunctionDeclaration* node) {
        enter_scope(node);
        for (const auto& param : node->params) {
            add_symbol(new Symbol(param.name, param.resolved_type, VARIABLE, param.line));
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
        node->resolved_type = node->type;
        add_symbol(new Symbol(node->name, node->resolved_type, VARIABLE, node->line));
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
        else if (auto p = dynamic_cast<UnaryOp*>(node)) visit(p);
    }

    void visit(BinaryOperation* node) {
        visit(node->left);
        visit(node->right);
    }
    
    void visit(UnaryOp* node) {
        visit(node->right);
    }
    
    void visit(Assignment* node) {
        visit(node->identifier);
        visit(node->value);
        node->inferred_type = node->identifier->inferred_type;
    }

    void visit(Identifier* node) {
        Symbol* sym = find_variable_symbol(node->name);
        if (!sym) {
            string message = "Undeclared variable '" + node->name + "' used on line " + to_string(node->line) + ".";
            throw ScopeError(ScopeErrorType::UndeclaredVariableAccessed, message);
        }
        node->inferred_type = sym->type_name;
    }

    void visit(FunctionCall* node) {
        if (!function_exists(node->callee)) {
            string message = "Call to undefined function '" + node->callee + "' on line " + to_string(node->line) + ".";
            throw ScopeError(ScopeErrorType::UndefinedFunctionCalled, message);
        }
        for(auto& arg : node->arguments) visit(arg);
    }
};