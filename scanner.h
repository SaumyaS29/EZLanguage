#pragma once
//
//  scanner.hpp
//  Compiler
//
//  Created by Saumya Srivastava on 23/05/21.
//  Copyright © 2021 Saumya Srivastava. All rights reserved.
//

#ifndef scanner_hpp
#define scanner_hpp

#include "global.h"
//...............................................................................

Lexeme get_Token();//This function runs the automata and returns a lexeme
char fgetch();   //Helper function to extract character in the file
int issplch(char c);  //check if a character is a special character or not
void Extract_src();   //read contents of source file into string variable src_code
char fgetch();

#endif /* scanner_hpp */
