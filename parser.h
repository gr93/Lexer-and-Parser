/*
 * parser.h
 *
 *  Created on: Oct 23, 2017
 *      Author: gerardryan
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
using std::istream;

#include <string>
using std::string;
using std::stoi;

#include "lexer.h"

extern void error(string fileName, int linenum, const string& message);

enum TypeForNode { STRINGDECL_TYPE, INTDECL_TYPE, IDVAL_TYPE, INT_TYPE, STRING_TYPE, ERROR_TYPE, STMTLIST_TYPE, STMT_TYPE , PRINT_TYPE, SET_TYPE, ADDITION_TYPE, PRINTLN_TYPE, MULT_TYPE, DIV_TYPE};

class ParseTree {
	int	        linenumber;
	ParseTree	*left;
	ParseTree	*right;
	TypeForNode	type;
	string		value = "";

public:
	ParseTree(int n, ParseTree *l = 0, ParseTree *r = 0, TypeForNode type = ERROR_TYPE) : linenumber(n), left(l), right(r), type(type)  {}
	virtual ~ParseTree() {}

	ParseTree* getLeft() const { return left; }
	ParseTree* getRight() const { return right; }
	int getLineNumber() const { return linenumber; }

	virtual TypeForNode GetType() const { return type; }
	virtual int GetIntValue() const { throw "no integer value"; }
	virtual string GetStringValue() const { return value; }
	virtual void SetStringValue(string val) { value = val; }
	virtual string GetIdValue() const { throw "no id value"; }
};

class StatementList : public ParseTree {
public:
	StatementList(ParseTree *first, ParseTree *rest) : ParseTree(0, first, rest, STMTLIST_TYPE) {}

};

class Addition : public ParseTree {
public:
	Addition(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	// will need to fill in type and value;
	// remember type is a function of
	virtual TypeForNode GetType() const { return ADDITION_TYPE; }
};

class Subtraction : public ParseTree {
public:
	Subtraction(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2) {}

	// will need to fill in type and value;
	// remember type is a function of
};

class IntegerConstant : public ParseTree {
	int	value;

public:
	IntegerConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
		value = stoi( tok.GetLexeme() );
	}

	TypeForNode GetType() const { return INT_TYPE; }
	int GetIntValue() const { return value; }
};

class StringConstant : public ParseTree {
        string     value;

public:
        StringConstant(const Token& tok) : ParseTree(tok.GetLinenum()) {
                value = tok.GetLexeme();
        }

        TypeForNode GetType() const { return STRING_TYPE; }
        string GetStringValue() const { return value; }
};

class intDecl : public ParseTree {
public:
        intDecl(int line, ParseTree *left, ParseTree *right) : ParseTree(line, left, right) {}

        TypeForNode GetType() const { return INTDECL_TYPE; }
};

class stringDecl : public ParseTree {
public:
        stringDecl(int line, ParseTree *left, ParseTree *right) : ParseTree(line, left, right) {}

        TypeForNode GetType() const { return STRINGDECL_TYPE; }
};

class idVal : public ParseTree {
	string value;
public:
        idVal(const Token& tok, ParseTree *left, ParseTree*right) : ParseTree(tok.GetLinenum(), left, right) {
		value = tok.GetLexeme();
        }

        TypeForNode GetType() const { return IDVAL_TYPE; }
	string GetIdValue() const { return value; }
};

extern ParseTree *	Prog(istream* in, string file);
extern ParseTree *	StmtList(istream* in);
extern ParseTree *	Stmt(istream* in);
extern ParseTree *	Decl(istream* in);
extern ParseTree *	Set(istream* in);
extern ParseTree *	Print(istream* in);
extern ParseTree *	Expr(istream* in);
extern ParseTree *	Term(istream* in);
extern ParseTree *	Primary(istream* in);
extern void             Traverse(ParseTree * node, bool printLetters);
extern void             Count(ParseTree * node);
extern void 		printTree(ParseTree * node, int count);
extern void		printStats();
extern void		checkErrors(ParseTree * node);

#endif /* PARSER_H_ */
