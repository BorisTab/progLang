//#include <iostream>
#include <fstream>
#include <codecvt>
#include "fileRead.h"
#include <regex>
#include "tree.h"

const int operatorsCount = 19;

class smileOp {
public:
    std::string opName = "";
    std::string symbol = "";
    int size = 0;

    smileOp(std::string opName, std::string symbol, int size) {
        this->opName = opName;
        this->symbol = symbol;
        this->size = size;
    }
};

int main() {
    char program[FILENAME_MAX] = "../emj.smile";

    std::ofstream debug ("../debug.txt");

    int size = getFileSize(program);
    char *buffer = new char [size] ();
    readFile(program, buffer, size);

    std::string buf;

    std::regex reg("[ |\n|\t]+");
    buf = std::regex_replace(buffer, reg, " ");
    debug << buf << std::endl;
    delete [] buffer;

    static const smileOp smileOperators[operatorsCount] = {
        smileOp("if", "❔", 3),
        smileOp("no", "❌", 3),
        smileOp("yes", "✅", 3),
        smileOp("func", "😻", 4),
        smileOp("leftFist", "🤛", 4),
        smileOp("rightFist", "🤜", 4),
        smileOp("print", "😱", 4),
        smileOp("openBrackets", "⏪", 3),
        smileOp("closeBrackets", "⏩", 3),
        smileOp("separator", "▶", 3),
        smileOp("comma", "✂", 3),
        smileOp("assignment", "✏", 3),
        smileOp("and", "⭐", 3),
        smileOp("or", "🆚", 4),
        smileOp("negative", "❕", 3),
        smileOp("scan", "👂",4),
        smileOp("global", "🌏",4),
        smileOp("return", "🔙",4),
        smileOp("circle", "🔁",4)
    };

    for(const auto & smileOperator : smileOperators) {
        std::cout << smileOperator.opName << smileOperator.size << std::endl;
    }

    debug.close();
    return 0;
}
