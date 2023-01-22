#pragma once
#include <string>
#include <vector>

enum tokens {   // Tokens used by the EZlang programming language
    BEGIN,
    END,
    IF,
    INPUT,
    PRINT,//kw
    ELSE,   //kw
    WHILE,   //kw
    DO,   //kw
    ASSIGN,  //:
    ID,
    GRTH,
    LTH,
    EQ,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LIN_END,
    OP_BRACE,
    CL_BRACE,
    OP_BRACKET,
    CL_BRACKET,
    END_FILE,
    ERR,
    COMMENT,
    INT_LIT,
    INT,
    STR,
    STR_LIT,
    MOD
};

enum States {
    start,
    In_Id,
    In_dig,
    In_com,
    In_str,
    splchar,
    endoffile,
    error,
    getID,
    getIntLit,
    getStrLit,
    finish
};

class Lexeme {
public:
    tokens tok;
    std::string value;
    /*void operator = (Lexeme& l)
    {
        tok = l.tok;
        value = l.value;
    }*/
};

typedef enum  node_kind { TERMINATE, EXPR, STATEMENT }node_type;
typedef enum statement_kind { IDENTIFIER, IDECL, SDECL, ASSIGNV, STRCONST, PRINTK, READK, IFK, WHILEK, DOWHILEK }stmt_type;
typedef enum expression_kind { ARITHMETIC, BOOLEAN }expr_type;

struct SyntaxTree
{
    node_type node_t;
    stmt_type stmt_t;
    expr_type expr_t;
    Lexeme token;
    int ivalue;
    int line;
    std::vector<SyntaxTree*>children;
};

typedef enum { INTEGER, STRING }data_type;
struct varAttribs
{
    data_type dtype;
    std::string strval;
    //int offset;
};
