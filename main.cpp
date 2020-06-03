#include <iostream>
#include <fstream>
#include <codecvt>
#include "fileRead.h"
#include <regex>
#include "tree.h"

int main(const int argc, const char *argv[]) {
    const char *program = argv[argc - 2];

//    std::ofstream debug ("../debug.txt");

    int size = getFileSize(program);
    char *buffer = new char [size] ();
    readFile(program, buffer, size + 100);

//    for(const auto & smileOperator : smileOperators) {
//        std::cout << smileOperator.opName << smileOperator.symbol.length() << std::endl;
//    }

    tree::Tree <char *> langTree = {}; //('L', "AST.txt");
    langTree.getG(buffer);
//    langTree.dump();
    langTree.saveTo(argv[argc - 1]);

//    debug.close();
    delete [] buffer;
    return 0;
}
