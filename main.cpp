#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include "lexer_raw.cpp"
using namespace std; 



string load_file(const string& path) {
    ifstream in(path, std::ios::binary);
    if (!in) throw runtime_error("Cannot open file: " + path);
    return {istreambuf_iterator<char>(in), istreambuf_iterator<char>()};
}

int main(int argc, char* argv[]) {
    string filepath = (argc > 1) ? argv[1] : "sample_C_code/sample2.c";
    string content = load_file(filepath);
    lexerRaw lex;
    cout<<" started tokenzing ";
    auto Tokens=lex.tokenizer(content);
    lex.printTokens(Tokens);
    return 0;
}
