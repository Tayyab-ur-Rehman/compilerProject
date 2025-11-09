#include "tokens.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cctype>
#include <algorithm>

using namespace std;

const map<string, TokenType> keywordMap = {
    {"static_assert", T_KW_STATIC_ASSERT},
    {"thread_local", T_KW_THREAD_LOCAL},
    {"constexpr", T_KW_CONSTEXPR},
    {"continue", T_KW_CONTINUE},
    {"register", T_KW_REGISTER},
    {"restrict", T_KW_RESTRICT},
    {"unsigned", T_KW_UNSIGNED},
    {"volatile", T_KW_VOLATILE},
    {"alignas", T_KW_ALIGNAS},
    {"alignof", T_KW_ALIGNOF},
    {"default", T_KW_DEFAULT},
    {"nullptr", T_KW_NULLPTR},
    {"typedef", T_KW_TYPEDEF},
    {"extern", T_KW_EXTERN},
    {"sizeof", T_KW_SIZEOF},
    {"static", T_KW_STATIC},
    {"struct", T_KW_STATIC_ASSERT},
    {"switch", T_KW_SWITCH},
    {"return", T_KW_RETURN},
    {"double", T_KW_DOUBLE},
    {"typeof", T_KW_TYPEOF},
    {"inline", T_KW_INLINE},
    {"signed", T_KW_SIGNED},
    {"float", T_KW_FLOAT},
    {"short", T_KW_SHORT},
    {"break", T_KW_BREAK},
    {"while", T_KW_WHILE},
    {"union", T_KW_UNION},
    {"const", T_KW_CONST},
    {"true", T_KW_TRUE},
    {"false", T_KW_FALSE},
    {"bool", T_KW_BOOL},
    {"goto", T_KW_GOTO},
    {"else", T_KW_ELSE},
    {"case", T_KW_CASE},
    {"enum", T_KW_ENUM},
    {"auto", T_KW_AUTO},
    {"long", T_KW_LONG},
    {"void", T_KW_VOID},
    {"char", T_KW_CHAR},
    {"int", T_KW_INT},
    {"for", T_KW_FOR},
    {"if", T_KW_IF},
    {"do", T_KW_DO},
    {"#include",T_KW_INCLUDE},
    {"#define",T_KW_DEFINE},
};

string tokenTypeToString(TokenType type) {
    static map<TokenType, string> typeToString = {
        {T_KW_ALIGNAS, "KW_ALIGNAS"},
        {T_KW_ALIGNOF, "KW_ALIGNOF"},
        {T_KW_AUTO, "KW_AUTO"},
        {T_KW_BOOL, "KW_BOOL"},
        {T_KW_BREAK, "KW_BREAK"},
        {T_KW_CASE, "KW_CASE"},
        {T_KW_CHAR, "KW_CHAR"},
        {T_KW_CONST, "KW_CONST"},
        {T_KW_CONSTEXPR, "KW_CONSTEXPR"},
        {T_KW_CONTINUE, "KW_CONTINUE"},
        {T_KW_DEFAULT, "KW_DEFAULT"},
        {T_KW_DO, "KW_DO"},
        {T_KW_DOUBLE, "KW_DOUBLE"},
        {T_KW_ELSE, "KW_ELSE"},
        {T_KW_ENUM, "KW_ENUM"},
        {T_KW_EXTERN, "KW_EXTERN"},
        {T_KW_FALSE, "KW_FALSE"},
        {T_KW_FLOAT, "KW_FLOAT"},
        {T_KW_FOR, "KW_FOR"},
        {T_KW_GOTO, "KW_GOTO"},
        {T_KW_IF, "KW_IF"},
        {T_KW_INLINE, "KW_INLINE"},
        {T_KW_INT, "KW_INT"},
        {T_KW_LONG, "KW_LONG"},
        {T_KW_NULLPTR, "KW_NULLPTR"},
        {T_KW_REGISTER, "KW_REGISTER"},
        {T_KW_RESTRICT, "KW_RESTRICT"},
        {T_KW_RETURN, "KW_RETURN"},
        {T_KW_SHORT, "KW_SHORT"},
        {T_KW_SIGNED, "KW_SIGNED"},
        {T_KW_SIZEOF, "KW_SIZEOF"},
        {T_KW_STATIC, "KW_STATIC"},
        {T_KW_STATIC_ASSERT, "KW_STATIC_ASSERT"},
        {T_KW_SWITCH, "KW_SWITCH"},
        {T_KW_THREAD_LOCAL, "KW_THREAD_LOCAL"},
        {T_KW_TRUE, "KW_TRUE"},
        {T_KW_TYPEDEF, "KW_TYPEDEF"},
        {T_KW_TYPEOF, "KW_TYPEOF"},
        {T_KW_TYPEOF_UNQUAL, "KW_TYPEOF_UNQUAL"},
        {T_KW_UNION, "KW_UNION"},
        {T_KW_UNSIGNED, "KW_UNSIGNED"},
        {T_KW_VOID, "KW_VOID"},
        {T_KW_VOLATILE, "KW_VOLATILE"},
        {T_KW_WHILE, "KW_WHILE"},
        {T_IDENTIFIER, "IDENTIFIER"},
        {T_INTLIT, "INTLIT"},
        {T_FLOATLIT, "FLOATLIT"},
        {T_CHARLIT, "CHARLIT"},
        {T_STRINGLIT, "STRINGLIT"},
        {T_PARENL, "PARENL"},
        {T_PARENR, "PARENR"},
        {T_BRACEL, "BRACEL"},
        {T_BRACER, "BRACER"},
        {T_BRACKETL, "BRACKETL"},
        {T_BRACKETR, "BRACKETR"},
        {T_SEMICOLON, "SEMICOLON"},
        {T_COMMA, "COMMA"},
        {T_COLON, "COLON"},
        {T_QUESTION, "QUESTION"},
        {T_OP_ARROW, "OP_ARROW"},
        {T_OP_DOT, "OP_DOT"},
        {T_OP_INC, "OP_INC"},
        {T_OP_DEC, "OP_DEC"},
        {T_OP_PLUS, "OP_PLUS"},
        {T_OP_MINUS, "OP_MINUS"},
        {T_OP_MUL, "OP_MUL"},
        {T_OP_DIV, "OP_DIV"},
        {T_OP_MOD, "OP_MOD"},
        {T_OP_ASSIGN, "OP_ASSIGN"},
        {T_OP_PLUS_ASSIGN, "OP_PLUS_ASSIGN"},
        {T_OP_MINUS_ASSIGN, "OP_MINUS_ASSIGN"},
        {T_OP_MUL_ASSIGN, "OP_MUL_ASSIGN"},
        {T_OP_DIV_ASSIGN, "OP_DIV_ASSIGN"},
        {T_OP_MOD_ASSIGN, "OP_MOD_ASSIGN"},
        {T_OP_EQ, "OP_EQ"},
        {T_OP_NEQ, "OP_NEQ"},
        {T_OP_LT, "OP_LT"},
        {T_OP_GT, "OP_GT"},
        {T_OP_LE, "OP_LE"},
        {T_OP_GE, "OP_GE"},
        {T_OP_LSHIFT, "OP_LSHIFT"},
        {T_OP_RSHIFT, "OP_RSHIFT"},
        {T_OP_LSHIFT_ASSIGN, "OP_LSHIFT_ASSIGN"},
        {T_OP_RSHIFT_ASSIGN, "OP_RSHIFT_ASSIGN"},
        {T_OP_AND, "OP_AND"},
        {T_OP_OR, "OP_OR"},
        {T_OP_NOT, "OP_NOT"},
        {T_OP_XOR, "OP_XOR"},
        {T_OP_AND_ASSIGN, "OP_AND_ASSIGN"},
        {T_OP_OR_ASSIGN, "OP_OR_ASSIGN"},
        {T_OP_XOR_ASSIGN, "OP_XOR_ASSIGN"},
        {T_OP_BITWISENOT, "OP_BITWISENOT"},
        {T_PP_HASH, "PP_HASH"},
        {T_PP_HASHHASH, "PP_HASHHASH"},
        {T_PP_IDENTIFIER, "PP_IDENTIFIER"},
        {T_PP_NUMBER, "PP_NUMBER"},
        {T_PP_STRING, "PP_STRING"},
        {T_EOF, "EOF"},
        {T_INVALID, "INVALID"},
        {T_KW_INCLUDE, "INCLUDE"},
        {T_KW_DEFINE, "DEFINE"}


    };
    return typeToString[type];
}

string tokenToString(const Token& token) {
    switch (token.type) {
        case T_KW_INCLUDE:
        case T_KW_DEFINE:
            return "T_" + tokenTypeToString(token.type) + "(\"" + token.lexeme + "\")";
        case T_IDENTIFIER:
        case T_INTLIT:
        case T_FLOATLIT:
        case T_CHARLIT:
        case T_STRINGLIT:
        case T_PP_IDENTIFIER:
        case T_PP_NUMBER:
        case T_PP_STRING:
            return "T_" + tokenTypeToString(token.type) + "(\"" + token.lexeme + "\")";
        default:
            return "T_" + tokenTypeToString(token.type);
    }
}

vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    size_t pos = 0;
    int line = 1;
    int column = 1;
    
    vector<pair<regex, TokenType>> patterns = {
        // Whitespace and comments
        { regex(R"(\s+)"), T_INVALID }, // Will be skipped
        { regex(R"(//.*)"), T_INVALID }, // Will be skipped
        { regex(R"(/\*[\s\S]*?\*/)"), T_INVALID }, // Will be skipped

        { regex(R"(#.*)"), T_INVALID }, // Find '#' and consume the rest of the line. Mark as INVALID to skip.

        // { regex(R"(#include\b)"), T_KW_INCLUDE },
        // { regex(R"(#define\b)"), T_KW_DEFINE },
        // Preprocessor directives
        { regex(R"(##)"), T_PP_HASHHASH },
        { regex(R"(#)"), T_PP_HASH },
        
        // Multi-character operators
        { regex(R"(<<=)"), T_OP_LSHIFT_ASSIGN },
        { regex(R"(>>=)"), T_OP_RSHIFT_ASSIGN },
        { regex(R"(\.\.\.)"), T_OP_DOT }, // Ellipsis
        { regex(R"(<<)"), T_OP_LSHIFT },
        { regex(R"(>>)"), T_OP_RSHIFT },
        { regex(R"(==)"), T_OP_EQ },
        { regex(R"(!=)"), T_OP_NEQ },
        { regex(R"(<=)"), T_OP_LE },
        { regex(R"(>=)"), T_OP_GE },
        { regex(R"(&&)"), T_OP_AND },
        { regex(R"(\|\|)"), T_OP_OR },
        { regex(R"(\+\+)"), T_OP_INC },
        { regex(R"(\-\-)"), T_OP_DEC },
        { regex(R"(\+=)"), T_OP_PLUS_ASSIGN },
        { regex(R"(\-=)"), T_OP_MINUS_ASSIGN },
        { regex(R"(\*=)"), T_OP_MUL_ASSIGN },
        { regex(R"(/=)"), T_OP_DIV_ASSIGN },
        { regex(R"(%=)"), T_OP_MOD_ASSIGN },
        { regex(R"(&=)"), T_OP_AND_ASSIGN },
        { regex(R"(\|=)"), T_OP_OR_ASSIGN },
        { regex(R"(\^=)"), T_OP_XOR_ASSIGN },
        { regex(R"(->)"), T_OP_ARROW },
        
        // Single-character operators and punctuation
        { regex(R"(\()"), T_PARENL },
        { regex(R"(\))"), T_PARENR },
        { regex(R"(\{)"), T_BRACEL },
        { regex(R"(\})"), T_BRACER },
        { regex(R"(\[)"), T_BRACKETL },
        { regex(R"(\])"), T_BRACKETR },
        { regex(R"(;)"), T_SEMICOLON },
        { regex(R"(,)"), T_COMMA },
        { regex(R"(:)"), T_COLON },
        { regex(R"(\?)"), T_QUESTION },
        { regex(R"(\.)"), T_OP_DOT },
        { regex(R"(\+)"), T_OP_PLUS },
        { regex(R"(\-)"), T_OP_MINUS },
        { regex(R"(\*)"), T_OP_MUL },
        { regex(R"(/)"), T_OP_DIV },
        { regex(R"(%)"), T_OP_MOD },
        { regex(R"(=)"), T_OP_ASSIGN },
        { regex(R"(<)"), T_OP_LT },
        { regex(R"(>)"), T_OP_GT },
        { regex(R"(&)"), T_OP_AND },
        { regex(R"(\|)"), T_OP_OR },
        { regex(R"(!)"), T_OP_NOT },
        { regex(R"(\^)"), T_OP_XOR },
        { regex(R"(~)"), T_OP_BITWISENOT },
        
        // Literals and Identifiers (Identifier must be last)
        { regex(R"(\b\d+\.\d*([eE][-+]?\d+)?\b)"), T_FLOATLIT },
        { regex(R"(\b\d+\b)"), T_INTLIT },
        { regex(R"([a-zA-Z_][a-zA-Z0-9_]*)"), T_IDENTIFIER }
    };

    while (pos < input.size()) {
 // Handle character literals
        if (input[pos] == '\'') {
            int start_line = line;
            int start_column = column;
            
            string char_lit;
            pos++;
            column++;
            
            while (pos < input.size() && input[pos] != '\'') {
                if (input[pos] == '\\') {
                    char_lit += input[pos];
                    pos++;
                    column++;
                    if (pos < input.size()) {
                        char_lit += input[pos];
                        pos++;
                        column++;
                    }
                } else {
                    char_lit += input[pos];
                    pos++;
                    column++;
                }
                
                if (pos >= input.size()) {
                    throw runtime_error("Unterminated character literal at line " + to_string(start_line) + 
                                       ", column " + to_string(start_column));
                }
            }
            
            if (pos < input.size() && input[pos] == '\'') {
                pos++;
                column++;
                tokens.push_back(Token(T_CHARLIT, char_lit, start_line, start_column));
            }
            continue;
        }
        
  // Handle string literals
        if (input[pos] == '"') {
            int start_line = line;
            int start_column = column;
            
            string str_lit;
            pos++;
            column++;
            
            while (pos < input.size() && input[pos] != '"') {
                if (input[pos] == '\\') {
                    str_lit += input[pos];
                    pos++;
                    column++;
                    if (pos < input.size()) {
                        str_lit += input[pos];
                        pos++;
                        column++;
                    }
                } else if (input[pos] == '\n') {
                    line++;
                    column = 1;
                    str_lit += input[pos];
                    pos++;
                } else {
                    str_lit += input[pos];
                    pos++;
                    column++;
                }
                
                if (pos >= input.size()) {
                    throw runtime_error("Unterminated string literal at line " + to_string(start_line) + 
                                       ", column " + to_string(start_column));
                }
            }
            
            if (pos < input.size() && input[pos] == '"') {
                pos++;
                column++;
                tokens.push_back(Token(T_STRINGLIT, str_lit, start_line, start_column));
            }
            continue;
        }
        
  // Handle other tokens with regex
        bool matched = false;
        for (auto& pattern : patterns) {
            regex re = pattern.first;
            smatch match;
            string remaining = input.substr(pos);
            
            if (regex_search(remaining, match, re, regex_constants::match_continuous)) {
                string lexeme = match.str();
                TokenType type = pattern.second;
                
   // Update line and column counters
                for (char c : lexeme) {
                    if (c == '\n') {
                        line++;
                        column = 1;
                    } else {
                        column++;
                    }
                }
                
          // Skip whitespace and comments
                if (type == T_INVALID) {
                    pos += lexeme.size();
                    matched = true;
                    break;
                }
                
          // Check if identifier is a keyword
                if (type == T_IDENTIFIER) {
                    if (lexeme[0] == '_' && lexeme.size() > 1 && isdigit(lexeme[1])) {
                        throw runtime_error("Invalid identifier: " + lexeme + " at line " + 
                                           to_string(line) + ", column " + to_string(column - lexeme.size()));
                    }
                    
                    auto it = keywordMap.find(lexeme);
                    if (it != keywordMap.end()) {
                        type = it->second;
                    }
                }
                
                tokens.push_back(Token(type, lexeme, line, column - lexeme.size()));
                pos += lexeme.size();
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            throw runtime_error("Unexpected character: '" + string(1, input[pos]) + "' at line " + 
                               to_string(line) + ", column " + to_string(column));
        }
    }
    
    tokens.push_back(Token(T_EOF, "", line, column));
    return tokens;
}

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }
    
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         cerr << "Usage: " << argv[0] << " <filename.c>" << endl;
//         return 1;
//     }
    
//     try {
//         string input = readFile(argv[1]);
//         vector<Token> tokens = tokenize(input);
        
//         for (const auto& token : tokens) {
//             cout << tokenToString(token) << ", ";
//         }
//         cout << endl;
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//         return 1;
//     }
    
//     return 0;
// }