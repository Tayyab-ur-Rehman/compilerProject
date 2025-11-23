#pragma once

#include "ast.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

using namespace std;

class IRGenerator {
public:
    IRGenerator();
    void generate(Program* program, const string& output_filename);

private:
    stringstream code_section;
    stringstream data_section;
    long long temp_counter;
    long long label_counter;
    
    // Map from variable name to QBE temporary holding its stack address
    map<string, string> string_literal_labels;
    map<string, string> var_map; 
    
    string current_func_end_label;
    string current_loop_continue_label;
    string current_loop_break_label;

    string new_temp();
    string new_label(const string& base);

    // Helper to get QBE type from C-like type
    string get_qbe_type(const string& type);
    string get_qbe_alloc_size(const string& type);

    // Visitor methods for AST nodes
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

    // Expression visitors return the SSA temporary holding the result
    string visit_expr(Expression* node);
    string visit_expr(BinaryOperation* node);
    string visit_expr(Assignment* node);
    string visit_expr(Identifier* node);
    string visit_expr(FunctionCall* node);
    string visit_expr(UnaryOp* node);
    string visit_expr(NumberLiteral* node);
    string visit_expr(StringLiteral* node);
    string visit_expr(CharLiteral* node);
    string visit_expr(BoolLiteral* node);
};
