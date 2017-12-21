/*
 * parser.cpp
 *
 */

#include <string>
#include <iostream>
#include <map>
#include "parser.h"
using std::string;
using namespace std;




class ParserToken {
private:
	Token	tok;
	bool	pushedBack;

public:
	ParserToken() : pushedBack(false) {}

	Token getToken(istream *in) {
		if( pushedBack ) {
			pushedBack = false;
			return tok;
		}

		return ::getToken(in);
	}
	void pushbackToken(const Token& t) {
		if( pushedBack ) {
			// error
		}
		tok = t;
		pushedBack = true;
	}

} ParserToken;



map<string, string> stringMap;
map<string, int> intMap;
string fileName;
int numID = 0;
int numSet = 0;
int numPlus = 0;
int numStar = 0;

int checkVarExists(string var) {
  if(stringMap.find(var) == stringMap.end() && intMap.find(var) == intMap.end()) {  
    return 0;
  }
  return 1;
}

ParseTree * Prog(istream* in, string file)
{
	fileName = file;
	return StmtList(in);
}

ParseTree * StmtList(istream* in)
{
	ParseTree *stmt = Stmt(in);
	Token tok = ParserToken.getToken(in);
	if(stmt == 0) {
	  return 0;
	}
        if(tok == T_SC) {
          return new StatementList( stmt, StmtList(in));
        }
	else {
	  error(fileName, tok.GetLinenum(), "Syntax error semicolon required");
	}
	return 0;
}

ParseTree * Stmt(istream* in) {
	ParseTree *decl = Decl(in);
	ParseTree *set = Set(in);
	ParseTree *print = Print(in);
	if(decl != 0) {
	  return decl;
	}
	else if(set != 0) {
	  return set;
	}
	else if(print != 0) {
	  return print;
	}
	return 0;
}

ParseTree * Decl(istream* in) {
	Token tok = ParserToken.getToken(in);
	if(tok == T_INT) {
	  Token idTok = ParserToken.getToken(in); 
	  if(idTok == T_ID) {
	    ParseTree * decl = new ParseTree(tok.GetLinenum(), 0, 0, INTDECL_TYPE);
	    decl->SetStringValue(idTok.GetLexeme());
	    return decl;
	  }
	  else {
	    ParserToken.pushbackToken(idTok);
	    cout << "Expected id after int declaration" << endl;
	  }
	}
	else {
	  ParserToken.pushbackToken(tok);
	}
	tok = ParserToken.getToken(in);
	if(tok == T_STRING) {
          Token idTok = ParserToken.getToken(in);
          if(idTok == T_ID) {
	    ParseTree * decl = new ParseTree(tok.GetLinenum(), 0, 0, STRINGDECL_TYPE);
	    decl->SetStringValue(idTok.GetLexeme());
	    return decl;
          }
          else {
            ParserToken.pushbackToken(idTok);
            cout << "Expected id after string declaration" << endl;
          }
        }
        else {
          ParserToken.pushbackToken(tok);
        }
	return 0;
}

ParseTree * Set(istream* in) {
	Token tok = ParserToken.getToken(in);
	if(tok == T_SET) {
	  Token idTok = ParserToken.getToken(in);
	  if(idTok == T_ID) {
	    ParseTree *expr = Expr(in);
	    if(expr == 0) {
	      error(fileName,idTok.GetLinenum(),"Syntax error expression required");
	    }
	    ParseTree * set = new ParseTree(tok.GetLinenum(), expr, 0, SET_TYPE);
	    set->SetStringValue(idTok.GetLexeme());
	    return set;
	  }
	  else {
	    ParserToken.pushbackToken(idTok);
	    cout << "expected id after set" << endl;
	  }
	}
	else {
	  ParserToken.pushbackToken(tok);
	}
	return 0;
}

ParseTree * Print(istream* in) {
	Token tok = ParserToken.getToken(in);
	if(tok == T_PRINT) {
	  ParseTree *expr = Expr(in);
	  if(expr == 0) {
	    error(fileName,tok.GetLinenum(),"Syntax error expression required");
	  }
	  return new ParseTree(tok.GetLinenum(), expr, 0, PRINT_TYPE);
	}
	else if(tok == T_PRINTLN) {
	  ParseTree *expr = Expr(in);
	  if(expr == 0) {
	    error(fileName,tok.GetLinenum(),"Syntax error expression required");
	  }
	  return new ParseTree(tok.GetLinenum(), expr, 0, PRINTLN_TYPE);
	}
	else {
	  ParserToken.pushbackToken(tok);
	  return 0;
	}
}

ParseTree *Expr(istream* in) { 
	ParseTree *t1 = Term(in);
	if( t1 == 0 ) {
		return 0;
	}

	for(;;) {
		Token op = ParserToken.getToken(in);
		if( op != T_PLUS && op != T_MINUS ) {
			ParserToken.pushbackToken(op);
			return t1;
		}

		ParseTree *t2 = Expr(in);
		if( t2 == 0 ) {
			error(fileName, op.GetLinenum(), "expression required after + or - operator");
			return 0;
		}

		// combine t1 and t2 together
		if( op == T_PLUS )
			t1 = new Addition(op.GetLinenum(), t1, t2);
		else
			t1 = new Subtraction(op.GetLinenum(), t1, t2);
	}

	// should never get here...
	return 0;
}

ParseTree * Term(istream* in) {
        ParseTree *t1 = Primary(in);
        if( t1 == 0 ) {
                return 0;
        }

        for(;;) {
                Token op = ParserToken.getToken(in);
                if( op != T_STAR && op != T_SLASH ) {
                        ParserToken.pushbackToken(op);
                        return t1;
                }

                ParseTree *t2 = Term(in);
                if( t2 == 0 ) {
                        error(fileName, op.GetLinenum(), "expression required after * or / operator");
                        return 0;
                }

                // combine t1 and t2 together
                if( op == T_STAR )
                        t1 = new ParseTree(op.GetLinenum(), t1, t2, MULT_TYPE);
                else
                        t1 = new ParseTree(op.GetLinenum(), t1, t2, DIV_TYPE);
        }

        // should never get here...
        return 0;
}

ParseTree * Primary(istream* in) {
	Token tok = ParserToken.getToken(in);
	if(tok == T_ICONST) {
	  return new IntegerConstant(tok);
	}
	else if(tok == T_SCONST) {
	  return new StringConstant(tok);
	}
	else if(tok == T_ID) {
	  return new idVal(tok, 0, 0);
	}
	else if(tok == T_LPAREN) {
	  ParseTree *expr = Expr(in);
	  Token rpTok = ParserToken.getToken(in);
	  if(rpTok != T_RPAREN) {
	    error(fileName, rpTok.GetLinenum(), "Syntax error right paren expected");
	    ParserToken.pushbackToken(rpTok);
	    return 0;
	  }
	  else {
	    return expr;
	  }
	}
	else {
	  error(fileName, tok.GetLinenum(), "Syntax error primary expected");
	}
	return 0;
}

void Traverse(ParseTree * node, bool printLetters) {
	if(node->getLeft() != 0) {
	  if(printLetters) {cout << "L";}
	  Traverse(node->getLeft(), printLetters);
	  if(printLetters) {cout << "u";}
	}
	if(node->getRight() != 0) {
	  if(printLetters) {cout << "R";}
	  Traverse(node->getRight(), printLetters);
	  if(printLetters) {cout << "U";}
	}
	if(node->GetType() == INTDECL_TYPE || node->GetType() == STRINGDECL_TYPE) {
	  numID++;
	}
	else if(node->GetType() == SET_TYPE) {
	  numSet++;
	}
	else if(node->GetType() == ADDITION_TYPE) {
	  numPlus++;
	}
	else if(node->GetType() == MULT_TYPE) {
	  numStar++;
	}
	if(printLetters) {cout << "N";}
}

void checkErrors(ParseTree * node) {
  if(node != 0) {
    if(node->GetType() == INTDECL_TYPE) {
      if( intMap.find(node->GetStringValue()) == intMap.end() && stringMap.find(node->GetStringValue()) == stringMap.end() ) {
        intMap[node->GetStringValue()] = 0;
      }
      else {
        error(fileName, node->getLineNumber(), "variable " + node->GetStringValue() + " was already declared");
      }
    }
    else if(node->GetType() == STRINGDECL_TYPE) {
      if( intMap.find(node->GetStringValue()) == intMap.end() && stringMap.find(node->GetStringValue()) == stringMap.end() ) {
        stringMap[node->GetStringValue()] = "";
      }
      else {
        error(fileName, node->getLineNumber(), "variable " + node->GetStringValue() + " was already declared");
      }
    }
    else if(node->GetType() == SET_TYPE) {
      if( intMap.find(node->GetStringValue()) == intMap.end() && stringMap.find(node->GetStringValue()) == stringMap.end() ) {
        error(fileName, node->getLineNumber(), "variable " + node->GetStringValue() + " is used before being declared");
      }
    }
    else if(node->GetType() == IDVAL_TYPE) {
      if( intMap.find(node->GetIdValue()) == intMap.end() && stringMap.find(node->GetIdValue()) == stringMap.end() ) {
        error(fileName, node->getLineNumber(), "variable " + node->GetIdValue() + " is used before being declared");
      }
    }
    checkErrors(node->getLeft());
    checkErrors(node->getRight());
  }
}
      
    

void printStats() {
  cout << "Total number of identifiers: " << numID << endl;
  cout << "Total number of set: " << numSet << endl;
  cout << "Total number of +: " << numPlus << endl;
  cout << "Total number of *: " << numStar << endl;
}

void printTree(ParseTree *root, int space)
{
    int COUNT = 10;
    // Base case
    if (root == 0)
        return;
 
    // Increase distance between levels
    space += COUNT;
 
    // Process right child first
    printTree(root->getRight(), space);
 
    // Print current node after space
    // count
    printf("\n");
    for (int i = COUNT; i < space; i++)
        printf(" ");
    printf("%d\n", root->GetType());
 
    // Process left child
    printTree(root->getLeft(), space);
}

void error(int linenum, const string& message);
