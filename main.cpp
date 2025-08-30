#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std; 


string load_file(const string& path) {
    ifstream in(path, std::ios::binary);
    if (!in) throw runtime_error("Cannot open file: " + path);
    return {istreambuf_iterator<char>(in), istreambuf_iterator<char>()};
}

int main(int argc, char* argv[]) {
    std::string filepath = (argc > 1) ? argv[1] : "sample C code/sample.c";
    std::string content = load_file(filepath);

    return 0;
}
