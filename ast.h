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
    virtual ~Expression() {} 
    virtual void print(int indent = 0) const = 0;
};

struct NumberLiteral : Expression {
    string value;
    NumberLiteral(string val) : value(val) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "NumberLiteral(" << value << ")" << endl;
    }
};

struct StringLiteral : Expression {
    string value;
    StringLiteral(string val) : value(val) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "StringLiteral(\"" << value << "\")" << endl;
    }
};

struct BoolLiteral : Expression {
    bool value;
    BoolLiteral(bool val) : value(val) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BoolLiteral(" << (value ? "true" : "false") << ")" << endl;
    }
};

struct Identifier : Expression {
    string name;
    Identifier(string n) : name(n) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Identifier(" << name << ")" << endl;
    }
};

struct BinaryOperation : Expression {
    Expression* left;
    string op;
    Expression* right;

    BinaryOperation(Expression* l, string o, Expression* r) : left(l), op(o), right(r) {}
    
    ~BinaryOperation() {
        delete left;
        delete right;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BinaryOperation(" << op << ")" << endl;
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct UnaryOp : Expression {
    string op;
    Expression* right;
    UnaryOp(string o, Expression* r) : op(o), right(r) {}
    
    ~UnaryOp() {
        delete right;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "UnaryOp(" << op << ")" << endl;
        right->print(indent + 2);
    }
};

struct Assignment : Expression {
    Identifier* identifier;
    Expression* value;
    Assignment(Identifier* id, Expression* v) : identifier(id), value(v) {}

    ~Assignment() {
        delete identifier;
        delete value;
    }

    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Assignment(" << identifier->name << ")" << endl;
        value->print(indent + 2);
    }
};

struct FunctionCall : Expression {
    string callee;
    vector<Expression*> arguments;
    FunctionCall(string c, vector<Expression*> args) : callee(c), arguments(args) {}

    ~FunctionCall() {
        for (size_t i = 0; i < arguments.size(); ++i) {
            delete arguments[i];
        }
    }
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "FunctionCall(" << callee << ")" << endl;
        if (!arguments.empty()) {
            cout << string(indent + 2, ' ') << "Arguments:" << endl;
            for(size_t i = 0; i < arguments.size(); ++i) {
                arguments[i]->print(indent + 4);
            }
        }
    }
};

struct Statement {
    virtual ~Statement() {}
    virtual void print(int indent = 0) const = 0;
};

struct BlockStatement : Statement {
    vector<Statement*> statements;
    BlockStatement(vector<Statement*> stmts) : statements(stmts) {}

    ~BlockStatement() {
        for (size_t i = 0; i < statements.size(); ++i) {
            delete statements[i];
        }
    }
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Block {" << endl;
        for(size_t i = 0; i < statements.size(); ++i) {
            statements[i]->print(indent + 2);
        }
        cout << string(indent, ' ') << "}" << endl;
    }
};

struct ExpressionStatement : Statement {
    Expression* expression;
    ExpressionStatement(Expression* expr) : expression(expr) {}
    ~ExpressionStatement() {
        delete expression;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ExpressionStatement" << endl;
        expression->print(indent + 2);
    }
};

struct VariableDeclarationStatement : Statement {
    string type;
    string name;
    Expression* initializer; 
    VariableDeclarationStatement(string t, string n, Expression* init)
        : type(t), name(n), initializer(init) {}
    ~VariableDeclarationStatement() {
        if (initializer) {
            delete initializer;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "VariableDeclaration(" << name << ", type: " << type << ")" << endl;
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
    IfStatement(Expression* c, Statement* t, Statement* e)
        : condition(c), thenBranch(t), elseBranch(e) {}
    ~IfStatement() {
        delete condition;
        delete thenBranch;
        if (elseBranch) {
            delete elseBranch;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "IfStatement" << endl;
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
    WhileStatement(Expression* c, Statement* b)
        : condition(c), body(b) {}
    ~WhileStatement() {
        delete condition;
        delete body;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "WhileStatement" << endl;
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

    ForStatement(Statement* init, Expression* cond, Expression* inc, Statement* b)
        : initializer(init), condition(cond), increment(inc), body(b) {}
    ~ForStatement() {
        if(initializer) delete initializer;
        if(condition) delete condition;
        if(increment) delete increment;
        delete body;
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ForStatement" << endl;
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
    ReturnStatement(Expression* val) : returnValue(val) {}
    ~ReturnStatement() {
        if (returnValue) {
            delete returnValue;
        }
    }
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ReturnStatement" << endl;
        if (returnValue) {
            returnValue->print(indent + 2);
        }
    }
};

struct BreakStatement : Statement {
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BreakStatement" << endl;
    }
};

struct ContinueStatement : Statement {
     void print(int indent = 0) const override {
        cout << string(indent, ' ') << "ContinueStatement" << endl;
    }
};
struct Parameter {
    string type;
    string name;
    void print(int indent = 0) const {
        cout << string(indent, ' ') << "Param(" << name << ", type: " << type << ")" << endl;
    }
};

struct FunctionDeclaration {
    string returnType;
    string name;
    vector<Parameter> params;
    BlockStatement* body;

    FunctionDeclaration(string rt, string n, vector<Parameter> p, BlockStatement* b)
        : returnType(rt), name(n), params(p), body(b) {}

    ~FunctionDeclaration() {
        delete body;
    }
    void print(int indent = 0) const {
    cout << string(indent, ' ') << "FunctionDeclaration(" << name << ", returns: " << returnType << ")" << endl;
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
        for (size_t i = 0; i < functions.size(); ++i) {
            delete functions[i];
        }
        for (size_t i = 0; i < globals.size(); ++i) {
            delete globals[i];
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



