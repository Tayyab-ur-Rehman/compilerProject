#pragma once

#include "ast.h"
#include "scope_analyzer.h" 
#include <stdexcept>
#include <vector>

enum class TypeChkError {
    ErroneousVarDecl,
    FnCallParamCount,
    FnCallParamType,
    ErroneousReturnType,
    ExpressionTypeMismatch,
    InvalidAssignment,
    NonBooleanCondStmt,
    ErroneousBreak,
    ErroneousContinue,
    AttemptedOpOnNonNumeric,
    AttemptedOpOnNonInt,
    NoMatchingFunction,
};

class TypeError : public std::runtime_error {
public:
    TypeChkError type;
    TypeError(TypeChkError t, const string& message) : runtime_error(message), type(t) {}
};

class TypeChecker {
public:
    TypeChecker(Scope* global_scope) : global_scope(global_scope) {
        current_scope = this->global_scope;
        in_loop = false;
    }

    void check(Program* program_node) {
        visit(program_node);
    }

private:
    Scope* global_scope;
    Scope* current_scope;
    string current_function_return_type;
    bool in_loop;

    bool is_numeric(const string& type) { return type == "int" || type == "float" || type == "double" || type == "char"; }
    bool is_integer(const string& type) { return type == "int" || type == "char"; }
    string get_wider_type(const string& t1, const string& t2) {
        if (t1 == "double" || t2 == "double") return "double";
        if (t1 == "float" || t2 == "float") return "float";
        return "int";
    }

    Expression* inject_cast(Expression* expr, const string& src_type, const string& dest_type, int line) {
        if (src_type == dest_type) return expr;

        if ((src_type == "int" || src_type == "bool" || src_type == "char") && 
            (dest_type == "float" || dest_type == "double")) {
            auto cast_node = new UnaryOp("sitofp", expr, line);
            cast_node->inferred_type = dest_type;
            return cast_node;
        }
        
        if ((src_type == "float" || src_type == "double") && 
            (dest_type == "int" || dest_type == "char")) {
            auto cast_node = new UnaryOp("fptosi", expr, line);
            cast_node->inferred_type = dest_type;
            return cast_node;
        }
        
        return expr;
    }

    vector<Symbol*> find_function_symbols(const string& name) {
        vector<Symbol*> candidates;
        Scope* s = current_scope;
        while(s) {
            auto range = s->symbols.equal_range(name);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second->kind == FUNCTION) {
                    candidates.push_back(it->second);
                }
            }
            s = s->parent;
        }
        return candidates;
    }

    Symbol* find_variable_symbol(const string& name) {
        Scope* s = current_scope;
        while(s) {
            auto range = s->symbols.equal_range(name);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second->kind == VARIABLE) return it->second;
            }
            s = s->parent;
        }
        return nullptr;
    }

    void enter_scope(const void* node_key) {
        if (current_scope->children_scopes.count(node_key)) {
            current_scope = current_scope->children_scopes[node_key];
        }
    }

    void exit_scope() {
        if (current_scope->parent) {
            current_scope = current_scope->parent;
        }
    }

    void visit(Program* node);
    void visit(FunctionDeclaration* node);
    void visit(BlockStatement* node);
    void visit(Statement* node);
    void visit(VariableDeclarationStatement* node);
    void visit(ExpressionStatement* node);
    void visit(IfStatement* node);
    void visit(WhileStatement* node);
    void visit(ForStatement* node);
    void visit(ReturnStatement* node);
    void visit(BreakStatement* node);
    void visit(ContinueStatement* node);
    string check(Expression* node);
    string check(BinaryOperation* node);
    string check(Assignment* node);
    string check(Identifier* node);
    string check(FunctionCall* node);
    string check(UnaryOp* node);
    string check(NumberLiteral* node);
    string check(CharLiteral* node);
    string check(StringLiteral* node) { node->inferred_type = "string"; return "string"; }
    string check(BoolLiteral* node) { node->inferred_type = "bool"; return "bool"; }
};

void TypeChecker::visit(Program* node) {
    for (auto g : node->globals) visit(g);
    for (auto f : node->functions) visit(f);
}

void TypeChecker::visit(FunctionDeclaration* node) {
    current_function_return_type = node->returnType;
    enter_scope(node);
    visit(node->body);
    exit_scope();
    current_function_return_type = "";
}

void TypeChecker::visit(VariableDeclarationStatement* node) {
    node->resolved_type = node->type;
    if (node->initializer) {
        string init_type = check(node->initializer);
        if (node->resolved_type != init_type && !(is_numeric(node->resolved_type) && is_numeric(init_type))) {
            throw TypeError(TypeChkError::ErroneousVarDecl, "Initializer type '" + init_type + "' does not match variable type '" + node->type + "' on line " + to_string(node->line));
        }
        node->initializer = inject_cast(node->initializer, init_type, node->resolved_type, node->line);
    }
    if (Symbol* sym = find_variable_symbol(node->name)) {
        sym->type_name = node->resolved_type;
    }
}

void TypeChecker::visit(BlockStatement* node) {
    enter_scope(node);
    for(auto s : node->statements) visit(s);
    exit_scope();
}

void TypeChecker::visit(Statement* node) {
    if (!node) return;
    if (auto p = dynamic_cast<BlockStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<VariableDeclarationStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ExpressionStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<IfStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<WhileStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ForStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ReturnStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<BreakStatement*>(node)) visit(p);
    else if (auto p = dynamic_cast<ContinueStatement*>(node)) visit(p);
}

void TypeChecker::visit(ExpressionStatement* node) { check(node->expression); }

void TypeChecker::visit(IfStatement* node) {
    string cond_type = check(node->condition);
    if (cond_type != "bool") {
        throw TypeError(TypeChkError::NonBooleanCondStmt, "If statement condition must be 'bool', but got '" + cond_type + "' on line " + to_string(node->line));
    }
    visit(node->thenBranch);
    if (node->elseBranch) visit(node->elseBranch);
}

void TypeChecker::visit(WhileStatement* node) {
    string cond_type = check(node->condition);
    if (cond_type != "bool") {
        throw TypeError(TypeChkError::NonBooleanCondStmt, "While loop condition must be 'bool', but got '" + cond_type + "' on line " + to_string(node->line));
    }
    bool prev_in_loop = in_loop;
    in_loop = true;
    visit(node->body);
    in_loop = prev_in_loop;
}

void TypeChecker::visit(ForStatement* node) {
    enter_scope(node);
    if(node->initializer) visit(node->initializer);
    if(node->condition) {
        string cond_type = check(node->condition);
        if (cond_type != "bool") {
            throw TypeError(TypeChkError::NonBooleanCondStmt, "For loop condition must be 'bool', but got '" + cond_type + "' on line " + to_string(node->line));
        }
    }
    if(node->increment) check(node->increment);
    bool prev_in_loop = in_loop;
    in_loop = true;
    visit(node->body);
    in_loop = prev_in_loop;
    exit_scope();
}

void TypeChecker::visit(ReturnStatement* node) {
    string return_type = "void";
    if (node->returnValue) {
        return_type = check(node->returnValue);
    }
    if (return_type != current_function_return_type && !(is_numeric(return_type) && is_numeric(current_function_return_type))) {
        throw TypeError(TypeChkError::ErroneousReturnType, "Return type '" + return_type + "' does not match function's declared return type '" + current_function_return_type + "' on line " + to_string(node->line));
    }
    if (node->returnValue) {
        node->returnValue = inject_cast(node->returnValue, return_type, current_function_return_type, node->line);
    }
}

void TypeChecker::visit(BreakStatement* node) {
    if (!in_loop) throw TypeError(TypeChkError::ErroneousBreak, "'break' statement used outside of a loop on line " + to_string(node->line));
}

void TypeChecker::visit(ContinueStatement* node) {
    if (!in_loop) throw TypeError(TypeChkError::ErroneousContinue, "'continue' statement used outside of a loop on line " + to_string(node->line));
}

string TypeChecker::check(Expression* node) {
    if (!node) return "void";
    if (auto p = dynamic_cast<BinaryOperation*>(node)) return check(p);
    if (auto p = dynamic_cast<Assignment*>(node)) return check(p);
    if (auto p = dynamic_cast<Identifier*>(node)) return check(p);
    if (auto p = dynamic_cast<FunctionCall*>(node)) return check(p);
    if (auto p = dynamic_cast<UnaryOp*>(node)) return check(p);
    if (auto p = dynamic_cast<NumberLiteral*>(node)) return check(p);
    if (auto p = dynamic_cast<StringLiteral*>(node)) return check(p);
    if (auto p = dynamic_cast<CharLiteral*>(node)) return check(p);
    if (auto p = dynamic_cast<BoolLiteral*>(node)) return check(p);
    return "void";
}

string TypeChecker::check(Assignment* node) {
    string var_type = check(node->identifier);
    string val_type = check(node->value);
    if (var_type != val_type && !(is_numeric(var_type) && is_numeric(val_type))) {
        throw TypeError(TypeChkError::InvalidAssignment, "Cannot assign type '" + val_type + "' to variable '" + node->identifier->name + "' of type '" + var_type + "' on line " + to_string(node->line));
    }
    node->value = inject_cast(node->value, val_type, var_type, node->line);

    node->inferred_type = var_type;
    return var_type;
}

string TypeChecker::check(Identifier* node) {
    Symbol* sym = find_variable_symbol(node->name);
    if (!sym) throw TypeError(TypeChkError::ErroneousVarDecl, "Undefined variable " + node->name);
    node->inferred_type = sym->type_name;
    return node->inferred_type;
}

string TypeChecker::check(NumberLiteral* node) {
    node->inferred_type = (node->value.find('.') != string::npos) ? "double" : "int";
    return node->inferred_type;
}

string TypeChecker::check(CharLiteral* node) {
    node->inferred_type = "char";
    return node->inferred_type;
}

string TypeChecker::check(UnaryOp* node) {
    string right_type = check(node->right);
    
    if (node->op == "p++" || node->op == "p--" || node->op == "++" || node->op == "--") {
         if (!is_numeric(right_type)) throw TypeError(TypeChkError::AttemptedOpOnNonNumeric, "Increment/Decrement requires numeric operand");
         node->inferred_type = right_type;
         return right_type;
    }

    if (node->op == "!") {
        if(right_type != "bool") throw TypeError(TypeChkError::ExpressionTypeMismatch, "Logical NOT '!' operator requires a boolean operand, but got '" + right_type + "' on line " + to_string(node->line));
        node->inferred_type = "bool";
        return "bool";
    }
    if (node->op == "-") {
        if(!is_numeric(right_type)) throw TypeError(TypeChkError::AttemptedOpOnNonNumeric, "Unary minus '-' operator requires a numeric operand, but got '" + right_type + "' on line " + to_string(node->line));
        node->inferred_type = right_type;
        return right_type;
    }
    node->inferred_type = "void";
    return "void";
}

string TypeChecker::check(FunctionCall* node) {
    vector<Symbol*> candidates = find_function_symbols(node->callee);
    if (candidates.empty()) {
        throw TypeError(TypeChkError::ErroneousVarDecl, "Call to undefined function '" + node->callee + "' on line " + to_string(node->line));
    }

    vector<string> arg_types;
    for (auto arg : node->arguments) {
        arg_types.push_back(check(arg));
    }

    Symbol* match = nullptr;
    for (Symbol* sym : candidates) {
        if (sym->params.size() != arg_types.size()) continue;
        bool params_match = true;
        for (size_t i = 0; i < sym->params.size(); ++i) {
            string param_t = sym->params[i].resolved_type.empty() ? sym->params[i].type : sym->params[i].resolved_type;
            string arg_t = arg_types[i];
            
            if (param_t != arg_t && !(is_numeric(param_t) && is_numeric(arg_t))) {
                params_match = false;
                break;
            }
        }
        if (params_match) {
            match = sym;
            break;
        }
    }

    if (!match) {
        string sig = node->callee + "(";
        for(size_t i=0; i<arg_types.size(); i++) sig += (i>0?", ":"") + arg_types[i];
        sig += ")";
        throw TypeError(TypeChkError::NoMatchingFunction, "No matching function for call " + sig + " on line " + to_string(node->line));
    }
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        string arg_type = arg_types[i];
        const Parameter& param = match->params[i];
        string param_type = !param.resolved_type.empty() ? param.resolved_type : param.type;
        
        node->arguments[i] = inject_cast(node->arguments[i], arg_type, param_type, node->line);
    }

    node->inferred_type = match->type_name;
    if (match->mangled_name != "main") {
        node->callee = match->mangled_name;
    }

    return node->inferred_type;
}

string TypeChecker::check(BinaryOperation* node) {
    string left_type = check(node->left);
    string right_type = check(node->right);
    const string& op = node->op;
    if (op == "+" || op == "-" || op == "*" || op == "/") {
        if (!is_numeric(left_type) || !is_numeric(right_type)) throw TypeError(TypeChkError::AttemptedOpOnNonNumeric, "Binary operator '" + op + "' requires numeric operands, but got '" + left_type + "' and '" + right_type + "' on line " + to_string(node->line));
        
        string result_type = get_wider_type(left_type, right_type);
        node->left = inject_cast(node->left, left_type, result_type, node->line);
        node->right = inject_cast(node->right, right_type, result_type, node->line);
        
        node->inferred_type = result_type;
        return node->inferred_type;
    }
    if (op == "%" || op == "<<" || op == ">>" || op == "&" || op == "|" || op == "^") {
        if (!is_integer(left_type) || !is_integer(right_type)) throw TypeError(TypeChkError::AttemptedOpOnNonInt, "Binary operator '" + op + "' requires integer operands, but got '" + left_type + "' and '" + right_type + "' on line " + to_string(node->line));
        node->inferred_type = "int";
        return "int";
    }
    if (op == "&&" || op == "||") {
        if (left_type != "bool" || right_type != "bool") throw TypeError(TypeChkError::ExpressionTypeMismatch, "Logical operator '" + op + "' requires boolean operands, but got '" + left_type + "' and '" + right_type + "' on line " + to_string(node->line));
        node->inferred_type = "bool";
        return "bool";
    }
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        if (left_type != right_type && !(is_numeric(left_type) && is_numeric(right_type))) throw TypeError(TypeChkError::ExpressionTypeMismatch, "Comparison operator '" + op + "' cannot compare incompatible types '" + left_type + "' and '" + right_type + "' on line " + to_string(node->line));
        string compare_type = get_wider_type(left_type, right_type);
        node->left = inject_cast(node->left, left_type, compare_type, node->line);
        node->right = inject_cast(node->right, right_type, compare_type, node->line);

        node->inferred_type = "bool";
        return "bool";
    }
    node->inferred_type = "void";
    return "void";
}