#pragma once
#include "global.h"
void strToInts(SyntaxTree* r);
void print(SyntaxTree* root);

int fold(SyntaxTree* n,bool &varFlag);
void foldConstants(SyntaxTree* r);

void declarationChecker(SyntaxTree* r);
void findStrayDecls(SyntaxTree* r);

void checkAssignments(SyntaxTree* r);

