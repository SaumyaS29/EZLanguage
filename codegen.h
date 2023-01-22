#pragma once
#include "global.h"
void filesetup();
std::string gen_label();
void allocate_programmer_strings(SyntaxTree *r);
void allocate_free_strings(SyntaxTree* r);
void allocate_empty_strings();
void allocate_ints();
void gen_expression_final(SyntaxTree* r);
void gen_expression_branch(SyntaxTree* r);


void generate_program(SyntaxTree* r);
int count_read(SyntaxTree* r);
void gen_code(SyntaxTree* r);
void gen_int_assignment_code(SyntaxTree* r);
void gen_bool_exp_code(SyntaxTree* r);