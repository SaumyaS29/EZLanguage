#include "parser.h"
#include "scanner.h"
#include <iostream>

extern Lexeme current_token;
extern int line_no;
bool match(tokens expected_t)
{
	if (current_token.tok == expected_t) {
		current_token = get_Token();
		return true;
	}
	else {
		std::cout <<"Line "<<line_no << ": Unexpected " << current_token.value << std::endl;
		printf("Line %i: Unexpected token %s",line_no,current_token.value.c_str());
		return false;
	}
}

SyntaxTree* makeNodeStmt(stmt_type stype)
{
	SyntaxTree* n = new SyntaxTree;
	n->node_t = STATEMENT;
	n->stmt_t = stype;
	n->token = current_token;
	n->line = line_no;
	return n;
}
SyntaxTree* makeNodeTerminator()
{
	SyntaxTree* n = new SyntaxTree;
	n->node_t = TERMINATE;
	n->token = current_token;
	n->line = line_no;
	return n;
}
SyntaxTree* makeNodeExpr(expr_type etype)
{
	SyntaxTree* n = new SyntaxTree;
	n->node_t = EXPR;
	n->expr_t = etype;
	n->token = current_token;
	n->line = line_no;
	return n;
}


SyntaxTree* Program()
{
	SyntaxTree* node = NULL;
	match(BEGIN);
	node = stmt_seq();
	match(END);
	return node;
}

SyntaxTree* stmt_seq()
{
	if (current_token.tok == CL_BRACE || current_token.tok == END)
		return NULL;
	SyntaxTree* node = NULL, * left = NULL, * right = NULL;
	left = stmt();
	if (current_token.tok == LIN_END) {
		node = makeNodeTerminator();
		node->children.push_back(left);
		match(LIN_END);
		right = stmt_seq();
		node->children.push_back(right);//put cond here if needed
	}
	else {
		return NULL;
	}
	return node;
}

SyntaxTree* stmt()
{
	SyntaxTree* node = NULL, * child = NULL;
	if (current_token.tok == INT)
	{
		node = idecl();
	}
	else if (current_token.tok == STR)
	{
		node = sdecl();
	}
	else if (current_token.tok == ID)
	{
		node = makeNodeStmt(ASSIGNV);
		match(ID);
		match(ASSIGN);
		if (current_token.tok == STR_LIT)
		{
			child = sassign();
		}
		else
		{
			child = iassign();
		}
		node->children.push_back(child);
	}
	else if (current_token.tok == IF)
	{
		node = ifst();
	}
	else if (current_token.tok == WHILE)
	{
		node = whileloop();
	}
	else if (current_token.tok == DO)
	{
		node = dowhileloop();
	}
	else if (current_token.tok == PRINT)
	{
		node = printst();
	}
	else if (current_token.tok == INPUT)
	{
		node = inputst();
	}
	else {
		std::cout <<"Line " << line_no << ": Error! Unrecognized token: " << current_token.value << std::endl;
	}
	return node;
}

SyntaxTree* idecl()
{
	match(INT);
	SyntaxTree* node = makeNodeStmt(IDECL);
	match(ID);
	return node;
}

SyntaxTree* sdecl()
{
	match(STR);
	SyntaxTree* node = makeNodeStmt(SDECL);
	match(ID);
	return node;
}

SyntaxTree* iassign()
{
	SyntaxTree* node = expr();
	return node;
}
SyntaxTree* sassign()
{
	SyntaxTree* node = makeNodeStmt(STRCONST);
	match(STR_LIT);
	return node;
}

SyntaxTree* expr()
{
	SyntaxTree* temp = NULL, * newtemp = NULL, * right = NULL;
	temp = term();
	while (current_token.tok == PLUS || current_token.tok == MINUS)
	{
		newtemp = makeNodeExpr(ARITHMETIC);
		match(current_token.tok);
		newtemp->children.push_back(temp);
		right = term();
		newtemp->children.push_back(right);
		temp = newtemp;
	}
	return temp;
}

SyntaxTree* term()
{
	SyntaxTree* temp = NULL, * newtemp = NULL, * right = NULL;
	temp = factor();
	while (current_token.tok == MUL || current_token.tok == DIV || current_token.tok == MOD)
	{
		newtemp = makeNodeExpr(ARITHMETIC);
		match(current_token.tok);
		newtemp->children.push_back(temp);
		right = factor();
		newtemp->children.push_back(right);
		temp = newtemp;
	}
	return temp;
}

SyntaxTree* factor()
{
	SyntaxTree* node = NULL;
	if (current_token.tok == OP_BRACKET) {
		match(OP_BRACKET);
		node = expr();
		match(CL_BRACKET);
	}
	else if (current_token.tok == INT_LIT || current_token.tok == ID)
	{
		node = makeNodeExpr(ARITHMETIC);
		match(current_token.tok);
	}
	return node;
}
SyntaxTree* ifst()
{
	SyntaxTree* node = makeNodeStmt(IFK);
	SyntaxTree* testc = NULL, * then = NULL, * elsep = NULL;
	match(IF);
	match(OP_BRACKET);
	testc = cond_exp();
	match(CL_BRACKET);
	node->children.push_back(testc);
	match(OP_BRACE);
	then = stmt_seq();
	match(CL_BRACE);
	node->children.push_back(then);
	if (current_token.tok == ELSE)
	{
		match(ELSE);
		match(OP_BRACE);
		elsep = stmt_seq();
		match(CL_BRACE);
		node->children.push_back(elsep);
	}
	return node;
}

SyntaxTree* cond_exp()
{
	SyntaxTree* left = NULL, * right = NULL, * mid = NULL;
	left = expr();
	if (current_token.tok == EQ || current_token.tok == LTH || current_token.tok == GRTH)
	{
		mid = makeNodeExpr(BOOLEAN);
		mid->children.push_back(left);
		match(current_token.tok);
		right = expr();
		mid->children.push_back(right);
	}
	else
	{
		std::cout << "Error! Expected boolean operator!\n";
		return NULL;
	}
	return mid;
}

SyntaxTree* whileloop()
{
	SyntaxTree* mid = makeNodeStmt(WHILEK);
	SyntaxTree* testc = NULL, * body = NULL;
	match(WHILE);
	match(OP_BRACKET);
	testc = cond_exp();
	match(CL_BRACKET);
	mid->children.push_back(testc);
	match(OP_BRACE);
	body = stmt_seq();
	match(CL_BRACE);
	mid->children.push_back(body);
	return mid;
}

SyntaxTree* dowhileloop()
{
	SyntaxTree* mid = makeNodeStmt(DOWHILEK), * body, * exitcond;
	match(DO);
	match(OP_BRACE);
	body = stmt_seq();
	match(CL_BRACE);
	mid->children.push_back(body);
	match(WHILE);
	match(OP_BRACKET);
	exitcond = cond_exp();
	match(CL_BRACKET);
	mid->children.push_back(exitcond);
	return mid;
}

SyntaxTree* printst()
{
	SyntaxTree* node = makeNodeStmt(PRINTK);
	SyntaxTree* child = NULL;
	match(PRINT);
	if (current_token.tok == STR_LIT)
	{
		child = makeNodeStmt(STRCONST);
		match(STR_LIT);
	}
	else
	{
		child = expr();
	}
	node->children.push_back(child);
	return node;
}
SyntaxTree* inputst()
{
	SyntaxTree* node = makeNodeStmt(READK);
	SyntaxTree* child = NULL;
	match(INPUT);
	if (current_token.tok == ID)
	{
		child = makeNodeStmt(IDENTIFIER);
		match(ID);
	}
	else {
		std::cout << "Error!Expected valid identifier!\n";
		return NULL;
	}
	node->children.push_back(child);
	return node;
}
