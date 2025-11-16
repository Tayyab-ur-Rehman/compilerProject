#pragma once

#include <vector>
#include <stdexcept>
#include "tokens.h" 
#include "ast.h"

enum class ParseErrorType { 
    UnexpectedEOF, FailedToFindToken, ExpectedTypeSpecifier, ExpectedIdentifier, 
    UnexpectedToken, ExpectedExpression, ExpectedSemicolonAfterStatement, 
    ExpectedLeftParenAfterKeyword, ExpectedRightParenAfterCondition, InvalidAssignmentTarget, 
    ExpectedLeftBraceForBody, ExpectedToken
};

class ParseError : public std::runtime_error {
public:
    ParseErrorType type;
    ParseError(ParseErrorType t, const string& message) : runtime_error(message), type(t) {}
};

class Parser {
public:
    Parser(const vector<Token>& tokens) : tokens(tokens), current(0) {}

    Program* parse_program() {
        Program* program = new Program();
        while (!is_at_end()) {
            int line = peek().line;
            if (!is_type_specifier()) {
                 throw ParseError(ParseErrorType::ExpectedTypeSpecifier, 
                    "Expected a type specifier for top-level declaration at line " + to_string(line));
            }
            string type = advance().lexeme;
            Token name_token = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected identifier for declaration");
            string name = name_token.lexeme;
            if (check(T_PARENL)) {
                program->functions.push_back(finish_parse_function(type, name, line));
            } else if (check(T_OP_ASSIGN) || check(T_SEMICOLON)) {
                program->globals.push_back(finish_parse_variable(type, name, line));
            } else {
                consume(T_PARENL, ParseErrorType::FailedToFindToken, "Expected '(' for function declaration or '=' or ';' for variable declaration");
            }
        }
        return program;
    }

private:
    const vector<Token>& tokens;
    size_t current;

    bool is_at_end() { return peek().type == T_EOF; }
    Token peek() { return tokens[current]; }
    Token previous() { return tokens[current - 1]; }
    Token advance() { if (!is_at_end()) current++; return previous(); }
    bool check(TokenType type) { if (is_at_end()) return false; return peek().type == type; }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    Token consume(TokenType type, ParseErrorType err_type, const string& message) {
        if (check(type)) return advance();
        if (is_at_end()) {
            throw ParseError(ParseErrorType::UnexpectedEOF, message + " (unexpected end of file)");
        }
        throw ParseError(err_type, message + " at line " + to_string(peek().line));
    }

    bool is_type_specifier() {
        TokenType t = peek().type;
        if (t == T_IDENTIFIER && peek().lexeme == "string") return true; 
        return t==T_KW_VOID || t==T_KW_CHAR || t==T_KW_INT || t==T_KW_FLOAT || t==T_KW_DOUBLE || t==T_KW_BOOL || t==T_KW_AUTO;
    }

    FunctionDeclaration* finish_parse_function(string returnType, string name, int line) {
        consume(T_PARENL, ParseErrorType::FailedToFindToken, "Expected '(' after function name");
        vector<Parameter> params;
        if (!check(T_PARENR)) {
            do {
                if (!is_type_specifier()) {
                    throw ParseError(ParseErrorType::ExpectedTypeSpecifier, "Expected parameter type.");
                }
                string param_type = advance().lexeme;
                Token param_name = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected parameter name");
                params.push_back(Parameter(param_type, param_name.lexeme, param_name.line));
            } while (match(T_COMMA));
        }
        consume(T_PARENR, ParseErrorType::FailedToFindToken, "Expected ')' after parameters");
        BlockStatement* body = parse_block_statement();
        return new FunctionDeclaration(returnType, name, params, body, line);
    }
    
    VariableDeclarationStatement* finish_parse_variable(string type, string name, int line) {
        Expression* initializer = NULL;
        if (match(T_OP_ASSIGN)) {
            initializer = parse_expression();
        }
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after variable declaration");
        return new VariableDeclarationStatement(type, name, initializer, line);
    }

    Statement* parse_statement() {
        int line = peek().line;
        if (match(T_KW_IF)) return parse_if_statement(line);
        if (match(T_KW_WHILE)) return parse_while_statement(line);
        if (match(T_KW_FOR)) return parse_for_statement(line);
        if (match(T_KW_RETURN)) return parse_return_statement(line);
        if (match(T_KW_BREAK)) return parse_break_statement(line);
        if (match(T_KW_CONTINUE)) return parse_continue_statement(line);
        if (check(T_BRACEL)) return parse_block_statement();
        if (is_type_specifier()) {
            return parse_variable_declaration_statement();
        }
        return parse_expression_statement();
    }
    
    Statement* parse_variable_declaration_statement() {
        int line = peek().line;
        string type = advance().lexeme;
        Token name_token = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected variable name");
        return finish_parse_variable(type, name_token.lexeme, line);
    }

    ExpressionStatement* parse_expression_statement() {
        int line = peek().line;
        Expression* expr = parse_expression();
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after expression");
        return new ExpressionStatement(expr, line);
    }

    BlockStatement* parse_block_statement() {
        int line = peek().line;
        consume(T_BRACEL, ParseErrorType::ExpectedLeftBraceForBody, "Expected '{' to start a block");
        vector<Statement*> statements;
        while (!check(T_BRACER) && !is_at_end()) {
            statements.push_back(parse_statement());
        }
        consume(T_BRACER, ParseErrorType::FailedToFindToken, "Expected '}' to end a block");
        return new BlockStatement(statements, line);
    }

    IfStatement* parse_if_statement(int line) {
        consume(T_PARENL, ParseErrorType::ExpectedLeftParenAfterKeyword, "Expected '(' after 'if'");
        Expression* condition = parse_expression();
        consume(T_PARENR, ParseErrorType::ExpectedRightParenAfterCondition, "Expected ')' after if condition");
        Statement* thenBranch = parse_statement();
        Statement* elseBranch = NULL;
        if (match(T_KW_ELSE)) {
            elseBranch = parse_statement();
        }
        return new IfStatement(condition, thenBranch, elseBranch, line);
    }
    
    WhileStatement* parse_while_statement(int line) {
        consume(T_PARENL, ParseErrorType::ExpectedLeftParenAfterKeyword, "Expected '(' after 'while'");
        Expression* condition = parse_expression();
        consume(T_PARENR, ParseErrorType::ExpectedRightParenAfterCondition, "Expected ')' after while condition");
        Statement* body = parse_statement();
        return new WhileStatement(condition, body, line);
    }

    ForStatement* parse_for_statement(int line) {
        consume(T_PARENL, ParseErrorType::ExpectedLeftParenAfterKeyword, "Expected '(' after 'for'");
        Statement* initializer = NULL;
        if (match(T_SEMICOLON)) { /* no initializer */ } 
        else if (is_type_specifier()) { initializer = parse_variable_declaration_statement(); }
        else { initializer = parse_expression_statement(); }
        
        Expression* condition = NULL;
        if (!check(T_SEMICOLON)) { condition = parse_expression(); }
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after loop condition");
        
        Expression* increment = NULL;
        if (!check(T_PARENR)) { increment = parse_expression(); }
        consume(T_PARENR, ParseErrorType::ExpectedRightParenAfterCondition, "Expected ')' after for clauses");
        
        Statement* body = parse_statement();
        return new ForStatement(initializer, condition, increment, body, line);
    }
    
    ReturnStatement* parse_return_statement(int line) {
        Expression* value = NULL;
        if (!check(T_SEMICOLON)) { value = parse_expression(); }
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after return value");
        return new ReturnStatement(value, line);
    }

    BreakStatement* parse_break_statement(int line) {
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after 'break'");
        return new BreakStatement(line);
    }

    ContinueStatement* parse_continue_statement(int line) {
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after 'continue'");
        return new ContinueStatement(line);
    }

    Expression* parse_expression() { return parse_assignment(); }

    Expression* parse_assignment() {
        Expression* expr = parse_logical_or();
        if (match(T_OP_ASSIGN)) {
            int line = previous().line; 
            Expression* value = parse_assignment();
            
            Identifier* id = dynamic_cast<Identifier*>(expr);
            if (id) {
                return new Assignment(id, value, line);
            }
            delete value;
            delete expr;
            throw ParseError(ParseErrorType::InvalidAssignmentTarget, "Invalid assignment target at line " + to_string(line));
        }
        return expr;
    }

    Expression* parse_logical_or() {
        Expression* expr = parse_logical_and();
        while (match(T_OP_OR)) {
            int line = previous().line; 
            string op = previous().lexeme;
            Expression* right = parse_logical_and();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }

    Expression* parse_logical_and() {
        Expression* expr = parse_equality();
        while (match(T_OP_AND)) {
            int line = previous().line;
            string op = previous().lexeme;
            Expression* right = parse_equality();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }

    Expression* parse_equality() {
        Expression* expr = parse_comparison();
        while (check(T_OP_EQ) || check(T_OP_NEQ)) {
            int line = peek().line; 
            advance();
            string op = previous().lexeme;
            Expression* right = parse_comparison();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }
    
    Expression* parse_comparison() {
        Expression* expr = parse_term();
        while (check(T_OP_LT) || check(T_OP_GT) || check(T_OP_LE) || check(T_OP_GE)) {
            int line = peek().line; 
            advance();
            string op = previous().lexeme;
            Expression* right = parse_term();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }

    Expression* parse_term() {
        Expression* expr = parse_factor();
        while (check(T_OP_PLUS) || check(T_OP_MINUS)) {
            int line = peek().line;
            advance();
            string op = previous().lexeme;
            Expression* right = parse_factor();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }
    
    Expression* parse_factor() {
        Expression* expr = parse_unary();
        while (check(T_OP_MUL) || check(T_OP_DIV) || check(T_OP_MOD)) {
            int line = peek().line;
            advance();
            string op = previous().lexeme;
            Expression* right = parse_unary();
            expr = new BinaryOperation(expr, op, right, line);
        }
        return expr;
    }

    Expression* parse_unary() {
        if (check(T_OP_NOT) || check(T_OP_MINUS) || check(T_OP_INC) || check(T_OP_DEC)) {
            int line = peek().line;
            advance();
            string op = previous().lexeme;
            Expression* right = parse_unary();
            return new UnaryOp(op, right, line);
        }
        return parse_call();
    }
    
    Expression* parse_call() {
        Expression* expr = parse_primary();
        if (match(T_PARENL)) {
            Identifier* id = dynamic_cast<Identifier*>(expr);
            if(id) {
                string callee_name = id->name;
                int line = id->line;
                delete id; 
                vector<Expression*> args;
                if (!check(T_PARENR)) {
                    do { args.push_back(parse_expression()); } while (match(T_COMMA));
                }
                consume(T_PARENR, ParseErrorType::FailedToFindToken, "Expected ')' after arguments.");
                return new FunctionCall(callee_name, args, line);
            }
        }
        return expr;
    }

    Expression* parse_primary() {
        int line = peek().line;
        if (match(T_INTLIT)) return new NumberLiteral(previous().lexeme, line);
        if (match(T_FLOATLIT)) return new NumberLiteral(previous().lexeme, line);
        if (match(T_STRINGLIT)) return new StringLiteral(previous().lexeme, line);
        if (match(T_KW_TRUE)) return new BoolLiteral(true, line);
        if (match(T_KW_FALSE)) return new BoolLiteral(false, line);
        if (match(T_IDENTIFIER)) return new Identifier(previous().lexeme, line);

        if (match(T_PARENL)) {
            Expression* expr = parse_expression();
            consume(T_PARENR, ParseErrorType::FailedToFindToken, "Expected ')' after expression.");
            return expr;
        }

        throw ParseError(ParseErrorType::ExpectedExpression, "Expected an expression at line " + to_string(peek().line));
    }
};