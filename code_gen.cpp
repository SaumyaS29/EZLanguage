#include "codegen.h"
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
extern std::ofstream target_file;
extern std::unordered_map<std::string, varAttribs>symbol_table;
extern SyntaxTree* syntax_tree;
extern bool GoodToGo;
int printst_num = -1;
int labelno = 0;
extern std::string asmfilepath;

std::string gen_label() {
	std::string s = ".L" + std::to_string(labelno++);
	return s;
}
int count_read(SyntaxTree *r) {
	if (!r)return 0;
	if (r->stmt_t == READK)return 1;
	int n=0;
	for (auto x : r->children) {
		n += count_read(x);
	}
	return n;
}

void generate_program(SyntaxTree* r)
{
	if (!GoodToGo) {
		std::cout << "Compilation aborted due to errors!\n";
		return;
	}
	std::cout << "Generating code...\n";
	target_file << "section .data\n";
	target_file << "	.fmtstr: db \"%s\",0\n";
	target_file << "	.fmtint: db \"%i\",0\n";
	allocate_programmer_strings(syntax_tree);
	allocate_free_strings(syntax_tree);
	allocate_empty_strings();
	allocate_ints();
	if (printst_num >= 0)target_file << "extern printf\n";
	if (count_read(syntax_tree) > 0) target_file << "extern scanf\n";
	target_file << "section .text\nglobal main\nmain:\n";
	printst_num = -1;
	gen_code(syntax_tree);
	target_file << "ret\n";
}
void gen_code(SyntaxTree* r) {
	if (!r)return;
	SyntaxTree* cur = r;
	while (cur != NULL) {
		if (cur->children[0]->stmt_t == ASSIGNV && symbol_table[r->children[0]->token.value].dtype == INTEGER) {
			gen_int_assignment_code(cur->children[0]);
		}
		else if (cur->children[0]->stmt_t == IFK) {
			std::string sk = gen_label(),sk2;
			if (cur->children[0]->children.size() > 2)
				sk2 = gen_label();
			gen_bool_exp_code(cur->children[0]->children[0]);
			switch (cur->children[0]->children[0]->token.tok) {
			case GRTH:
				target_file << "jle " << sk << std::endl;
				break;
			case LTH:
				target_file << "jge " << sk << std::endl;
				break;
			case EQ:
				target_file << "jne " << sk << std::endl;
				break;
			}
			gen_code(cur->children[0]->children[1]);
			//target_file << sk << ":\n";
			//Else part here
			if (cur->children[0]->children.size() > 2) {
				target_file << "jmp " << sk2 << std::endl;
				target_file << sk << ":\n";
				gen_code(cur->children[0]->children[2]);
				target_file << sk2 << ":\n";
			}
			else {
				target_file << sk << ":\n";
			}
		}
		else if (cur->children[0]->stmt_t == READK) {
			std::string vname = cur->children[0]->children[0]->token.value;
			target_file << "lea ebx,[" << vname << "]\n";
			target_file << "push eax\npush ecx\npush edx\npush ebx\n";
			if (symbol_table[vname].dtype == INTEGER)
				target_file << "push .fmtint\n"; 
			else
				target_file << "push .fmtstr\n";
			target_file << "call scanf\nadd esp,8\npop edx\npop ecx\npop eax\n";
		}
		else if (cur->children[0]->stmt_t == WHILEK)
		{
			std::string l_entry = gen_label(), l_exit = gen_label();
			target_file << l_entry<<":\n";
			gen_bool_exp_code(cur->children[0]->children[0]);
			switch (cur->children[0]->children[0]->token.tok) {
			case GRTH:
				target_file << "jle " << l_exit << std::endl;
				break;
			case LTH:
				target_file << "jge " << l_exit << std::endl;
				break;
			case EQ:
				target_file << "jne " << l_exit << std::endl;
				break;
			}
			gen_code(cur->children[0]->children[1]);
			target_file << "jmp " << l_entry << "\n"<<l_exit<<":\n";
		}
		else if (cur->children[0]->stmt_t == DOWHILEK) {
			std::string l_entry = gen_label();
			target_file << l_entry << ":\n";
			gen_code(cur->children[0]->children[0]);
			gen_bool_exp_code(cur->children[0]->children[1]);
			switch (cur->children[0]->children[1]->token.tok) {
			case GRTH:
				target_file << "jg " << l_entry << std::endl;
				break;
			case LTH:
				target_file << "jl " << l_entry << std::endl;
				break;
			case EQ:
				target_file << "je " << l_entry << std::endl;
				break;
			}

		}
		else if (cur->children[0]->stmt_t == PRINTK) {
			printst_num++;
			target_file << "push eax\npush ecx\npush edx\n";
			std::string fmti = "push .fmtint\n",fmts = "push .fmtstr\n";
			if (cur->children[0]->children[0]->token.tok == ID) {
				
				if (symbol_table[cur->children[0]->children[0]->token.value].dtype == INTEGER) {
					target_file << "mov ebx,DWORD[" << cur->children[0]->children[0]->token.value << "]\n";
					target_file << "push ebx\n"<<fmti;
				}
				else {
					target_file << "push " << cur->children[0]->children[0]->token.value << "\n";
					target_file << fmts;
				}
			}
			else if (cur->children[0]->children[0]->stmt_t == STRCONST) {
				target_file << "push " << (".str" + std::to_string(printst_num)+"\n");
				target_file << fmts;
			}
			else /*if (cur->children[0]->children[0]->stmt_t == EXPR)*/ {
				gen_expression_branch(cur->children[0]->children[0]);
				target_file << fmti;

			}
			target_file << "xor eax,eax\ncall printf\nadd esp,8\n";
			target_file << "pop edx\npop ecx\npop eax\n";
		}
		cur = cur->children[1];
	}
}


void allocate_empty_strings()
{
	for (auto x : symbol_table) {
		if (x.second.strval == "" && x.second.dtype == STRING) {
			target_file << "\t" << x.first << " : db \"\",0\n";
		}
	}
}
void filesetup()
{
	target_file.open(asmfilepath.c_str(),std::ios_base::app);
	generate_program(syntax_tree);
	target_file.close();
}

void allocate_programmer_strings(SyntaxTree* r)
{
	if (!r)return;
	if (r->stmt_t == ASSIGNV) {
		if ((symbol_table[r->token.value]).dtype == STRING) {
			target_file <<"\t"<< r->token.value << ": db ";
			target_file << "\"" << r->children[0]->token.value << "\"" << ",0" << std::endl;
			symbol_table[r->token.value].strval = r->children[0]->token.value;
		}
	}
	for (auto x : r->children) {
		allocate_programmer_strings(x);
	}
}

void allocate_free_strings(SyntaxTree* r)
{
	if (!r) return;
	if (r->stmt_t == PRINTK) {
		printst_num++;
		if (r->children[0]->token.tok == STR_LIT) {
			std::string label = "\t.str" + std::to_string(printst_num);
			target_file << label << ": db " << "\"" << r->children[0]->token.value << "\",0\n";
		}
	}
	for (auto x : r->children) {
		allocate_free_strings(x);
	}
}

void allocate_ints()
{
	target_file << "section .bss\n";
	for (auto x : symbol_table) {
		if (x.second.dtype == INTEGER) {
			target_file << "\t" << x.first << " resb 4\n";
		}
	}
}

void gen_expression_branch(SyntaxTree* r)
{
	if (!r)return;
	//if (r->expr_t != ARITHMETIC && r->node_t != EXPR)return;
	if (r->token.tok == ID) {
		target_file << "mov eax, DWORD[" << r->token.value << "]\n"<<"push eax\n";
		return;
	}
	else if (r->token.tok == INT_LIT) {
		target_file << "mov eax, " << r->token.value <<"\npush eax\n";
		return;
	}
	// if r is not a leaf node i.e it is an operator node
	else if (r->token.tok != INT_LIT && r->token.tok != ID) {
		//if node has no grandchildren
		if ((r->children[0]->children.size()) == 0 && (r->children[1]->children.size()) == 0) 
		{
			if (r->children[0]->token.tok == ID) {
				target_file << "mov eax,DWORD[" << r->children[0]->token.value << "]\n";
			}
			if (r->children[0]->token.tok == INT_LIT) {
				target_file << "mov eax," << r->children[0]->token.value <<"\n";
			}
			if (r->children[1]->token.tok == ID) {
				target_file << "mov ecx,DWORD[" << r->children[1]->token.value << "]\n";
			}
			if (r->children[1]->token.tok == INT_LIT) {
				target_file << "mov ecx," << r->children[1]->token.value << "\n";
			}
			switch (r->token.tok) {
			case PLUS:
				target_file << "add eax,ecx\npush eax\n";
				break;
			case MINUS:
				target_file << "sub eax,ecx\npush eax\n";
				break;
			case MUL:
				target_file << "mul ecx\npush eax\n";
				break;
			case DIV:
				target_file << "xor edx,edx\ndiv ecx\npush eax\n";
				break;
			case MOD:
				target_file << "xor edx,edx\ndiv ecx\nmov eax,edx\npush eax\n";
				break;
			}
			return;
		}
	}
	for (auto x : r->children) {
		gen_expression_branch(x);
	}
	target_file << "pop ecx\npop eax\n";
	switch (r->token.tok) {
	case PLUS:
		target_file << "add eax,ecx\npush eax\n";
		break;
	case MINUS:
		target_file << "sub eax,ecx\npush eax\n";
		break;
	case MUL:
		target_file << "mul ecx\npush eax\n";
		break;
	case DIV:
		target_file << "xor edx,edx\ndiv ecx\npush eax\n";
		break;
	case MOD:
		target_file << "xor edx,edx\ndiv ecx\nmov eax,edx\npush eax\n";
		break;
	}
}
void gen_expression_final(SyntaxTree* r)
{
	if (!r)return;
	if (r->token.tok == INT_LIT) {
		target_file << "mov eax, " << r->token.value << "\n";
		return;
	}
	if (r->token.tok == ID) {
		target_file << "mov eax, DWORD[" << r->token.value << "]\n";
		return;
	}
	if (r->token.tok != INT_LIT && r->token.tok != ID) {
		//if node has no grandchildren
		if ((r->children[0]->children.size()) == 0 && (r->children[1]->children.size()) == 0)
		{
			if (r->children[0]->token.tok == ID) {
				target_file << "mov eax,DWORD[" << r->children[0]->token.value << "]\n";
			}
			if (r->children[0]->token.tok == INT_LIT) {
				target_file << "mov eax," << r->children[0]->token.value << "\n";
			}
			if (r->children[1]->token.tok == ID) {
				target_file << "mov ecx,DWORD[" << r->children[1]->token.value << "]\n";
			}
			if (r->children[1]->token.tok == INT_LIT) {
				target_file << "mov ecx," << r->children[1]->token.value << "\n";
			}
			switch (r->token.tok) {
			case PLUS:
				target_file << "add eax,ecx\n";
				break;
			case MINUS:
				target_file << "sub eax,ecx\n";
				break;
			case MUL:
				target_file << "mul ecx\n";
				break;
			case DIV:
				target_file << "xor edx,edx\ndiv ecx\n";
				break;
			case MOD:
				target_file << "xor edx,edx\ndiv ecx\nmov eax,edx\n";
				break;
			}
			return;
		}
	}
	gen_expression_branch(r->children[0]);
	gen_expression_branch(r->children[1]);
	target_file << "pop ecx\npop eax\n";
	switch (r->token.tok) {
	case PLUS:
		target_file << "add eax,ecx\n";
		break;
	case MINUS:
		target_file << "sub eax,ecx\n";
		break;
	case MUL:
		target_file << "mul ecx\n";
		break;
	case DIV:
		target_file << "xor edx,edx\ndiv ecx\n";
		break;
	case MOD:
		target_file << "xor edx,edx\ndiv ecx\nmov eax,edx\n";
		break;
	}
}

void gen_int_assignment_code(SyntaxTree* r)
{
	if (!r)return;
	gen_expression_final(r->children[0]);
	target_file << "mov DWORD[" << r->token.value << "], eax\n";
}

void gen_bool_exp_code(SyntaxTree* r) {
	if (!r)return;
	if (r->expr_t == BOOLEAN) {
		gen_expression_branch(r->children[0]);
		gen_expression_branch(r->children[1]);
		target_file << "pop ecx\npop eax\ncmp eax,ecx\n";
	}
}