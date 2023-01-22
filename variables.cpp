#include <string>
#include <fstream>
#include <unordered_map>
#include "global.h"

Lexeme current_token;
char splch[] = { ':','>','<','=','+','-','*','/','(',')','{','}',';','%' }; //special characters
std::ifstream source; // Source file object
std::ofstream target_file;
std::string src_code;  //source code
unsigned int current;  //character index in the source code
int line_no = 1;
SyntaxTree* syntax_tree = NULL;

std::unordered_map<std::string, tokens> lookup = {
    {"if",IF},
    {"else",ELSE},
    {"while",WHILE},
    {"do",DO},
    {"begin",BEGIN},
    {"end",END},
    {"int",INT},
    {"input",INPUT},
    {"print",PRINT},
    {"str",STR},
    {":",ASSIGN},
    {">",GRTH},
    {"<",LTH},
    {"=",EQ},
    {";",LIN_END},
    {"+",PLUS},
    {"-",MINUS},
    {"*",MUL},
    {"/",DIV},
    {"{",OP_BRACE},
    {"}",CL_BRACE},
    {"(",OP_BRACKET},
    {")",CL_BRACKET},
    {"%",MOD}
};

std::unordered_map<std::string, varAttribs>symbol_table;
bool GoodToGo;
std::string asmfilepath;