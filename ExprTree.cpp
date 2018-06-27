#include "ExprTree.h"
#include <sstream>

/*
 * Helper function that tests whether a string is a non-negative integer.
 */

bool isdigit(const char & c) {

	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': return true;
	}

	return false;

}

bool is_number(const std::string & s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

/*
 * Helper function that converts a string to an int.
 */
int to_number(const std::string & s) {
	return atoi(s.c_str());
}

/*
 * Helper function that converts a number to a string.
 */
string to_string(const int & n) {
	std::stringstream stream;
	stream << n;
	return stream.str();
}

/*
 * Additional overloaded helper function that converts a char to a string.
 */
string to_string(const char &c) {
	std::stringstream stream;
	stream << c;
	return stream.str();
}

/*
 * Helper function that creates a TreeNode with the appropriate operator
 * when given a string that's "+", "-", "*" or "/". If the string is wrong
 * it gives a NoOp value.
 */
TreeNode * createOperatorNode(const string & op) {

	if (op == "+") return new TreeNode(Plus);
	if (op == "-") return new TreeNode(Minus);
	if (op == "*") return new TreeNode(Times);
	if (op == "/") return new TreeNode(Divide);
	return new TreeNode(NoOp);

}

/*
 * Recursive function that counts the size of nodes in the tree.
 * It return 0 if the size is null.
 * +1 of the return statement is for the current node.
 */
int countSize(TreeNode *r) {
	if (r == NULL)
		return 0;
	return (countSize(r->getLeftChild()) + countSize(r->getRightChild()) + 1);
}

/*
 * Basic constructor that sets up an empty Expr Tree.
 */
ExprTree::ExprTree() {
	root = NULL;
	_size = countSize(NULL);
}

/*
 * Constructor that takes a TreeNode and sets up an ExprTree with that node at the root.
 */
ExprTree::ExprTree(TreeNode * r) {
	root = r;
	_size = countSize(r);
}

/*
 * Destructor to clean up the tree.
 */
ExprTree::~ExprTree() {
	delete root;
}

/*
 * This function takes a string representing an arithmetic expression and breaks
 * it up into components (number, operators, parentheses).
 * It returns the broken up expression as a vector of strings.
 *
 * Algortihm:
 * Scan the expression string.
 * If it is not a whitespace,
 *	check if the output vector is not empty,
 *	the scanned char is a digit and the previous element in the vector is a non-negative integer,
 *	convert the scanned digit into a string and append it to the previous number in the vector.
 *	This is to deal with more than 1 digit expression.
 * Else, convert the scanned char into a string and push into the back of the vector.
 */
vector<string> ExprTree::tokenise(string expression) {
	vector<string> vec;

	for (string::iterator i = expression.begin(); i != expression.end(); ++i) {
		if (*i != ' ') {
			if (!vec.empty() && isdigit(*i) && is_number(vec[vec.size() - 1]))				
				vec[vec.size() - 1] += to_string(*i);
			else
				vec.push_back(to_string(*i));
		}
	}
	return vec;
}

/*
 * This function returns the precedence level of the operators.
 * Lower level number means lower precedence.
 * Higher level number means higher precedence which goes first.
 */
int getPrecedence(string op) {
	if (op == "(")
		return 0;
	if (op == "+" || op == "-")
		return 1;
	if (op == "*" || op == "/")
		return 2;
	return 3;
}

/*
 * This function takes the infix notation of the vector of strings
 * and converts it into its postfix notation.
 * It returns the postfix notation in a vector of strings.
 * 
 * Algorithm:
 * Scan the infix notation from left to right.
 * If it is an open parenthesis, push it onto the stack.
 * Else if it is a close parenthesis, 
 * push the value from the stack into the back of the vector until an open parenthesis is encountered.
 * Else it is either a number or an operator.
 *	If is a number (operand), push into the back of the vector.
 *	Else it is an operator, while the stack is not empty and
 *	the precedence of operator in the stack is equal or higher than
 *	the precedence of the scanned operator, 
 *	push into the back of the vector and pop.
 *	Push the scanned operator onto the stack.
 * While the stack is not empty, push into the back of the vector and pop.
 * Return the postfix vector of strings.
 */
vector<string> to_postfix(vector<string> tokens) {
	stack<string> opStack;
	vector<string> vec;

	for (vector<string>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
		if ((*i) == "(")
			opStack.push(*i);
		else if ((*i) == ")") {
			while (opStack.top() != "(") {
				vec.push_back(opStack.top());
				opStack.pop();
			}
			opStack.pop();
		}
		else {
			if (is_number(*i))
				vec.push_back(*i);
			else {
				while (!opStack.empty() && getPrecedence(opStack.top()) >= getPrecedence(*i)) {
					vec.push_back(opStack.top());
					opStack.pop();
				}
				opStack.push(*i);
			}
		}
	}
	while (!opStack.empty()) {
		vec.push_back(opStack.top());
		opStack.pop();
	}
	return vec;
}

/*
 * This function takes a vector of strings representing an expression (as produced
 * by tokenise(string), and builds an ExprTree representing the same expression.
 * 
 * Algorithm:
 * Convert the infix tokens into postfix (using stack) to build the tree with postfix tokens.
 * Scan the postfix tokens from left to right.
 *	If it is a number:
 *	Create a number node (converting the string into int type) and push it onto the stack.
 *	Else if it is an operator:
 *	Create an operator node, set the right and left childs, and then push the operator node onto the stack.
 * If the stack is empty, return null.
 * Else return the top of the stack.
 */
ExprTree ExprTree::buildTree(vector<string> tokens) {
	stack<TreeNode *> nodeStack;
	vector<string> postfix = to_postfix(tokens);

	for (vector<string>::const_iterator i = postfix.begin(); i != postfix.end(); i++) {
		if (is_number(*i))
			nodeStack.push(new TreeNode(to_number(*i)));
		else {
			TreeNode *op = createOperatorNode(*i);
			op->setRightChild(nodeStack.top());
			nodeStack.pop();
			op->setLeftChild(nodeStack.top());
			nodeStack.pop();
			nodeStack.push(op);
		}
	}

	if (nodeStack.empty())
		return NULL;
	else
		return nodeStack.top();
}

/*
 * This function takes a TreeNode and does the maths to calculate
 * the value of the expression it represents.
 */
int ExprTree::evaluate(TreeNode * n) {
	switch (n->getOperator()) {
	case Plus:
		return (evaluate(n->getLeftChild()) + evaluate(n->getRightChild()));
	case Minus:
		return (evaluate(n->getLeftChild()) - evaluate(n->getRightChild()));
	case Times:
		return (evaluate(n->getLeftChild()) * evaluate(n->getRightChild()));
	case Divide:
		return evaluate(n->getLeftChild()) / evaluate(n->getRightChild());
	}
	return n->getValue();
}

/*
 * When called on an ExprTree, this function calculates the value of the
 * expression represented by the whole tree.
 */
int ExprTree::evaluateWholeTree() {
	return evaluate(root);
}

/*
 * Given an ExprTree t, this function returns a string
 * that represents that same expression as the tree in
 * prefix notation.
 */
string ExprTree::prefixOrder(const ExprTree &t) {
	switch (t.root->getOperator()) {
	case Value: return t.root->toString();
	case Plus:
	case Minus:
	case Times:
	case Divide:
		return t.root->toString() + " " + prefixOrder(t.root->getLeftChild()) + " " + prefixOrder(t.root->getRightChild());
	default:
		return "";
	}
}

/*
 * Given an ExprTree t, this function returns a string
 * that represents that same expression as the tree in
 * infix notation.
 */
string ExprTree::infixOrder(const ExprTree &t) {
	switch (t.root->getOperator()) {
	case Value: return t.root->toString();
	case Plus:
	case Minus:
	case Times:
	case Divide:
		return infixOrder(t.root->getLeftChild()) + " " + t.root->toString() + " " + infixOrder(t.root->getRightChild());
	default:
		return "";
	}
}

/*
 * Given an ExprTree t, this function returns a string
 * that represents that same expression as the tree in
 * postfix notation.
 */
string ExprTree::postfixOrder(const ExprTree &t) {
	switch (t.root->getOperator()) {
	case Value: return t.root->toString();
	case Plus:
	case Minus:
	case Times:
	case Divide:
		return postfixOrder(t.root->getLeftChild()) + " " + postfixOrder(t.root->getRightChild()) + " " + t.root->toString();
	default:
		return "";
	}
}

/*
 * Returns the size of the tree. (i.e. the number of nodes in it)
 */
int ExprTree::size() { return _size; }

/*
 * Returns true if the tree contains no nodes. False otherwise.
 */
bool ExprTree::isEmpty() { return _size == 0; }

/*
 * Returns the root of the tree.
 */
TreeNode * ExprTree::getRoot() { return root; }
