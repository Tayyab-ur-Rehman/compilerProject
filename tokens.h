#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>

enum TokenType
{
    
    T_KW_INCLUDE,
    T_KW_DEFINE,
    /* "alignas" */ T_KW_ALIGNAS,
    /* "alignof" */ T_KW_ALIGNOF,
    /* "auto" */ T_KW_AUTO,
    /* "bool" */ T_KW_BOOL,
    /* "break" */ T_KW_BREAK,
    /* "case" */ T_KW_CASE,
    /* "char" */ T_KW_CHAR,
    /* "const" */ T_KW_CONST,
    /* "constexpr" */ T_KW_CONSTEXPR,
    /* "continue" */ T_KW_CONTINUE,
    /* "default" */ T_KW_DEFAULT,
    /* "do" */ T_KW_DO,
    /* "double" */ T_KW_DOUBLE,
    /* "else" */ T_KW_ELSE,
    /* "enum" */ T_KW_ENUM,
    /* "extern" */ T_KW_EXTERN,
    /* "false" */ T_KW_FALSE,
    /* "float" */ T_KW_FLOAT,
    /* "for" */ T_KW_FOR,
    /* "goto" */ T_KW_GOTO,
    /* "if" */ T_KW_IF,
    /* "inline" */ T_KW_INLINE,
    /* "int" */ T_KW_INT,
    /* "long" */ T_KW_LONG,
    /* "nullptr" */ T_KW_NULLPTR,
    /* "register" */ T_KW_REGISTER,
    /* "restrict" */ T_KW_RESTRICT,
    /* "return" */ T_KW_RETURN,
    /* "short" */ T_KW_SHORT,
    /* "signed" */ T_KW_SIGNED,
    /* "sizeof" */ T_KW_SIZEOF,
    /* "static" */ T_KW_STATIC,
    
    /* "struct" */ T_KW_STATIC_ASSERT,
    /* "switch" */ T_KW_SWITCH,
    /* "thread_local" */ T_KW_THREAD_LOCAL,
    /* "true" */ T_KW_TRUE,
    /* "typedef" */ T_KW_TYPEDEF,
    /* "typeof" */ T_KW_TYPEOF,
    /* "typeof_unqual" */ T_KW_TYPEOF_UNQUAL,
    /* "union" */ T_KW_UNION,
    /* "unsigned" */ T_KW_UNSIGNED,
    /* "void" */ T_KW_VOID,
    /* "volatile" */ T_KW_VOLATILE,
    /* "while" */ T_KW_WHILE,

    /* "foo" (identifier example) */ T_IDENTIFIER,
    /* 42 */ T_INTLIT,
    /* 3.14 */ T_FLOATLIT,
    /* 'a' */ T_CHARLIT,
    /* "hello" */ T_STRINGLIT,

    /* "(" */ T_PARENL,
    /* ")" */ T_PARENR,
    /* "{" */ T_BRACEL,
    /* "}" */ T_BRACER,
    /* "[" */ T_BRACKETL,
    /* "]" */ T_BRACKETR,
    /* ";" */ T_SEMICOLON,
    /* "," */ T_COMMA,
    /* ":" */ T_COLON,
    /* "?" */ T_QUESTION,

    /* "->" */ T_OP_ARROW,
    /* "." */ T_OP_DOT,
    /* "++" */ T_OP_INC,
    /* "--" */ T_OP_DEC,
    /* "+" */ T_OP_PLUS,
    /* "-" */ T_OP_MINUS,
    /* "*" */ T_OP_MUL,
    /* "/" */ T_OP_DIV,
    /* "%" */ T_OP_MOD,
    /* "=" */ T_OP_ASSIGN,
    /* "+=" */ T_OP_PLUS_ASSIGN,
    /* "-=" */ T_OP_MINUS_ASSIGN,
    /* "*=" */ T_OP_MUL_ASSIGN,
    /* "/=" */ T_OP_DIV_ASSIGN,
    /* "%=" */ T_OP_MOD_ASSIGN,
    /* "==" */ T_OP_EQ,
    /* "!=" */ T_OP_NEQ,
    /* "<" */ T_OP_LT,
    /* ">" */ T_OP_GT,
    /* "<=" */ T_OP_LE,
    /* ">=" */ T_OP_GE,
    /* "<<" */ T_OP_LSHIFT,
    /* ">>" */ T_OP_RSHIFT,
    /* "<<=" */ T_OP_LSHIFT_ASSIGN,
    /* ">>=" */ T_OP_RSHIFT_ASSIGN,
    /* "&" */ T_OP_AND,
    /* "|" */ T_OP_OR,
    /* "!" */ T_OP_NOT,
    /* "^" */ T_OP_XOR,
    /* "&=" */ T_OP_AND_ASSIGN,
    /* "|=" */ T_OP_OR_ASSIGN,
    /* "^=" */ T_OP_XOR_ASSIGN,
    /* "~" */ T_OP_BITWISENOT,

    /* "#" */ T_PP_HASH,
    /* "##" */ T_PP_HASHHASH,
    /* "MACRO_NAME" */ T_PP_IDENTIFIER,
    /* "123" (in preprocessor) */ T_PP_NUMBER,
    /* "\"str\"" (in preprocessor) */ T_PP_STRING,

    /* end-of-file */ T_EOF,
    /* invalid/unrecognized */ T_INVALID,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& l, int ln, int col) 
        : type(t), lexeme(l), line(ln), column(col) {}
};

// Function declarations
std::vector<Token> tokenize(const std::string& input);
std::string tokenTypeToString(TokenType type);
std::string tokenToString(const Token& token);
std::string readFile(const std::string& filename);



#endif // LEXER_H