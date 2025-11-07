program          ::= { global_declaration } EOF

global_declaration ::= type_specifier IDENTIFIER ( function_declaration | variable_declaration )

function_declaration ::= "(" [ parameter_list ] ")" block_statement
parameter_list       ::= parameter { "," parameter }
parameter            ::= type_specifier IDENTIFIER

variable_declaration ::= [ "=" expression ] ";"

statement        ::= expression_statement
                 | if_statement
                 | while_statement
                 | for_statement
                 | return_statement
                 | break_statement
                 | continue_statement
                 | block_statement
                 | type_specifier IDENTIFIER variable_declaration

expression_statement ::= expression ";"
block_statement      ::= "{" { statement } "}"

if_statement     ::= "if" "(" expression ")" statement [ "else" statement ]
while_statement  ::= "while" "(" expression ")" statement
for_statement    ::= "for" "(" [for_initializer] ";" [expression] ";" [expression] ")" statement
for_initializer  ::= type_specifier IDENTIFIER variable_declaration | expression_statement

return_statement ::= "return" [ expression ] ";"
break_statement  ::= "break" ";"
continue_statement ::= "continue" ";"

// Expressions (ordered by precedence, lowest to highest)
expression       ::= assignment 
assignment       ::= logical_or [ "=" assignment ]
logical_or       ::= logical_and { "||" logical_and }
logical_and      ::= equality { "&&" equality }
equality         ::= comparison { ("==" | "!=") comparison }
comparison       ::= term { ("<" | ">" | "<=" | ">=") term }
term             ::= factor { ("+" | "-") factor } 
factor           ::= unary { ("*" | "/" | "%") unary }
unary            ::= ("!" | "-") unary | call
call             ::= primary [ "(" [argument_list] ")" ]
argument_list    ::= expression { "," expression }
primary          ::= INTLIT | FLOATLIT | STRINGLIT | "true" | "false" | IDENTIFIER | "("expression ")"

type_specifier   ::= "void" | "char" | "int" | "float" | "double" | "bool" | "auto"