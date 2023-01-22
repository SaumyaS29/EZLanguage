/*A recursive descent parser for the language
  For simplicity, there is minimal error handling and recovery 
  in the parser.(:P sorry)
  Some mistakes in the source code it parses 
  may cause the parser to go into an infinite recursion*/


#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "global.h"


bool match(tokens expected_t);
SyntaxTree* makeNodeStmt(stmt_type stype);
SyntaxTree* makeNodeTerminator();
SyntaxTree* makeNodeExpr(expr_type etype);

SyntaxTree* Program();
SyntaxTree* stmt_seq();
SyntaxTree* stmt();
SyntaxTree* idecl();
SyntaxTree* sdecl();
SyntaxTree* iassign();
SyntaxTree* sassign();
SyntaxTree* expr();
SyntaxTree* term();
SyntaxTree* factor();
SyntaxTree* ifst();
SyntaxTree* cond_exp();
SyntaxTree* whileloop();
SyntaxTree* dowhileloop();
SyntaxTree* printst();
SyntaxTree* inputst();

#endif