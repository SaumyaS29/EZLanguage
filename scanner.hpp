//
//  scanner.hpp
//  Compiler
//
//  Created by Saumya Srivastava on 23/05/21.
//  Copyright © 2021 Saumya Srivastava. All rights reserved.
//

#ifndef scanner_hpp
#define scanner_hpp

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

//...............................................................................
char splch[]={':','>','<','=','+','-','*','/','(',')','{','}',';',',','%'}; //special characters
std::ifstream source; // Source file object
std::string src_code;  //source code
unsigned int current;  //character index in the source code

//................................................................................
Lexeme get_Token();//This function runs the automata and returns a lexeme
char fgetch();   //Helper function to extract character in the file
int issplch(char c);  //check if a character is a special character or not
void Extract_src();   //read contents of source file into string variable src_code

//..................>Definitions....................................................................................

char fgetch()
{
    char a = src_code[current];
    current++;
    if(a != '\0'){
        return a;
    }
    return 0;
}

int issplch(char c)
{
    for(int i=0;i<15;i++){
        if(splch[i] == c) return 1;
    }
    return 0;
}

void Extract_src()
{
    std::stringstream temp;
    temp << source.rdbuf();
    src_code = temp.str();
}


Lexeme get_Token()
{
    struct Lexeme token;
    std::string token_attrib;
    States state=start;
    char ch;
    while(state != finish)
    {
        switch(state)
        {
            case start:
                ch = fgetch();
                while(isspace(ch)) {ch=fgetch();}
                if(isalpha(ch)) {
                    token_attrib.push_back(ch);
                    state = In_Id;
                }
                else if(isdigit(ch)){
                    token_attrib.push_back(ch);
                    state = In_dig;
                }
                else if(ch == '@') { state = In_com;}
                else if(ch == '"'){ state = In_str;}
                else if(issplch(ch)){
                    token_attrib.push_back(ch);
                    state = splchar;
                }
                else if(ch == 0) { state = endoffile;}
                else  { state = error; }
                break;
            
            case In_Id:
                ch=fgetch();
                while(isalnum(ch)){
                    token_attrib.push_back(ch);
                    ch=fgetch();
                }
                state = getID;
                break;
            case In_dig:
                ch=fgetch();
                while(isdigit(ch)){
                    token_attrib.push_back(ch);
                    ch=fgetch();
                }
                state = getIntLit;
                break;
            case In_com:
                ch = fgetch();
                while(ch != '\n' && ch != 0) ch=fgetch();
                state = start;
                break;
            case In_str:
                ch=fgetch();
                while(ch != '"' && ch !=0 && ch!= '\n'){
                    token_attrib.push_back(ch);
                    ch=fgetch();
                }
                if(ch == '"'){
                    state = getStrLit;
                    break;
                }
                else{
                    std::cout<<"Error! Unterminated string!\n";
                    state = finish;
                }
                break;
            case splchar:
                token.tok = lookup[token_attrib];
                token.value = token_attrib;
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case endoffile:
                token.tok = END_FILE;
                token.value = "EOF";
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case error:
                token.tok = ERR;
                token.value = "Err";
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case getID:
                current--;
                if(lookup.find(token_attrib) != lookup.end()){
                    token.tok = lookup[token_attrib];
                }
                else token.tok = ID;
                token.value = token_attrib;
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case getIntLit:
                current--;
                token.tok = INT_LIT;
                token.value = token_attrib;
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case getStrLit:
                token.tok = STR_LIT;
                token.value = token_attrib;
                //std::cout<<token.tok<<" "<<token.value<<"\n";
                state = finish;
                break;
            case finish:
                break;
        }
    }
    return token;
}
//End here



#endif /* scanner_hpp */
