#include "semantics.h"
#include <queue>
#include <iostream>
#include <unordered_map>
extern std::unordered_map<std::string, varAttribs>symbol_table;
extern bool GoodToGo;
void strToInts(SyntaxTree* r)
{
	if (!r) return;
	if (r->node_t == EXPR && r->token.tok == INT_LIT)
	{
		//std::cout << "Integer found here!\n";
		r->ivalue = std::stoi(r->token.value);
		//std::cout << r->ivalue << std::endl;
	}
	for (auto child : r->children)
	{
		strToInts(child);
	}
}


void print(SyntaxTree* root)
{
	if (root != NULL) {
		std::cout << root->token.value << std::endl;
		for (auto x : root->children) {
			print(x);
		}
	}
}

int fold(SyntaxTree* n,bool &varFlag)
{
	if (n == NULL)return INFINITY;
	if (n->children.empty() && n->token.tok == INT_LIT) {
		return n->ivalue;
	}
	else if(n->token.tok == ID){
		varFlag = true;
		if (symbol_table.find(n->token.value) == symbol_table.end()) {
			std::cout << "Line " << n->line << ": Error! Undeclared identifier " << n->token.value << std::endl;
		}
		else {
			if (symbol_table[n->token.value].dtype == STRING) {
				std::cout << "Line " << n->line << ": Error! String variable " << n->token.value <<" used in arithmetic expression" << std::endl;
			}
		}
		return INFINITY;
	}
	if (n->expr_t == ARITHMETIC) {
		std::string op = n->token.value;
		int v1 = fold(n->children[0],varFlag);
		int v2 = fold(n->children[1],varFlag);
		if (op == "+")return v1 + v2;
		else if (op == "-")return v1 - v2;
		else if (op == "*")return v1 * v2;
		else if (op == "/")return v1 / v2;
		else if (op == "%")return v1 % v2;
	}
	return INT_MAX;
}


void foldConstants(SyntaxTree* r)
{
	if (!r) return;
	std::queue<SyntaxTree*>bfs;
	SyntaxTree* temp = NULL;
	bfs.push(r);
	while (!bfs.empty())
	{
		bool isVarPresent = false;
		temp = bfs.front();
		if (temp) {
			if (temp->expr_t == ARITHMETIC && temp->token.tok != ID) {
				//std::cout << "Arithmetic Expr here!\n";
				int t = fold(temp, isVarPresent);
				if (isVarPresent) {
					bfs.pop();
					continue;
				}
				//else {
					//std::cout << "Folded value = " << t << std::endl;
					//temp->ivalue = t;
				//}
			}
			else {
				for (auto x : temp->children) {
					bfs.push(x);
				}
			}
		}
		bfs.pop();
	}
}

void declarationChecker(SyntaxTree* r)
{
	varAttribs attribs;
	if (r == NULL)return;
	SyntaxTree* tmp = r->children[0];
	SyntaxTree* mid = r;
	while ((tmp->stmt_t == IDECL || tmp->stmt_t == SDECL))
	{
		if (tmp->stmt_t == IDECL) {
			//std::cout << "int decl ID name " << tmp->token.value << std::endl;
			attribs.dtype = INTEGER;
			if (symbol_table.find(tmp->token.value) == symbol_table.end()) {
				symbol_table.insert(std::make_pair(tmp->token.value, attribs));
			}
			else {
				std::cout << "Line " << tmp->line << ": Error! variable '" << tmp->token.value << "' redeclared!\n";
			}
		}
		else if (tmp->stmt_t == SDECL)
		{
			//std::cout << "str decl ID name " << tmp->token.value << std::endl;
			attribs.dtype = STRING;
			if (symbol_table.find(tmp->token.value) == symbol_table.end()) {
				symbol_table.insert(std::make_pair(tmp->token.value, attribs));
			}
			else {
				std::cout << "Line " << tmp->line << ": Error! variable '" << tmp->token.value << "' redeclared!\n";
			}
		}
		if (mid->children[1] != NULL) {
			mid = mid->children[1];
			tmp = mid->children[0];
		}
		else {
			mid = NULL;
			break;
		}
	}
	findStrayDecls(mid);
}

void findStrayDecls(SyntaxTree* r)
{
	if (r == NULL) return;
	if (r->stmt_t == IDECL || r->stmt_t == SDECL)
	{
		std::cout << "Line " << r->line << " Error! Declare variable '"<< r->token.value<<"' at the start\n";
	}
	for (auto x : r->children) {
		findStrayDecls(x);
	}
}

void checkAssignments(SyntaxTree* r)
{
	if (r == NULL)return;
	if (r->stmt_t == ASSIGNV)
	{
		if (symbol_table.find(r->token.value) == symbol_table.end())
		{
			std::cout << "Line " << r->line << ": Error! Undeclared variable '" << r->token.value << "'\n";
			GoodToGo = false;
			return;
		}
		else {
			SyntaxTree* check = r->children[0];
			data_type ty = symbol_table[r->token.value].dtype;
			if (check->stmt_t == ARITHMETIC && ty == STRING) {
				std::cout << "Line " << check->line << ": Error!Cannot assign arithmetic expression to a string variable '" << r->token.value << "'\n";
				GoodToGo = false;
			}
			else if (ty == STRING && check->token.tok != STR_LIT) {
				std::cout << "Line " << check->line << "Error! Only string literals can be assigned to a string variable!\n";
				GoodToGo = false;
			}
			if (ty == INTEGER) 
			{
				if ((check->expr_t != ARITHMETIC)) {
					std::cout << "Line " << check->line << ": Error!Non integer value being assigned to int variable '" << r->token.value << "'\n";
					GoodToGo = false;
				}
				if ((check->token.tok == ID) && (symbol_table[check->token.value].dtype == STRING)) {
					std::cout << "Line " << check->line << ": Error!Non integer value being assigned to int variable '" << r->token.value << "'\n";
					GoodToGo = false;
				}
			}
		}
	}
	for (auto x : r->children) {
		checkAssignments(x);
	}
}