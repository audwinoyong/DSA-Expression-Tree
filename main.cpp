//#include "ExprTree.h"
//#include <sstream>
//#include <iostream>
//using namespace std;
//
///*
// * Main function for own testing.
// */
//int main() {
//	string s;
//	cout << "Input Infix Expression: " << endl;
//	getline(cin, s);
//
//	ExprTree* t = new ExprTree();
//	vector<string> infix = t->tokenise(s);
//	vector<string> postfix = t->to_postfix(infix);
//	
//	cout << "Postfix expression is: " << endl;
//	for (vector<string>::const_iterator i = postfix.begin(); i != postfix.end(); i++) {
//		cout << (*i) << " ";
//	}
//	cout << endl;
//}