#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

enum TokenType
{
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
    /* "static_assert" */ T_KW_STATIC_ASSERT,
    /* "struct" */ T_KW_STRUCT,
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
    /* include */ T_KW_INCLUDE,
    T_KW_stdlib,

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
    /* "-=" */ T_OP_MINUS_ASSIGN,
    /* "*=" */ T_OP_MUL_ASSIGN,
    /* "/=" */ T_OP_DIV_ASSIGN,
    /* "%=" */ T_OP_MOD_ASSIGN,
    T_KW_scanf,
    T_KW_printf,

    /* "#" */ T_PP_HASH,
    /* "##" */ T_PP_HASHHASH,
    /* "MACRO_NAME" define max sqr */ T_PP_IDENTIFIER, // not handled yet
    /* "123" (in preprocessor) */ T_PP_NUMBER,
    /* "\"str\"" (in preprocessor) */ T_PP_STRING,

    /* end-of-file */ T_EOF,
    /* invalid/unrecognized */ T_INVALID,
};

const map<string, TokenType> keywordMap = {
    {"static_assert", T_KW_STATIC_ASSERT},
    {"thread_local", T_KW_THREAD_LOCAL},
    {"constexpr", T_KW_CONSTEXPR},
    {"continue", T_KW_CONTINUE},
    {"register", T_KW_REGISTER},
    {"restrict", T_KW_RESTRICT},
    {"unsigned", T_KW_UNSIGNED},
    {"volatile", T_KW_VOLATILE},
    {"printf", T_KW_printf},
    {"scanf", T_KW_scanf},
    {"stdlib",T_KW_stdlib},

    {"alignas", T_KW_ALIGNAS},
    {"alignof", T_KW_ALIGNOF},
    {"default", T_KW_DEFAULT},
    {"nullptr", T_KW_NULLPTR},
    {"typedef", T_KW_TYPEDEF},
    {"include", T_KW_INCLUDE},
    {"extern", T_KW_EXTERN},
    {"sizeof", T_KW_SIZEOF},
    {"static", T_KW_STATIC},
    {"struct", T_KW_STRUCT},
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
    {"true", T_KW_TRUE},   // C23-ish
    {"false", T_KW_FALSE}, // C23-ish
    {"bool", T_KW_BOOL},   // C23-ish
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
};
const map<string, TokenType> twoCharOpMap = {
    {"->", T_OP_ARROW},
    {"++", T_OP_INC},
    {"--", T_OP_DEC},
    {"+=", T_OP_PLUS_ASSIGN},
    {"-=", T_OP_MINUS_ASSIGN},
    {"*=", T_OP_MUL_ASSIGN},
    {"/=", T_OP_DIV_ASSIGN},
    {"%=", T_OP_MOD_ASSIGN},
    {"==", T_OP_EQ},
    {"!=", T_OP_NEQ},
    {"<=", T_OP_LE},
    {">=", T_OP_GE},
    {"<<", T_OP_LSHIFT},
    {">>", T_OP_RSHIFT},
    {"&=", T_OP_AND_ASSIGN},
    {"|=", T_OP_OR_ASSIGN},
    {"^=", T_OP_XOR_ASSIGN},
    {"##", T_PP_HASHHASH}};
map<char, TokenType> singleCharTokens = {
    {'(', T_PARENL}, {')', T_PARENR}, {'{', T_BRACEL}, {'}', T_BRACER}, {'[', T_BRACKETL}, {']', T_BRACKETR}, {';', T_SEMICOLON}, {',', T_COMMA}, {':', T_COLON}, {'?', T_QUESTION}, {'.', T_OP_DOT}, {'+', T_OP_PLUS}, {'-', T_OP_MINUS}, {'*', T_OP_MUL}, {'/', T_OP_DIV}, {'%', T_OP_MOD}, {'=', T_OP_ASSIGN}, {'<', T_OP_LT}, {'>', T_OP_GT}, {'&', T_OP_AND}, {'|', T_OP_OR}, {'!', T_OP_NOT}, {'^', T_OP_XOR}, {'#', T_PP_HASH}};
string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case T_KW_ALIGNAS:
        return "KW_ALIGNAS";
    case T_KW_ALIGNOF:
        return "KW_ALIGNOF";
    case T_KW_AUTO:
        return "KW_AUTO";
    case T_KW_BOOL:
        return "KW_BOOL";
    case T_KW_BREAK:
        return "KW_BREAK";
    case T_KW_CASE:
        return "KW_CASE";
    case T_KW_CHAR:
        return "KW_CHAR";
    case T_KW_CONST:
        return "KW_CONST";
    case T_KW_CONSTEXPR:
        return "KW_CONSTEXPR";
    case T_KW_CONTINUE:
        return "KW_CONTINUE";
    case T_KW_DEFAULT:
        return "KW_DEFAULT";
    case T_KW_DO:
        return "KW_DO";
    case T_KW_DOUBLE:
        return "KW_DOUBLE";
    case T_KW_ELSE:
        return "KW_ELSE";
    case T_KW_ENUM:
        return "KW_ENUM";
    case T_KW_EXTERN:
        return "KW_EXTERN";
    case T_KW_FALSE:
        return "KW_FALSE";
    case T_KW_FLOAT:
        return "KW_FLOAT";
    case T_KW_FOR:
        return "KW_FOR";
    case T_KW_GOTO:
        return "KW_GOTO";
    case T_KW_IF:
        return "KW_IF";
    case T_KW_INLINE:
        return "KW_INLINE";
    case T_KW_INT:
        return "KW_INT";
    case T_KW_LONG:
        return "KW_LONG";
    case T_KW_NULLPTR:
        return "KW_NULLPTR";
    case T_KW_REGISTER:
        return "KW_REGISTER";
    case T_KW_RESTRICT:
        return "KW_RESTRICT";
    case T_KW_RETURN:
        return "KW_RETURN";
    case T_KW_SHORT:
        return "KW_SHORT";
    case T_KW_SIGNED:
        return "KW_SIGNED";
    case T_KW_SIZEOF:
        return "KW_SIZEOF";
    case T_KW_STATIC:
        return "KW_STATIC";
    case T_KW_STATIC_ASSERT:
        return "KW_STATIC_ASSERT";
    case T_KW_STRUCT:
        return "KW_STRUCT";
    case T_KW_SWITCH:
        return "KW_SWITCH";
    case T_KW_THREAD_LOCAL:
        return "KW_THREAD_LOCAL";
    case T_KW_TRUE:
        return "KW_TRUE";
    case T_KW_TYPEDEF:
        return "KW_TYPEDEF";
    case T_KW_INCLUDE:
        return "KW_INCLUDE";
    case T_KW_TYPEOF:
        return "KW_TYPEOF";
    case T_KW_TYPEOF_UNQUAL:
        return "KW_TYPEOF_UNQUAL";
    case T_KW_UNION:
        return "KW_UNION";
    case T_KW_UNSIGNED:
        return "KW_UNSIGNED";
    case T_KW_VOID:
        return "KW_VOID";
    case T_KW_VOLATILE:
        return "KW_VOLATILE";
    case T_KW_WHILE:
        return "KW_WHILE";
    case T_IDENTIFIER:
        return "IDENTIFIER";
    case T_INTLIT:
        return "INTLIT";
    case T_FLOATLIT:
        return "FLOATLIT";
    case T_CHARLIT:
        return "CHARLIT";
    case T_STRINGLIT:
        return "STRINGLIT";
    case T_PARENL:
        return "PARENL";
    case T_PARENR:
        return "PARENR";
    case T_BRACEL:
        return "BRACEL";
    case T_BRACER:
        return "BRACER";
    case T_BRACKETL:
        return "BRACKETL";
    case T_BRACKETR:
        return "BRACKETR";
    case T_SEMICOLON:
        return "SEMICOLON";
    case T_COMMA:
        return "COMMA";
    case T_COLON:
        return "COLON";
    case T_QUESTION:
        return "QUESTION";
    case T_OP_ARROW:
        return "OP_ARROW";
    case T_OP_DOT:
        return "OP_DOT";
    case T_OP_INC:
        return "OP_INC";
    case T_OP_DEC:
        return "OP_DEC";
    case T_OP_PLUS:
        return "OP_PLUS";
    case T_OP_MINUS:
        return "OP_MINUS";
    case T_OP_MUL:
        return "OP_MUL";
    case T_OP_DIV:
        return "OP_DIV";
    case T_OP_MOD:
        return "OP_MOD";
    case T_OP_ASSIGN:
        return "OP_ASSIGN";
    case T_OP_PLUS_ASSIGN:
        return "OP_PLUS_ASSIGN";
    case T_OP_EQ:
        return "OP_EQ";
    case T_OP_NEQ:
        return "OP_NEQ";
    case T_OP_LT:
        return "OP_LT";
    case T_OP_GT:
        return "OP_GT";
    case T_OP_LE:
        return "OP_LE";
    case T_OP_GE:
        return "OP_GE";
    case T_OP_LSHIFT:
        return "OP_LSHIFT";
    case T_OP_RSHIFT:
        return "OP_RSHIFT";
    case T_OP_LSHIFT_ASSIGN:
        return "OP_LSHIFT_ASSIGN";
    case T_OP_RSHIFT_ASSIGN:
        return "OP_RSHIFT_ASSIGN";
    case T_OP_AND:
        return "OP_AND";
    case T_OP_OR:
        return "OP_OR";
    case T_OP_NOT:
        return "OP_NOT";
    case T_OP_XOR:
        return "OP_XOR";
    case T_OP_AND_ASSIGN:
        return "OP_AND_ASSIGN";
    case T_OP_OR_ASSIGN:
        return "OP_OR_ASSIGN";
    case T_OP_XOR_ASSIGN:
        return "OP_XOR_ASSIGN";
    case T_OP_MINUS_ASSIGN:
        return "OP_MINUS_ASSIGN";
    case T_OP_MUL_ASSIGN:
        return "OP_MUL_ASSIGN";
    case T_OP_DIV_ASSIGN:
        return "OP_DIV_ASSIGN";
    case T_OP_MOD_ASSIGN:
        return "OP_MOD_ASSIGN";
    case T_PP_HASH:
        return "PP_HASH";
    case T_PP_HASHHASH:
        return "PP_HASHHASH";
    case T_PP_IDENTIFIER:
        return "PP_IDENTIFIER";
    case T_PP_NUMBER:
        return "PP_NUMBER";
    case T_PP_STRING:
        return "PP_STRING";
    case T_EOF:
        return "EOF";
    case T_INVALID:
        return "INVALID";
    default:
        return "UNKNOWN";
    }
}

struct Token
{
    TokenType type;
    string lexeme;
    int line;
    int column;
};

class lexerRaw
{

public:
    static bool isalpha(char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    static bool isdigit(char c)
    {
        return (c >= '0' && c <= '9');
    }
    static bool isalnum(char c)
    {
        return isalpha(c) || isdigit(c);
    }

    static bool isspace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }
    vector<Token> tokenizer(string &code)
    {
        vector<Token> tokens;
        int line = 1, col = 1;
        int i = 0;
        while (i < code.size())
        {
            if (isspace(code[i]))
            {
                if (code[i] == '\n')
                {
                    line++;
                    col = 1;
                }
                else
                {
                    col++;
                }
                i++;
                continue;
            }

            if (code[i] == '/')
            {
                if (i + 1 < code.size())
                {
                    if (code[i + 1] == '/')
                    {
                        int find = code.find("\n", i);
                        if (find != string::npos)
                            i = find + 1;
                        else
                            i = code.size();
                        continue;
                    }
                    else if (code[i + 1] == '*')
                    {
                        int find = code.find("*/", i);
                        if (find != string::npos)
                            i = find + 2;
                        else
                            i = code.size();
                        continue;
                    }
                }
            }

            if (i + 1 < code.size())
            {
                string two = code.substr(i, 2);
                auto it = twoCharOpMap.find(two);
                if (it != twoCharOpMap.end())
                {
                    tokens.push_back({it->second, two, line, col});
                    i += 2;
                    col += 2;
                    continue;
                }
            }

            // Identifiers and keywords
            if (isalpha(code[i]) || code[i] == '_')
            {
                int start = i;
                int startCol = col;
                while (i < code.size() && (isalnum(code[i]) || code[i] == '_' ) )
                {
                    i++;
                    col++;
                }
                string lexeme = code.substr(start, i - start);
                TokenType type = keywordMap.count(lexeme) ? keywordMap.at(lexeme) : T_IDENTIFIER;
                tokens.push_back({type, lexeme, line, startCol});
                continue;
            }

            // Error: identifier starting with digit
            if (isdigit(code[i]))
            {
                int start = i;
                int startCol = col;
                i++;
                col++;
                // Otherwise, it's a number literal (handle float too)
                bool isFloat = false;
                while (i < code.size() && isdigit(code[i]))
                {
                    i++;    
                    col++;
                }
                if(!isdigit(code[i])&& code[i]!='.' && !isspace(code[i]) && code[i]!=';')
                {
                    while (i < code.size() && !(isspace(code[i])||code[i]==';'))
                    {
                        i++;
                        col++;
                    }
                    string lexeme = code.substr(start, i - start);
                    tokens.push_back({T_INVALID, lexeme, line, startCol});
                    cout<<"Error: Invalid numeric literal at line "<<line<<", column "<<startCol<<lexeme<<endl;
                    break;
                }
                if (i < code.size() && code[i] == '.')
                {
                    isFloat = true;
                    i++;
                    col++;
                    while (i < code.size() && isdigit(code[i]))
                    {
                        i++;
                        col++;
                    }
                    if(!isdigit(code[i]) && !isspace(code[i]) && code[i]!=';')
                    {
                        while (i < code.size() && !(isspace(code[i])||code[i]==';'))
                        {
                            i++;
                            col++;
                        }
                        string lexeme = code.substr(start, i - start);
                        tokens.push_back({T_INVALID, lexeme, line, startCol});
                        cout<<"Error: Invalid numeric literal at line "<<line<<", column "<<startCol<<lexeme<<endl;
                        break;
                    }
                }
                string lexeme = code.substr(start, i - start);
                tokens.push_back({isFloat ? T_FLOATLIT : T_INTLIT, lexeme, line, startCol});
                continue;
            }

            if (code[i] == '"')
            {
                int start = i;
                int startCol = col;
                i++;
                col++;
                bool foundEndQuote = false;
                while (i < code.size() && code[i] != '"')
                {
                    
                    if ((code[i] == '\\' || code[i] == '\"') && i + 1 < code.size())
                        i++; // skip escaped char and \" to be considerd end of char*
                    
                    i++;
                    col++;
                }
                if (i < code.size() && code[i] == '"')
                    foundEndQuote = true;
                if (!foundEndQuote)
                {
                    cout << "Error: Unterminated string literal at line " << line << ", column " << startCol << endl;
                    break;
                }
                col++;
                tokens.push_back({T_STRINGLIT, code.substr(start, i - start), line, startCol});
                continue;
            }

            // Char literals
            if (code[i] == '\'')
            {
                int start = i;
                int startCol = col;
                i++;
                col++;
                bool foundEndQuote = false;
                if(i < code.size() && code[i] == '\\') // escape sequence
                {
                    i++;
                    col++;
                    if(i < code.size()) // skip escaped char
                    {
                        i++;
                        col++;
                    }
                }
                else if(i < code.size()) // normal char
                {
                    i++;
                    col++;
                }
                if (i < code.size() && code[i] == '\'')
                    foundEndQuote = true;
                if (!foundEndQuote)
                {
                    cout << "Error: Unterminated char literal at line " << line << ", column " << startCol << endl;
                    break;
                }
                
                i++;
                col++; // skip closing quote
                tokens.push_back({T_CHARLIT, code.substr(start, i - start), line, startCol});
                continue;
            }

            // 1 char
            if (singleCharTokens.count(code[i]))
            {
                tokens.push_back({singleCharTokens[code[i]], string(1, code[i]), line, col});
                i++;
                col++;
                continue;
            }

            // Unknown character
            tokens.push_back({T_INVALID, string(1, code[i]), line, col});
            i++;
            col++;
        }
        tokens.push_back({T_EOF, "", line, col});
        return tokens;
    }
    void printTokens(const vector<Token> &tokens)
    {
        for (const auto &tok : tokens)
        {
            cout << "[" << tokenTypeToString(tok.type); // print lexeme if its identifier ot literel
            if (tokenTypeToString(tok.type) == "IDENTIFIER" || tokenTypeToString(tok.type) == "INTLIT" || tokenTypeToString(tok.type) == "FLOATLIT" || tokenTypeToString(tok.type) == "CHARLIT" || tokenTypeToString(tok.type) == "STRINGLIT")
                cout << " (" << tok.lexeme << ") " << "]";
            else
                cout << " ]";
            // cout << " (line " << tok.line << ", col " << tok.column << ")\n";
       
        }
        cout<<endl;
    }
};