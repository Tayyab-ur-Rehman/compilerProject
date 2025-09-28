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
            if (!is_type_specifier()) {
                 throw ParseError(ParseErrorType::ExpectedTypeSpecifier, 
                    "Expected a type specifier for top-level declaration at line " + to_string(peek().line));
            }
            string type = advance().lexeme;
            Token name_token = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected identifier for declaration");
            string name = name_token.lexeme;
            if (check(T_PARENL)) {
                program->functions.push_back(finish_parse_function(type, name));
            } else if (check(T_OP_ASSIGN) || check(T_SEMICOLON)) {
                program->globals.push_back(finish_parse_variable(type, name));
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
        return t==T_KW_VOID || t==T_KW_CHAR || t==T_KW_INT || t==T_KW_FLOAT || t==T_KW_DOUBLE || t==T_KW_BOOL || t==T_KW_AUTO;
    }

    FunctionDeclaration* finish_parse_function(string returnType, string name) {
        consume(T_PARENL, ParseErrorType::FailedToFindToken, "Expected '(' after function name");
        vector<Parameter> params;
        if (!check(T_PARENR)) {
            do {
                if (!is_type_specifier()) {
                    throw ParseError(ParseErrorType::ExpectedTypeSpecifier, "Expected parameter type.");
                }
                string param_type = advance().lexeme;
                Token param_name = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected parameter name");
                Parameter p = {param_type, param_name.lexeme};
                params.push_back(p);
            } while (match(T_COMMA));
        }
        consume(T_PARENR, ParseErrorType::FailedToFindToken, "Expected ')' after parameters");
        BlockStatement* body = parse_block_statement();
        return new FunctionDeclaration(returnType, name, params, body);
    }
    
    VariableDeclarationStatement* finish_parse_variable(string type, string name) {
        Expression* initializer = NULL;
        if (match(T_OP_ASSIGN)) {
            initializer = parse_expression();
        }
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after variable declaration");
        return new VariableDeclarationStatement(type, name, initializer);
    }

    Statement* parse_statement() {
        if (match(T_KW_IF)) return parse_if_statement();
        if (match(T_KW_WHILE)) return parse_while_statement();
        if (match(T_KW_FOR)) return parse_for_statement();
        if (match(T_KW_RETURN)) return parse_return_statement();
        if (match(T_KW_BREAK)) return parse_break_statement();
        if (match(T_KW_CONTINUE)) return parse_continue_statement();
        if (check(T_BRACEL)) return parse_block_statement();
        if (is_type_specifier()) {
            string type = advance().lexeme;
            Token name_token = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected variable name");
            return finish_parse_variable(type, name_token.lexeme);
        }
        return parse_expression_statement();
    }
    
    Statement* parse_variable_declaration_statement() {
        string type = advance().lexeme;
        Token name_token = consume(T_IDENTIFIER, ParseErrorType::ExpectedIdentifier, "Expected variable name");
        return finish_parse_variable(type, name_token.lexeme);
    }

    ExpressionStatement* parse_expression_statement() {
        Expression* expr = parse_expression();
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after expression");
        return new ExpressionStatement(expr);
    }

    BlockStatement* parse_block_statement() {
        consume(T_BRACEL, ParseErrorType::ExpectedLeftBraceForBody, "Expected '{' to start a block");
        vector<Statement*> statements;
        while (!check(T_BRACER) && !is_at_end()) {
            statements.push_back(parse_statement());
        }
        consume(T_BRACER, ParseErrorType::FailedToFindToken, "Expected '}' to end a block");
        return new BlockStatement(statements);
    }

    IfStatement* parse_if_statement() {
        consume(T_PARENL, ParseErrorType::ExpectedLeftParenAfterKeyword, "Expected '(' after 'if'");
        Expression* condition = parse_expression();
        consume(T_PARENR, ParseErrorType::ExpectedRightParenAfterCondition, "Expected ')' after if condition");
        Statement* thenBranch = parse_statement();
        Statement* elseBranch = NULL;
        if (match(T_KW_ELSE)) {
            elseBranch = parse_statement();
        }
        return new IfStatement(condition, thenBranch, elseBranch);
    }
    
    WhileStatement* parse_while_statement() {
        consume(T_PARENL, ParseErrorType::ExpectedLeftParenAfterKeyword, "Expected '(' after 'while'");
        Expression* condition = parse_expression();
        consume(T_PARENR, ParseErrorType::ExpectedRightParenAfterCondition, "Expected ')' after while condition");
        Statement* body = parse_statement();
        return new WhileStatement(condition, body);
    }

    ForStatement* parse_for_statement() {
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
        return new ForStatement(initializer, condition, increment, body);
    }
    
    ReturnStatement* parse_return_statement() {
        Expression* value = NULL;
        if (!check(T_SEMICOLON)) { value = parse_expression(); }
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after return value");
        return new ReturnStatement(value);
    }

    BreakStatement* parse_break_statement() {
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after 'break'");
        return new BreakStatement();
    }

    ContinueStatement* parse_continue_statement() {
        consume(T_SEMICOLON, ParseErrorType::ExpectedSemicolonAfterStatement, "Expected ';' after 'continue'");
        return new ContinueStatement();
    }

    Expression* parse_expression() { return parse_assignment(); }

    Expression* parse_assignment() {
        Expression* expr = parse_logical_or();// task of shafay
        if (match(T_OP_ASSIGN)) {
            Expression* value = parse_assignment();
            Identifier* id = (Identifier*)(expr);
            if (id) {
                return new Assignment(id, value);
            }
            delete value;
            delete expr;
            throw ParseError(ParseErrorType::InvalidAssignmentTarget, "Invalid assignment target.");
        }
        return expr;
    }

    // shafay task .......
};