#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "lexer.h"
#include "parser.h"
using namespace std;

/*
 * main.cpp
 */

int lineNumber = 1;
bool foundErrors = false;

void error(string fileName, int linenum, const string& message) {
	cout << fileName << ":" << linenum << ":" << message << endl;
	foundErrors = true;
}

int main(int argc, char *argv[])
{
  bool tFlag = false;
  vector <string> fileList;
  for(int i=1; i<argc; i++) {
     if(argv[i][0] == '-') {
        if(argv[i][1] == 't') {
           tFlag = true;
        }
        else {
           cout << argv[i] << " UNRECOGNIZED FLAG" << endl;
           return 0;
        }
     }
     else {
        fileList.push_back(argv[i]);
     }
  }
  if(fileList.size() > 1) {
     cout << "TOO MANY FILES" << endl;
     return 0;
   }
   ifstream myfile;
   if(fileList.size() > 0) {myfile.open(fileList.front());}
   if(myfile.is_open()) {
     ParseTree *tree = Prog( &myfile , fileList.front());
     if(tFlag) {
       Traverse(tree, true);
       //printTree(tree, 0);
       cout << endl;
     }
     if(!foundErrors) {
       checkErrors(tree);
     }
     if(!foundErrors) {
       if(!tFlag) {Traverse(tree, false);}
       printStats();
       //printTree(tree, 0);
     }
   }
   else if(fileList.size() == 0){
     ParseTree *tree = Prog( &cin , "");
     if(tFlag && !foundErrors) {
       Traverse(tree, true);
       cout << endl;
     }
     if(!foundErrors) {
       checkErrors(tree);
     }
     if(!foundErrors) {
       if(!tFlag) {Traverse(tree, false);}
       printStats();
       //printTree(tree, 0);
     }
   }
   else {
     cout << fileList.front() << " FILE NOT FOUND" << endl;
   }
   return 0;
}


