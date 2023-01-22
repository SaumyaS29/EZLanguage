#include <iostream>
#include "scanner.h"
#include <fstream>
#include "parser.h"
#include "semantics.h"
#include "codegen.h"

extern Lexeme current_token;
extern std::ifstream source;
extern SyntaxTree* syntax_tree;
extern bool GoodToGo;
extern std::string asmfilepath;
void clean_tree(SyntaxTree *root)
{
	if (root == NULL) return;
	if (root->children.empty()) {
		delete root;
	}
	else {
		for (auto x : root->children) {
			clean_tree(x);
		}
		delete root;
	}
}
int main()
{
	GoodToGo = true;
	std::string srcfilepath;
	std::cout << "Enter full path of source code file: ";
	std::cin >> srcfilepath;
	std::cout << "Enter full path of target file: ";
	std::cin >> asmfilepath;
	
	source.open(srcfilepath.c_str(), std::ios_base::in);
	Extract_src();
	current_token = get_Token();
	//std::cout << current_token.value << std::endl;

	//syntax_tree will be an extern for semantic analysis
	syntax_tree = Program();
	//print(syntax_tree);
	std::cout << "Parsing done!\n";
	//strToInts(syntax_tree);
	declarationChecker(syntax_tree);
	//foldConstants(syntax_tree);
	checkAssignments(syntax_tree);
	filesetup();
	clean_tree(syntax_tree);
	return 0;
}