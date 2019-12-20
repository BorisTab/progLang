//
// Created by boristab on 08.11.2019.
//
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cmath>

const int operatorsCount = 23;

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

static const smileOp smileOperators[operatorsCount] = {
        smileOp("if", "❔", 3),               // 0
        smileOp("no", "❌", 3),               // 1
        smileOp("yes", "✅", 3),              // 2
        smileOp("func", "😻", 4),             // 3
        smileOp("leftFist", "🤛", 4),         // 4
        smileOp("rightFist", "🤜", 4),        // 5
        smileOp("print", "😱", 4),            // 6
        smileOp("openBrackets", "⏪", 3),     // 7
        smileOp("closeBrackets", "⏩", 3),    // 8
        smileOp("separator", "▶", 3),        // 9
        smileOp("comma", "✂", 3),            // 10
        smileOp("assignment", "✏", 3),       // 11
        smileOp("and", "⭐", 3),              // 12
        smileOp("or", "🆚", 4),               // 13
        smileOp("negative", "❕", 3),         // 14
        smileOp("scan", "👂",4),              // 15
        smileOp("global", "🌏",4),            // 16
        smileOp("return", "🔙",4),            // 17
        smileOp("circle", "🔁",4),            // 18
        smileOp("sqrt", "✔", 3),              // 19      
        smileOp("==", "🆔", 4),               // 20 
        smileOp("<", "◀", 3),                // 21
        smileOp("main", "🏁", 4)              // 22
};

namespace tree {
#include "fileRead.h"

    enum errors {
        SUCCESS = 0,
        FILE_OPEN_FAILED = 1,
        NOT_FOUND_TREE_IN_FILE = 2,
        WRONG_KEY = 3,
        SYNTAX_ERROR = 4,
    };

    enum nodeTypes {
        NO_TYPE = 0,
        NUMBER = 1,
        MATH_OPERATION = 2,
        VARIABLE = 3,
        SYNTAX_OPERATION = 4,
    };

#define DEF_CMD(name, num, sign, code, texCode) \
    name = num,

    enum operations {
        NOTHING = 0,
#include "operations.h"

#undef DEF_CMD
    };

#define DEF_CMD(name, num, sign, code, texCode) \
    case num: return sign;

    const char *getEnumName(int op) {
        switch (op) {
#include "operations.h"

#undef DEF_CMD
            default: return nullptr;
        }
    }

    const char dumpFilePath[FILENAME_MAX] = "../TreeDumpFile.txt";

    int spaceN (const char *buffer);

    template <class elemType>
    class Node {
    public:
        Node <elemType> *parent = nullptr;
        Node <elemType> *leftChild = nullptr;
        Node <elemType> *rightChild = nullptr;
        elemType value = {};
        int nodeType = 0;

        explicit Node(elemType value) {
            this->value = value;
        }
    };


    template <class elemType>
    class Tree {
    private:
        Node <elemType> *root = nullptr;
        char *s = nullptr;

        void printNodeInit(std::ofstream *dumpFile, Node <elemType> *node) {
            assert(node);
            assert(dumpFile);



            *dumpFile << "node_" << node << " [shape=record, label=\" { "<< node
                      << " | { Val: ";

//            if (node->nodeType == VARIABLE) *dumpFile << node->value;
//            else if (node->nodeType == MATH_OPERATION) *dumpFile << node->value;
            *dumpFile << node->value;

            *dumpFile << " | Type: " << node->nodeType
                      << " } | { left: " << node->leftChild
                      << " | right: " << node->rightChild << " } } \"];\n";

            if (node->leftChild) printNodeInit(dumpFile, node->leftChild);
            if (node->rightChild) printNodeInit(dumpFile, node->rightChild);
        }

        void printNodeRel(std::ofstream *dumpFile, Node <elemType> *node) {
            if (node->leftChild) *dumpFile << "node_" << node << "-- node_" << node->leftChild << ";\n";
            if (node->rightChild) *dumpFile << "node_" << node << "-- node_" << node->rightChild << ";\n";

            if (node->leftChild) printNodeRel(dumpFile, node->leftChild);
            if (node->rightChild) printNodeRel(dumpFile, node->rightChild);
        }

        void saveNode(std::ofstream *outFile, Node <elemType> *node) {
            assert(outFile);
            assert(node);

            *outFile << "{ " << node->value << " ";

            if (!node->leftChild && !node->rightChild) {
                *outFile << "} ";
                return;
            }

            if (node->leftChild) saveNode(outFile, node->leftChild);
            else *outFile << "{ @ } ";

            if (node->rightChild) saveNode(outFile, node->rightChild);
            else *outFile << "{ @ } ";

            *outFile << "} ";
        }

        void writeNode(char **buffer, Node <elemType> *node) {
            (*buffer) += spaceN(*buffer);

            if (!strncmp(*buffer, "{ @ }", 5)) (*buffer) += 5 + spaceN(*buffer + 5);
            else if (**buffer == '{'){
                (*buffer) += 1 + spaceN((*buffer) + 1);
                *(strchr(*buffer, ' ')) = '\0';

                node->leftChild = newNode(*buffer);
                (*buffer) += strlen(*buffer) + 1;
                writeNode(buffer, node->leftChild);
            }

            (*buffer) += spaceN(*buffer);
            if (!strncmp(*buffer, "{ @ }", 5)) (*buffer) += 5 + spaceN(*buffer + 5);
            else if (**buffer == '{'){
                (*buffer) += 1 + spaceN((*buffer) + 1);
                *(strchr(*buffer, ' ')) = '\0';

                node->rightChild = newNode(*buffer);
                (*buffer) += strlen(*buffer) + 1;
                writeNode(buffer, node->rightChild);
            }

            if (**buffer == '}') {
                (*buffer) += 1 + spaceN(*buffer + 1);
                return;
            }
        }

        void prefixRead(const char *inPath) {
            int fileSize = getFileSize(inPath);

            char *buffer = new char[fileSize] ();
            char *bufferStart = buffer;
            readFile(inPath, buffer, fileSize);

            getG(buffer);
        }

        Node <elemType> *getGlobal() {
            skipUnprintableSymbols();
            Node <elemType> *node = nullptr;

            if (isOpCompared(16)) {
                node = newNode("DECLARATION", SYNTAX_OPERATION);
                s += smileOperators[16].size;

                node->rightChild = getVar();

            } else if(isOpCompared(3)) {
                node = newNode("DECLARATION", SYNTAX_OPERATION);
                s += smileOperators[3].size;

                node->rightChild = getFunc();

            } else if(isOpCompared(22)) {
                node = newNode("DECLARATION", SYNTAX_OPERATION);
                s += smileOperators[22].size;

                node->rightChild = getMain();
            } else
                return node;

            getOpEnd();

            node->leftChild = getGlobal();
            return node;
        }

        void getOpEnd() {
            skipUnprintableSymbols();
            if (isOpCompared(9)) {
                s += smileOperators[9].size;
            } else {
                printf("Syntax error: expected \'▶\' at the end of operation\n");
                exit(SYNTAX_ERROR);
            }
        }

        Node <elemType> *getFunc() {
            skipUnprintableSymbols();
            Node <elemType> *node = newNode("FUNCTION", SYNTAX_OPERATION);
            Node <elemType> *rightVal = node->rightChild = getStr();

            skipUnprintableSymbols();
            if (isOpCompared(7)) {
                s += smileOperators[7].size;
                node->leftChild = getVarList();

                skipUnprintableSymbols();
                if (isOpCompared(8)) {
                    s += smileOperators[8].size;
                } else {
                    printf("Syntax error: expected \'⏩\' after function args\n");
                    exit(SYNTAX_ERROR);
                }
            } else {
                printf("Syntax error: expected \'⏪\' after function name\n");
                exit(SYNTAX_ERROR);
            }

            rightVal->rightChild = getBWrap();

            return node;
        }

        Node <elemType> *getMain() {
            skipUnprintableSymbols();
            Node <elemType> *node = newNode("FUNCTION", SYNTAX_OPERATION);
            Node <elemType> *rightVal = node->rightChild = newNode("main", SYNTAX_OPERATION);

            skipUnprintableSymbols();
            if (isOpCompared(7)) {
                s += smileOperators[7].size;
                node->leftChild = getVarList();

                skipUnprintableSymbols();
                if (isOpCompared(8)) {
                    s += smileOperators[8].size;
                } else {
                    printf("Syntax error: expected \'⏩\' after function args\n");
                    exit(SYNTAX_ERROR);
                }
            } else {
                printf("Syntax error: expected \'⏪\' after function name\n");
                exit(SYNTAX_ERROR);
            }

            rightVal->rightChild = getBWrap();

            return node;
        }

        Node <elemType> *getBWrap() {
            Node <elemType> *node = nullptr;
            skipUnprintableSymbols();
            if (isOpCompared(4)){
                s += smileOperators[4].size;
                node = getB();

                skipUnprintableSymbols();
                if (isOpCompared(5)) {
                    s += smileOperators[5].size;
                } else {
                    printf("Syntax error: expected \'🤜\' after 🤛\n");
                    exit(SYNTAX_ERROR);
                }
            } else {
                printf("Syntax error: expected \'🤛\' after function init\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getB() {
            Node <elemType> *node = newNode("BLOCK", SYNTAX_OPERATION);
            node->rightChild = getOp();
            return node;
        }

        Node <elemType> *getOp() {
            Node <elemType> *node = newNode("OP", SYNTAX_OPERATION);

            skipUnprintableSymbols();
            if (isOpCompared(7)){
                char *sStart = s;
                s += smileOperators[7].size;

                while (!isOpCompared(8)) {
                    s++;
                }
                s += smileOperators[8].size;

                skipUnprintableSymbols();
                if (isOpCompared(0)) {
                    s = sStart;
                    node->rightChild = getIf();
                    getOpEnd();

                    node->leftChild = getOp();
                } else {
                    printf("Syntax error: expected \'❔\' after condition\n");
                    exit(SYNTAX_ERROR);
                }

                return node;
            }

            if (isOpCompared(18)) {
                s += smileOperators[18].size;
                node->rightChild = getWhile();

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            if (isOpCompared(16)) {
                s += smileOperators[16].size;
                node->rightChild = getVar();

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            if(isalpha(*s)) {
                char *sStart = s;
                while (*s != ' ') s++;

                skipUnprintableSymbols();
                if (isOpCompared(7)) {
                    s = sStart;
                    node->rightChild = getCall();
                } else {
                    s = sStart;
                    node->rightChild = getAssign();
                }

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            if (isOpCompared(6)) {
                s += smileOperators[6].size;

                node->rightChild = getPrint();

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            if (isOpCompared(15)) {
                s += smileOperators[15].size;

                node->rightChild = getScan();

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            if (isOpCompared(17)) {
                s += smileOperators[17].size;

                node->rightChild = getRet();

                getOpEnd();
                node->leftChild = getOp();
                return node;
            }

            return nullptr;
        }

        Node <elemType> *getCall() {
            Node <elemType> *node = newNode("CALL", SYNTAX_OPERATION);

            node->rightChild = getStr();

            skipUnprintableSymbols();
            if (isOpCompared(7)) {
                s += smileOperators[7].size;

                node->leftChild = getVarList();

                skipUnprintableSymbols();
                if (isOpCompared(8)) {
                    s += smileOperators[8].size;
                } else {
                    printf("Syntax error: expected \'⏩\' after ⏪\n");
                    exit(SYNTAX_ERROR);
                }
            } else {
                printf("Syntax error: expected \'⏪\' after function call\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getRet() {
            Node <elemType> *node = newNode("RETURN", SYNTAX_OPERATION);

            node->rightChild = getExp();
            if (!node->rightChild) {
                printf("Syntax error: expected variable after 👂\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getScan() {
            Node <elemType> *node = newNode("INPUT", SYNTAX_OPERATION);

            node->rightChild = getStr();
            if (!node->rightChild) {
                printf("Syntax error: expected variable after 👂\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getPrint() {
            Node <elemType> *node = newNode("OUTPUT", SYNTAX_OPERATION);

            node->rightChild = getExp();
            if (!node->rightChild) {
                printf("Syntax error: expected variable after 😱\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getAssign() {
            Node <elemType> *node = newNode("ASSIGNMENT", SYNTAX_OPERATION);

            node->leftChild = getStr();

            skipUnprintableSymbols();
            if (isOpCompared(11)) {
                s += smileOperators[11].size;

                node->rightChild = getExp();
            } else {
                printf("Syntax error: expected \'✏\' after variable\n");
                exit(SYNTAX_ERROR);
            }

            return node;
        }

        Node <elemType> *getWhile() {
            Node <elemType> *node = newNode("WHILE", SYNTAX_OPERATION);

            skipUnprintableSymbols();
            if (isOpCompared(7)) {
                s += smileOperators[7].size;
                node->leftChild = getCond();

                if (isOpCompared(8)) {
                    s += smileOperators[8].size;
                } else {
                    printf("Syntax error: expected \'⏩\' after ⏪\n");
                    exit(SYNTAX_ERROR);
                }
            } else {
                printf("Syntax error: expected \'⏪\' after 🔁\n");
                exit(SYNTAX_ERROR);
            }

            node->rightChild = getBWrap();

            return node;
        }

        Node <elemType> *getIf() {
            Node <elemType> *node = newNode("IF", SYNTAX_OPERATION);

            skipUnprintableSymbols();
            if (isOpCompared(7)) {
                s += smileOperators[7].size;
                node->leftChild = getCond();

                skipUnprintableSymbols();
                if (isOpCompared(8)) {
                    s += smileOperators[8].size;
                }
            }

            skipUnprintableSymbols();
            if (isOpCompared(0)) {
                s += smileOperators[0].size;
            }

            node->rightChild = getC();

            return node;
        }

        Node <elemType> *getCond() {
            Node <elemType> *node = getEq();
            return node;
        }

        Node <elemType> *getEq() {
            Node <elemType> *leftVal = getExp();
            Node <elemType> *node = nullptr;

            skipUnprintableSymbols();
            if (isOpCompared(20)) {
                node = newNode("EQUAL", SYNTAX_OPERATION);
                s += smileOperators[20].size;

            } else if (isOpCompared(21)) {
                node = newNode("BELOW", SYNTAX_OPERATION);
                s += smileOperators[21].size;

            } else if (isOpCompared(9)) {
                node = newNode("ABOVE", SYNTAX_OPERATION);
                s += smileOperators[9].size;

            } else {
                printf("Syntax error: expected conditional operation\n");
                exit(SYNTAX_ERROR);
            }

            node->leftChild = leftVal;
            node->rightChild = getExp();

            return node;
        }

        Node <elemType> *getC() {
            Node <elemType> *node = newNode("C", SYNTAX_OPERATION);

            skipUnprintableSymbols();
            if (isOpCompared(2)) {
                s += smileOperators[2].size;

                node->rightChild = getBWrap();
            } else {
                printf("Syntax error: expected \'✅\' after ❔\n");
                exit(SYNTAX_ERROR);
            }

            skipUnprintableSymbols();
            if (isOpCompared(1)) {
                s += smileOperators[1].size;

                node->leftChild = getBWrap();
            }

            return node;
        }

        Node <elemType> *getExp() {
            Node <elemType> *node = getE();
            return node;
        }

        Node <elemType> *getVarList() {
            skipUnprintableSymbols();

            Node <elemType> *rightVal = getExp();

            if (!rightVal) return nullptr;

            Node <elemType> *node = newNode("VARLIST", SYNTAX_OPERATION);
            node->rightChild = rightVal;

            skipUnprintableSymbols();
            if (isOpCompared(10)) {
                s += smileOperators[10].size;
                node->leftChild = getVarList();
            }

            return node;
        }

        Node <elemType> *getVar() {
            skipUnprintableSymbols();
            Node <elemType> *node = newNode("INITIALIZE", SYNTAX_OPERATION);
            node->rightChild = getStr();

            skipUnprintableSymbols();
            if (isOpCompared(11)) {
                s += smileOperators[11].size;
                node->leftChild = getExp();
            }
            return node;
        }

        Node <elemType> *getStr() {
            skipUnprintableSymbols();
            Node <elemType> *node = newNode(s);

            if (isalpha(*s)) node->nodeType = VARIABLE;
            else if (isdigit(*s) || (*s == '-' && isdigit(*(s + 1) ))) node->nodeType = NUMBER;
            else return nullptr;

            while (*s != ' ') s++;
            *s = '\0';
            s++;
            return node;
        }

//        Node <elemType> *getN() {
//            skipUnprintableSymbols();
//
//            if ((*s == '-' && isdigit(*(s + 1))) || isdigit(*s)){
//                elemType val = 0;
//                int size = 0;
//
//                sscanf(s, "%lf%n", &val, &size);
//                s += size;
//
//                Node <elemType> *node = newNode(val);
//                node->nodeType = NUMBER;
//                return node;
//            }
//
//            double val = *s - 'a';
//            s++;
//
//            Node <elemType> *node = newNode(val);
//            node->nodeType = VARIABLE;
//            return node;
//        }

        Node <elemType> *getT() {
            Node <elemType> *valLeft = getD();
            Node <elemType> *node = nullptr;
            Node <elemType> *valRight = nullptr;

            skipUnprintableSymbols();
            while (*s == '*' || *s == '/') {
                skipUnprintableSymbols();

                char op = *s;
                s++;

                valRight = getD();

                skipUnprintableSymbols();
                if (op == '*') {
                    node = newNode("MUL");
                }else node = newNode("DIV");

                tyingNodes(node, valLeft, valRight);
                node->nodeType = MATH_OPERATION;

                valLeft = node;
            }

            return valLeft;
        }

        void tyingNodes(Node <elemType> *node, Node <elemType> *valLeft, Node <elemType> *valRight) {
            node->leftChild = valLeft;
            if (valLeft) node->leftChild->parent = node;

            node->rightChild = valRight;
            node->rightChild->parent = node;
        }

        Node <elemType> *getE() {
            Node <elemType> *valLeft = getT();
            Node <elemType> *node = nullptr;
            Node <elemType> *valRight = nullptr;

            skipUnprintableSymbols();
            while (*s == '+' || *s == '-') {
                skipUnprintableSymbols();

                char op = *s;
                s++;

                valRight = getT();

                skipUnprintableSymbols();
                if (op == '+') {
                    node = newNode("ADD");
                } else node = newNode("SUB");

                tyingNodes(node, valLeft, valRight);
                node->nodeType = MATH_OPERATION;

                valLeft = node;
            }

            return valLeft;
        }

        Node <elemType> *getP() {
            skipUnprintableSymbols();

            if (*s == '(') {
                s++;
                Node <elemType> *valNode = getE();
                if (*s != ')') {
                    printf("Syntax error: expected ')'\n");
                    exit(SYNTAX_ERROR);
                }
                s++;
                return valNode;
            } else if (isalpha(*s)) {
                char *sStart = s;

                while (*s != ' ' || isOpCompared(7)) s++;

                skipUnprintableSymbols();
                if (isOpCompared(7)) {
                    s = sStart;
                    return getCall();
                }

                s = sStart;
                return getStr();

            } else return getStr();
        }

        Node <elemType> *getD() {
            Node <elemType> *valLeft = getF();
            Node <elemType> *node = nullptr;
            Node <elemType> *valRight = nullptr;

            skipUnprintableSymbols();
            while (*s == '^') {
                skipUnprintableSymbols();

                char op = *s;
                s++;

                valRight = getF();
                node = newNode("^");
            }

            if (!node) return valLeft;
            tyingNodes(node, valLeft, valRight);
            node->nodeType = MATH_OPERATION;

            return node;
        }

        Node <elemType> *getF() {
            Node <elemType> *node = nullptr;
            Node <elemType> *valRight = nullptr;

            skipUnprintableSymbols();
            if (isOpCompared(19)) {
                s += smileOperators[19].size;
            } else {
                return getP();
            }

            skipUnprintableSymbols();
            if (*s == '(') {
                s++;
            } else {
                printf("Syntax error: expected '(' after ✔");
                exit(SYNTAX_ERROR);
            }

            valRight = getE();

            skipUnprintableSymbols();
            if (*s != ')') {
                printf("Syntax error: expected ')' after (");
                exit(SYNTAX_ERROR);
            }
            s++;

            node = newNode("SQR", MATH_OPERATION);
            tyingNodes(node, nullptr, valRight);
            return node;
        }

        void nodeToTex(Node <double> *node, FILE *tex) {
            if (!node) return;
            if (node->nodeType == NUMBER) {
                if (node->value < 0) fprintf(tex, "\\left(");
                fprintf(tex, "%g", node->value);
                if (node->value < 0) fprintf(tex, "\\right)");
            }
            else if (node->nodeType == VARIABLE) fprintf(tex, "%c", (char) (node->value + 'a'));
            else if (node->nodeType == MATH_OPERATION) {
#define DEF_CMD(name, num, sign, code, texCode) \
            case num: texCode break;

                switch ((int) node->value) {
#include "operations.h"

                    default: printf("Error: unknown function");
                }
#undef DEF_CMD

            }
        }

    public:
        explicit Tree(elemType val) {
            auto *node = newNode(val);

            root = node;
        }

        Tree(char load, const char *inPath) {
            assert(load == 'L');


            int fileSize = getFileSize(inPath);

            char *buffer = new char[fileSize] ();
            char *bufferStart = buffer;
            readFile(inPath, buffer, fileSize);


            if (*buffer != '{') {
                printf("Error: No tree in file");
                exit(NOT_FOUND_TREE_IN_FILE);
            }

            if (*(buffer + spaceN(buffer + 1)) == '}') {
                printf("Error: tree is empty");
                exit(NOT_FOUND_TREE_IN_FILE);
            }

            buffer += 1 + spaceN(buffer + 1);
            char str[100] = "";
            sscanf(buffer, "%s", str);

            auto *node = newNode(str);
            root = node;
            buffer += strlen(str);
            buffer += spaceN(buffer + 1) + 1;

            writeNode(&buffer, root);
        }

        Tree() = default;

        Node <elemType> *getG(char *str) {
            s = str;

            skipUnprintableSymbols();
            Node <elemType> *valNode = getGlobal();

            skipUnprintableSymbols();
            if (*s != '\0') {
                printf("Syntax error: expected end of row\n");
                exit(SYNTAX_ERROR);
            }

            root = newNode("PROGRAM_ROOT", SYNTAX_OPERATION);
            root->rightChild = valNode;
            return root;
        }

        Node <elemType> *newNode(elemType val, int type = NOTHING) {
            auto *node = new Node <elemType> (val);
            node->nodeType = type;
            return node;
        }

        void insertNodeLeft(Node <elemType> *parent, Node <elemType> *node) {
            parent->leftChild = node;
        }

        void insertNodeRight(Node <elemType> *parent, Node <elemType> *node) {
            parent->rightChild = node;
        }

        void insertLeft(Node <elemType> *parentNode, elemType val) {
            auto *node = newNode(val);

            parentNode->leftChild = node;
            node->parent = parentNode;
        }

        void insertRight(Node <elemType> *parentNode, elemType val) {
            auto *node = newNode(val);

            parentNode->rightChild = node;
            node->parent = parentNode;
        }

        void deleteChildren(Node <elemType> *node) {
            if (node->leftChild) {
                deleteChildren(node->leftChild);
                delete node->leftChild;
            }
            if (node->rightChild) {
                deleteChildren(node->rightChild);
                delete node->rightChild;
            }

            node->leftChild = nullptr;
            node->rightChild = nullptr;
        }

        Node <elemType> *getRoot() {
            return root;
        }

        Node <elemType> *getLeftChild(Node <elemType> *node) {
            assert(node);

            return node->leftChild;
        }

        Node <elemType> *getRightChild(Node <elemType> *node) {
            assert(node);

            return node->rightChild;
        }

        Node <elemType> getParent(Node <elemType> *node) {
            assert(node);

            return node->parent;
        }

        elemType getVal(Node <elemType> *node) {
            return node->value;
        }

        Node <elemType> *findElem(Node <elemType> *subtree, Node <elemType> **valElem, elemType val) {
            assert(subtree);

            if(subtree->value == val) *valElem =  subtree;

            if (subtree->leftChild) {
                findElem(subtree->leftChild, valElem, val);
            }

            if (subtree->rightChild) {
                findElem(subtree->rightChild, valElem, val);
            }
        }

        void changeVal(Node <elemType> *node, elemType val) {
            node->value = val;
        }

        void changeType(Node <elemType> *node, int type) {
            node->nodeType = type;
        }

        void dump() {
            std::ofstream  dumpFile (dumpFilePath);
            if (!dumpFile) {
                printf("File isn't open\n");
                exit(FILE_OPEN_FAILED);
            }

            dumpFile << "graph G{\n";

            if (root) {
                Node <elemType> *currentNode = root;
                printNodeInit(&dumpFile, root);
                printNodeRel(&dumpFile, root);
            }

            dumpFile << "}\n";

            dumpFile.close();

            char dotCommand[FILENAME_MAX] = "";
            sprintf(dotCommand, "dot -Tpng -O %s", dumpFilePath);
            std::system(dotCommand);
        }

        void saveTo(const char *path) {
            std::ofstream outFile (path);

            if (!outFile) {
                printf("File isn't open\n");
                exit(FILE_OPEN_FAILED);
            }

            saveNode(&outFile, root);
            outFile.close();
        }

        void setRoot(Node <elemType> *node) {
            root = node;
        }

        Node <elemType> *copySubtree(Node <elemType> *node) {
            Node <elemType> *subtree = newNode(node->value, node->nodeType);

            if (node->leftChild) subtree->leftChild = copySubtree(node->leftChild);
            if (node->rightChild) subtree->rightChild = copySubtree(node->rightChild);

            return subtree;
        }

        void saveTreeTex(Node <elemType> *node, FILE *tex, const char *str, const char *funcName) {
            fprintf(tex, "%s\n", str);

            fprintf(tex, "\\begin{gather}\\label{eq:%x}", node);

            fprintf(tex, "%s = ", funcName);
            nodeToTex(node, tex);

            fprintf(tex, "\\end{gather}\n");

        }

        void simplify() {
            size_t simplifyCount = 0;

            simplifyCount += simplifyMul0(root, simplifyCount);

            simplifyCount += simplifyDiv0(root, simplifyCount);

            simplifyCount += simplifyAdd0(&root, simplifyCount);

            simplifyCount += simplifyMul1(&root, simplifyCount);

            simplifyCount += simplifySub0(&root, simplifyCount);

            simplifyCount += simplifyDiv1(&root, simplifyCount);

            simplifyCount += convolutionConst(root, simplifyCount);

            if (simplifyCount) simplify();
        }

        size_t simplifyMul0(Node <elemType> *node, size_t simplifyCount){
            if (node->nodeType == MATH_OPERATION && node->value == MUL) {
                bool check = false;
                if (node->leftChild && node->leftChild->nodeType == NUMBER && node->leftChild->value == 0) check = true;
                if (node->rightChild && node->rightChild->nodeType == NUMBER && node->rightChild->value == 0) check = true;

                if (check) {
                    deleteChildren(node);
                    node->value = 0;
                    node->nodeType = NUMBER;
                    simplifyCount++;
                }
            }
            if (node->leftChild) simplifyCount += simplifyMul0(node->leftChild, simplifyCount);
            if (node->rightChild) simplifyCount += simplifyMul0(node->rightChild, simplifyCount);

            return simplifyCount;
        }

        size_t simplifyDiv0(Node <elemType> *node, size_t simplifyCount){
            if (node->nodeType == MATH_OPERATION && node->value == DIV) {
                if (node->leftChild->nodeType == NUMBER && node->leftChild->value == 0) {
                    deleteChildren(node);
                    node->value = 0;
                    node->nodeType = NUMBER;
                    simplifyCount++;
                }
            }
            if (node->leftChild) simplifyCount += simplifyDiv0(node->leftChild, simplifyCount);
            if (node->rightChild) simplifyCount += simplifyDiv0(node->rightChild, simplifyCount);

            return simplifyCount;
        }

        size_t simplifyAdd0(Node <elemType> **nodePointer, size_t simplifyCount) {
            Node <elemType> *node = *nodePointer;

            if (node && node->nodeType == MATH_OPERATION && node->value == ADD) {
                if (node->leftChild && node->leftChild->nodeType == NUMBER && node->leftChild->value == 0) {
                    delete node->leftChild;
                    node->leftChild = nullptr;
                    *nodePointer = node->rightChild;
                    simplifyCount++;
                }
                else if (node->rightChild && node->rightChild->nodeType == NUMBER && node->rightChild->value == 0) {
                    delete node->rightChild;
                    node->rightChild = nullptr;
                    *nodePointer = node->leftChild;
                    simplifyCount++;
                }
            }
            if ((*nodePointer)->leftChild) simplifyCount += simplifyAdd0(&((*nodePointer)->leftChild), simplifyCount);
            if ((*nodePointer)->rightChild) simplifyCount += simplifyAdd0(&((*nodePointer)->rightChild), simplifyCount);

            return simplifyCount;
        }

        size_t simplifySub0(Node <elemType> **nodePointer, size_t simplifyCount) {
            Node <elemType> *node = *nodePointer;

            if (node->nodeType == MATH_OPERATION && node->value == SUB) {
                if (node->leftChild && node->leftChild->nodeType == NUMBER && node->leftChild->value == 0) {
                    node->value = MUL;
                    node->nodeType = MATH_OPERATION;
                    node->leftChild->value = -1;
                    simplifyCount++;
                }
                else if (node->rightChild && node->rightChild->nodeType == NUMBER && node->rightChild->value == 0) {
                    delete node->rightChild;
                    *nodePointer = node->leftChild;
                    simplifyCount++;
                }
            }
            if ((*nodePointer)->leftChild) simplifyCount += simplifySub0(&((*nodePointer)->leftChild), simplifyCount);
            if ((*nodePointer)->rightChild) simplifyCount += simplifySub0(&((*nodePointer)->rightChild), simplifyCount);

            return simplifyCount;
        }

        size_t simplifyMul1(Node <elemType> **nodePointer, size_t simplifyCount) {
            Node <elemType> *node = *nodePointer;

            if (node->nodeType == MATH_OPERATION && node->value == MUL) {
                if (node->leftChild && node->leftChild->nodeType == NUMBER && node->leftChild->value == 1) {
                    delete node->leftChild;
                    *nodePointer = node->rightChild;
                    simplifyCount++;
                }
                else if (node->rightChild && node->rightChild->nodeType == NUMBER && node->rightChild->value == 1) {
                    delete node->rightChild;
                    *nodePointer = node->leftChild;
                    simplifyCount++;
                }
            }
            if ((*nodePointer)->leftChild) simplifyCount += simplifyMul1(&((*nodePointer)->leftChild), simplifyCount);
            if ((*nodePointer)->rightChild) simplifyCount += simplifyMul1(&((*nodePointer)->rightChild), simplifyCount);

            return simplifyCount;
        }

        size_t simplifyDiv1(Node <elemType> **nodePointer, size_t simplifyCount) {
            Node <elemType> *node = *nodePointer;

            if (node->nodeType == MATH_OPERATION && node->value == DIV) {
                if (node->rightChild && node->rightChild->nodeType == NUMBER && node->rightChild->value == 1) {
                    delete node->rightChild;
                    *nodePointer = node->leftChild;
                    simplifyCount++;
                }
            }
            if (node->leftChild) simplifyCount += simplifyDiv1(&(node->leftChild), simplifyCount);
            if (node->rightChild) simplifyCount += simplifyDiv1(&(node->rightChild), simplifyCount);

            return simplifyCount;
        }

        size_t convolutionConst(Node <elemType> *node, size_t simplifyCount) {
            if (node->leftChild && node->rightChild) {
                if (node->nodeType == MATH_OPERATION && node->leftChild->nodeType == NUMBER && node->rightChild->nodeType == NUMBER) {
                    node->nodeType = NUMBER;
                    double leftNode = node->leftChild->value;
                    double rightNode = node->rightChild->value;

                    if (node->value == ADD) node->value = leftNode + rightNode;
                    else if (node->value == SUB) node->value = leftNode - rightNode;
                    else if (node->value == MUL) node->value = leftNode * rightNode;
                    else if (node->value == DIV) node->value = leftNode / rightNode;
                    else if (node->value == DEG) node->value = pow(leftNode, rightNode);

                    deleteChildren(node);
                    simplifyCount++;
                }
            }

            if (node->rightChild) {
                if (node->nodeType == MATH_OPERATION && node->leftChild == nullptr && node->rightChild->nodeType == NUMBER) {
                    node->nodeType = NUMBER;
                    double rightNode = node->rightChild->value;

                    if (node->value == SIN) node->value = sin(rightNode);
                    else if (node->value == COS) node->value = cos(rightNode);
                    else if (node->value == LN) node->value = log(rightNode);

                    deleteChildren(node);
                    simplifyCount++;
                }
            }

            if (node->leftChild) simplifyCount += convolutionConst(node->leftChild, simplifyCount);
            if (node->rightChild) simplifyCount += convolutionConst(node->rightChild, simplifyCount);

            return simplifyCount;
        }

        bool findVarInSubtree(Node <elemType> *node) {
            bool checker = false;
            if (node->nodeType == VARIABLE) return true;

            if (node->leftChild) checker = findVarInSubtree(node->leftChild);
            if (checker) return true;
            if (node->rightChild) checker = findVarInSubtree(node->rightChild);

            return checker;
        }

        void skipUnprintableSymbols() {
            while (*s == ' ' || *s == '\n' || *s == '\t') {
                s++;
            }
        }

        bool isOpCompared(int opNum) {
            smileOp operation = smileOperators[opNum];
            return !operation.symbol.compare(0, operation.size, s, operation.size);
        }
    };

    int spaceN (const char *buffer) {
        int count = 0;

        while (*buffer + count == ' ') {
            count++;
        }

        return count;
    }
}