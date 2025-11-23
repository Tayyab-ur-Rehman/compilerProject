#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "tokens.h"

using namespace std;

struct Statement;
struct Expression;
struct BlockStatement;
struct Identifier;
struct VariableDeclarationStatement;

struct Expression {
    int line;
    string inferred_type;
    Expression(int l) : line(l), inferred_type("") {}
    virtual ~Expression() {} 
    virtual void print(int indent = 0) const = 0;
};

struct NumberLiteral : Expression {
    string value;
    NumberLiteral(string val, int l) : value(val), Expression(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "NumberLiteral(" << value
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
    }
};

struct StringLiteral : Expression {
    string value;
    StringLiteral(string val, int l) : value(val), Expression(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "StringLiteral(\"" << value << "\""
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
    }
};
struct CharLiteral : Expression {
    string value;
    CharLiteral(string val, int l) : value(val), Expression(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "CharLiteral(" << value
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
    }
};
struct BoolLiteral : Expression {
    bool value;
    BoolLiteral(bool val, int l) : value(val), Expression(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BoolLiteral(" << (value ? "true" : "false")
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
    }
};

struct Identifier : Expression {
    string name;
    Identifier(string n, int l) : name(n), Expression(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Identifier(" << name
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
    }
};

struct BinaryOperation : Expression {
    Expression* left;
    string op;
    Expression* right;

    BinaryOperation(Expression* l, string o, Expression* r, int ln) : left(l), op(o), right(r), Expression(ln) {}
    
    ~BinaryOperation() {
        delete left;
        delete right;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BinaryOperation(" << op
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct UnaryOp : Expression {
    string op;
    Expression* right;
    UnaryOp(string o, Expression* r, int l) : op(o), right(r), Expression(l) {}
    
    ~UnaryOp() {
        delete right;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "UnaryOp(" << op
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
        right->print(indent + 2);
    }
};

struct Assignment : Expression {
    Identifier* identifier;
    Expression* value;
    Assignment(Identifier* id, Expression* v, int l) : identifier(id), value(v), Expression(l) {}

    ~Assignment() {
        delete identifier;
        delete value;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Assignment(" << identifier->name
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
        value->print(indent + 2);
    }
};

struct FunctionCall : Expression {
    string callee;
    vector<Expression*> arguments;
    FunctionCall(string c, vector<Expression*> args, int l) : callee(c), arguments(args), Expression(l) {}

    ~FunctionCall() {
        for (auto arg : arguments) {
            delete arg;
        }
    }
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "FunctionCall(" << callee
             << ", type: " << (inferred_type.empty() ? "<unknown>" : inferred_type)
             << ") [line: " << line << "]" << endl;
        if (!arguments.empty()) {
            cout << string(indent + 2, ' ') << "Arguments:" << endl;
            for(const auto& arg : arguments) {
                arg->print(indent + 4);
            }
        }
    }
};

struct Statement {
    int line;
    Statement(int l) : line(l) {}
    virtual ~Statement() {}
    virtual void print(int indent = 0) const = 0;
};

struct BlockStatement : Statement {
    vector<Statement*> statements;
    BlockStatement(vector<Statement*> stmts, int l) : statements(stmts), Statement(l) {}

    ~BlockStatement() {
        for (auto stmt : statements) {
            delete stmt;
        }
    }
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Block [line: " << line << "] {" << endl;
        for(const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
        cout << string(indent, ' ') << "}" << endl;
    }
};

struct ExpressionStatement : Statement {
    Expression* expression;
    ExpressionStatement(Expression* expr, int l) : expression(expr), Statement(l) {}
    ~ExpressionStatement() {
        delete expression;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ExpressionStatement [line: " << line << "]" << endl;
        expression->print(indent + 2);
    }
};

struct VariableDeclarationStatement : Statement {
    string type;
    string name;
    Expression* initializer;
    string resolved_type;
    VariableDeclarationStatement(string t, string n, Expression* init, int l)
        : type(t), name(n), initializer(init), resolved_type(t), Statement(l) {}
    ~VariableDeclarationStatement() {
        if (initializer) {
            delete initializer;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "VariableDeclaration(" << name << ", type: " << type;
        if (!resolved_type.empty()) {
            cout << " -> " << resolved_type;
        }
        cout << ") [line: " << line << "]" << endl;
        if (initializer) {
            cout << string(indent + 2, ' ') << "Initializer:" << endl;
            initializer->print(indent + 4);
        }
    }
};

struct IfStatement : Statement {
    Expression* condition;
    Statement* thenBranch;
    Statement* elseBranch; 
    IfStatement(Expression* c, Statement* t, Statement* e, int l)
        : condition(c), thenBranch(t), elseBranch(e), Statement(l) {}
    ~IfStatement() {
        delete condition;
        delete thenBranch;
        if (elseBranch) {
            delete elseBranch;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IfStatement [line: " << line << "]" << endl;
        cout << string(indent + 2, ' ') << "Condition:" << endl;
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "Then:" << endl;
        thenBranch->print(indent + 4);
        if (elseBranch) {
            cout << string(indent + 2, ' ') << "Else:" << endl;
            elseBranch->print(indent + 4);
        }
    }
};

struct WhileStatement : Statement {
    Expression* condition;
    Statement* body;
    WhileStatement(Expression* c, Statement* b, int l)
        : condition(c), body(b), Statement(l) {}
    ~WhileStatement() {
        delete condition;
        delete body;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "WhileStatement [line: " << line << "]" << endl;
        cout << string(indent + 2, ' ') << "Condition:" << endl;
        condition->print(indent + 4);
        cout << string(indent + 2, ' ') << "Body:" << endl;
        body->print(indent + 4);
    }
};

struct ForStatement : Statement {
    Statement* initializer;
    Expression* condition;
    Expression* increment;
    Statement* body;

    ForStatement(Statement* init, Expression* cond, Expression* inc, Statement* b, int l)
        : initializer(init), condition(cond), increment(inc), body(b), Statement(l) {}
    ~ForStatement() {
        if(initializer) delete initializer;
        if(condition) delete condition;
        if(increment) delete increment;
        delete body;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ForStatement [line: " << line << "]" << endl;
        if(initializer) {
            cout << string(indent + 2, ' ') << "Initializer:" << endl;
            initializer->print(indent + 4);
        }
        if(condition) {
            cout << string(indent + 2, ' ') << "Condition:" << endl;
            condition->print(indent + 4);
        }
        if(increment) {
            cout << string(indent + 2, ' ') << "Increment:" << endl;
            increment->print(indent + 4);
        }
        cout << string(indent + 2, ' ') << "Body:" << endl;
        body->print(indent + 4);
    }
};
struct ReturnStatement : Statement {
    Expression* returnValue;
    ReturnStatement(Expression* val, int l) : returnValue(val), Statement(l) {}
    ~ReturnStatement() {
        if (returnValue) {
            delete returnValue;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ReturnStatement [line: " << line << "]" << endl;
        if (returnValue) {
            returnValue->print(indent + 2);
        }
    }
};
struct BreakStatement : Statement {
    BreakStatement(int l) : Statement(l) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BreakStatement [line: " << line << "]" << endl;
    }
};

struct ContinueStatement : Statement {
    ContinueStatement(int l) : Statement(l) {}
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ContinueStatement [line: " << line << "]" << endl;
    }
};

struct Parameter {
    string type;
    string name;
    int line;
    string resolved_type;
    Parameter(string t, string n, int l) : type(t), name(n), line(l), resolved_type(t) {}
    void print(int indent = 0) const {
        cout << string(indent, ' ') << "Param(" << name << ", type: " << type;
        if (!resolved_type.empty()) {
            cout << " -> " << resolved_type;
        }
        cout << ") [line: " << line << "]" << endl;
    }
};

struct FunctionDeclaration {
    string returnType;
    string name;
    vector<Parameter> params;
    BlockStatement* body;
    string resolved_return_type;
    int line;
    FunctionDeclaration(string rt, string n, vector<Parameter> p, BlockStatement* b, int l)
        : returnType(rt), name(n), params(p), body(b), resolved_return_type(rt), line(l) {}

    ~FunctionDeclaration() {
        delete body;
    }
    void print(int indent = 0) const {
        cout << string(indent, ' ') << "FunctionDeclaration(" << name << ", returns: " << returnType;
        if (!resolved_return_type.empty()) {
            cout << " -> " << resolved_return_type;
        }
        cout << ") [line: " << line << "]" << endl;
        if (!params.empty()) {
            cout << string(indent + 2, ' ') << "Parameters:" << endl;
            for (const auto& param : params) {
                param.print(indent + 4);
            }
        }
        if (body) {
            body->print(indent + 2);
        }
    }
};

struct Program {
    vector<FunctionDeclaration*> functions;
    vector<VariableDeclarationStatement*> globals;

    ~Program() {
        for (auto func : functions) {
            delete func;
        }
        for (auto glob : globals) {
            delete glob;
        }
    }
    void print(int indent = 0) const {
        cout << string(indent, ' ') << "Program" << endl;
        
        if (!globals.empty()) {
             cout << string(indent + 2, ' ') << "Globals:" << endl;
             for(const auto& glob : globals) {
                 glob->print(indent + 4);
             }
        }
        
        if (!functions.empty()) {
            cout << string(indent + 2, ' ') << "Functions:" << endl;
             for(const auto& func : functions) {
                 func->print(indent + 4);
             }
        }
    }
};