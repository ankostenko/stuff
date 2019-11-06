#pragma once

/**
	Syntax was taken from : https://docs.pexip.com/admin/regex_reference.htm#syntax
	(without (?<!...))

	Plan:
	Convert regex to NFA transform into graf and bypass by DFS

	without {...} kvantificator because i tired after branches

	Also [a-Z] ilegal, just [a-zA-Z] other same [a-zA-Z1-9] or [a-z 1234 AB]

	\ chanched by / or [] (\@ incorect , /@ good , [@] good)

	negation represent only for single elem like [^...] , not a == [^a], not google == [^g][^o][^o]... absolute pain but easilly can be fixed but  and so it goes))

	branches can be difucult like a+ | (b?a*)+ | ....


	string aa("sa00709@mail.ru");
	string aa("sa00709@ya.ru");
	string aa("sa00709@gmail.com");
	string regex("^ [. a-z A-Z 0-9]+ /@ (mail)|(gmail)|(ya) /. (ru)|(com) $");

	it working fine 
*/

#include <ctype.h>
#include "string.h"
#include "vector.h"
#include "algorithms.h"


namespace my{

	const char PLUS = 110;
	const char STAR = 111;
	const char QUESTION = 112;
	const char OPEN_BRACK = 113;
	const char CLOSE_BRACK = 114;
	const char _DOT = 115;

	// Discribe the Node
	enum Essence {
		DEFAULT,
		EMPTY,
		START, // for intermidiante nodes
		END,
		SINGLE,
		DOT,
		ANY_DIGIT,
		ANY_ALPHA,
		SPACE,
		SQUR_BRACKETS
	};

	struct Node {
		bool negation{ false };
		Essence essence{ DEFAULT };
		vector<char> values;

		Node() = default;

		explicit Node(Essence essence) : essence(essence) {}

	};

	bool start_flag = false; // ^
	bool end_flag = false;  // $


	static bool regValidity(string& regex) {
		int brackets_checker = 0;
		
		for (int i = 0; i < regex.size();) {
			switch (regex[i]){
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
				case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
				case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': {
					++i;
					break;
				}
				case '[': {
					while (regex[++i] != ']') {
						if (i == regex.size())
							return false;

						if (regex[i] == '[')
							return false;

						if (regex[i] == '^' && regex[i - 1] != '[')
							return false;

						if (regex[i] == '-' && (!isalnum(regex[i - 1]) || !isalnum(regex[i + 1])))
							return false;
					}
					break;
				}
				case ']': {
					i++;
					break;
				}
				case '(': {
					++i;
					++brackets_checker;
					break;
				}
				case ')': {
					++i;
					--brackets_checker;
					break;
				}
				case '{': {
					if (!isdigit(regex[++i]))
						return false;

					if (regex[i] == '}') {
						++i;
						break;
					}

					if (regex[i++] != ',')
						return false;

					if (regex[i] == '}') {
						++i;
						break;
					}

					if (!isdigit(regex[i++]))
						return false;

					if (regex[i] == '}') {
						++i;
						break;
					}
					else
						return false;
				}
				case '*': {
					if (regex[i - 1] != ']' && !isalnum(regex[i - 1]) && regex[i - 1] != ')')
						return false;
					++i;
					break;
				}
				case '+': {
					if (regex[i - 1] != ']' && !isalnum(regex[i - 1]) && regex[i - 1] != ')')
						return false;
					++i;
					break;
				}
				case '?': {
					if (regex[i - 1] != ']' && !isalnum(regex[i - 1]) && regex[i - 1] != ')')
						return false;
					++i;
					break;
				}
				case '.': {
					++i;
					break;
				}
				case '/': {
					i += 2;
					break;
				}
				case '^': {
					if (i != 0 && regex[i - 1] != '[')
						return false;
					++i;
					break;
				}
				case '$': {
					if (i++ != regex.size() - 1)
						return false;
					break;
				}
				case ' ': {
					++i;
					break;
				}
				case '|': {
					++i;
					break;
				}
			}
		}
		if (brackets_checker)
			return false;

		return true;
	}


	static int backOneLexAgo(string& regex, int i) {
		if (regex[i] == '|')
			--i;

		if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?')
			--i;

		if (regex[i] == ')')
			while (regex[--i] != '(');

		return i;
	}

	static int lastBranch(string& regex, int i) {
		if (regex[i] == '|')
			++i;

		if (regex[i] == '(')
			while (regex[i++] != ')');
		else
			++i;

		if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?')
			++i;

		if (regex[i] == '|')
			return 0;
		else
			return i;
	}

	static string highLightBranches(string regex) {  // highLight branches 1 a|b|c 1,  neccesery for graf bulding
		bool branchFlag = false;

		for (int i = 0; i < regex.size(); i++) {

			while (i < regex.size() && regex[++i] != '|');

			if (!branchFlag && regex[i] == '|') {
				regex.insert(1, backOneLexAgo(regex, i++));
				branchFlag = true;
			}

			if (branchFlag && lastBranch(regex, i)) {
				i = lastBranch(regex, i);
				regex.insert(1, i);
				branchFlag = false;
			}
		}

		return regex;
	}

	static void parser(string& regex, vector<char>& stack, vector<Node>& nodes) {
		stack << 0;
		nodes << Node(START);  // start node
		bool branchFlag = false;


		for (int i = 0; i < regex.size();) {

			switch (regex[i]) {
			case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
			case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
			case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': {
				nodes << Node(SINGLE);
				nodes.back().values << regex[i++];
				stack << nodes.size() - 1;

				break;
			}
			case '[': {
				vector<char> vals;
				bool negation = false;
				while (regex[++i] != ']') {
					char loockahead = regex[i + 1];

					if (regex[i] == '^') {
						negation = true;
						continue;
					}

					if (loockahead == '-') {
						for (char ch = regex[i]; ch < regex[i + 2]; ++ch)
							vals << ch;
						++i;
						continue;
					}

					vals << regex[i];
				}
				nodes << Node(SQUR_BRACKETS);
				nodes.back().values = std::move(vals);
				nodes.back().negation = negation;
				stack << nodes.size() - 1;
				break;
			}
			case ']': {
				i++;
				break;
			}
			case '(': {
				++i;
				stack << OPEN_BRACK;
				nodes << Node(EMPTY);
				stack << nodes.size() - 1;
				break;
			}
			case ')': {
				i++;
				nodes << Node(EMPTY);
				stack << nodes.size() - 1;
				stack << CLOSE_BRACK;
				break;
			}
			case '{': {
				while (regex[i] != '}') {
					stack << regex[i++];
				}
				++i;
				break;
			}
			case 1: {
				++i;
				if (branchFlag) {
					stack << 100;
					branchFlag = false;

					nodes << Node(EMPTY);
					stack << nodes.size() - 1;
				}
				else {
					nodes << Node(EMPTY);
					stack << nodes.size() - 1;

					stack << 100;
					branchFlag = true;
				}
				break;
			}
			case '|': {
				stack << regex[i++];
				break;
			}
			case '*': {
				++i;
				stack << STAR;
				break;
			}
			case '+': {
				++i;
				stack << PLUS;
				break;
			}
			case '?': {
				++i;
				stack << QUESTION;
				break;
			}
			case '.': {
				++i;
				nodes << Node(DOT);
				stack << nodes.size() - 1;
				break;
			}
			case '/': {
				++i;
				Essence ess = DEFAULT;
				bool negation = false;
				bool flag_escape = false;

				switch (regex[i]) {
				case 'a': {
					ess = ANY_ALPHA;
					break;
				}
				case 'A': {
					ess = ANY_ALPHA;
					negation = true;
				}
				case 'd': {
					ess = ANY_DIGIT;
					break;
				}
				case 'D': {
					ess = ANY_DIGIT;
					negation = true;
					break;
				}
				case 'w': {
					ess = SPACE;
					break;
				}
				case 'W': {
					ess = SPACE;
					negation = true;
					break;
				}
				default: {                    // Escapes a regular expression special character: {}[]()^ $.|*+?\ 
					nodes << Node(SINGLE);
					nodes.back().values << regex[i++];
					stack << nodes.size() - 1;
					flag_escape = true;
					break;
				}
				}
				if (flag_escape)
					break;

				nodes << Node(ess);
				nodes.back().negation = negation;
				stack << nodes.size() - 1;
				break;
			}
			case '^': {
				start_flag = true;
				++i;
				break;
			}
			case '$': {
				end_flag = true;
				++i;
				break;
			}
			case ' ': {
				++i;
				break;
			}
			}
		}

		nodes << Node(END);  // end node
		stack << nodes.size() - 1;
	}


	static int backToBracketsStart(vector<char>& stack, int curPos) {  // return ...(<-....) this position
		int count = 0;
		while (count != -1) {
			if (stack[curPos] == OPEN_BRACK)
				--count;

			if (stack[curPos] == CLOSE_BRACK)
				++count;

			if (count == 0)
				break;
			--curPos;
		}
		return curPos;
	}

	static char PrivNodeInStack(vector<char>& stack, int curPos) {
		while (stack[--curPos] >= 100);
		return stack[curPos];
	}

	static char NextNodeInStack(vector<char>& stack, int curPos) {
		while (curPos <  stack.size() && stack[++curPos] >= 100);
		return stack[curPos];
	}

	static void buildGraf(vector<char>&  stack, vector<vector<int>>& graf) {
		char lastElem = 0;

		for (int i = 0; i < stack.size(); i++) {
			if (stack[i] < 110) { // != ( , ) , * and so on
				if (stack[i] < graf.size() - 1 && i != stack.size() - 1 && stack[i + 1] != 100)
					graf[stack[i]] << NextNodeInStack(stack, i);
				lastElem = stack[i];
			}
			else {
				char privius = stack[i - 1];
				switch (stack[i]) {
				case PLUS: {
					if (privius == CLOSE_BRACK) {
						int openBrPos = backToBracketsStart(stack, i - 1); // pos of '('
						graf[lastElem] << stack[openBrPos + 1];
					}
					else {
						graf[lastElem] << lastElem;
					}
					break;
				}
				case STAR: {
					if (privius == CLOSE_BRACK) {
						int openBrPos = backToBracketsStart(stack, i - 1);
						graf[lastElem] << stack[openBrPos + 1];		// right of the open br guaranteed stay Node ...(someNode....) so just i + 1

						graf[PrivNodeInStack(stack, openBrPos)] << NextNodeInStack(stack, i);  // ->a(c+b)*d<- , bind a and d
					}
					else {
						graf[lastElem] << lastElem;
						graf[PrivNodeInStack(stack, i - 1)] << NextNodeInStack(stack, i); // i - 1 because  ab*<- we need a so just skip b
					}
					break;
				}
				case QUESTION: {
					if (privius == CLOSE_BRACK) {
						int openBrPos = backToBracketsStart(stack, i - 1);
						graf[PrivNodeInStack(stack, openBrPos)] << NextNodeInStack(stack, i);
					}
					else {
						graf[PrivNodeInStack(stack, i - 1)] << NextNodeInStack(stack, i);
					}
					break;
				}
				case '{': {
					//... maybe latter
					break;
				}
				case 100: {
					int j = ++i;
					char startOfBranch = PrivNodeInStack(stack, i);  // start and end are empty nodes
					while (stack[j++] != 100);
					char endOfbranch = stack[j];
					vector<char> new_stack;
					new_stack << startOfBranch;

					for (; i < j; i++) {
						if (stack[i] == '|' || stack[i] == 100) {
							new_stack << endOfbranch;
							buildGraf(new_stack, graf);
							new_stack.clear();
							new_stack << startOfBranch;
							continue;
						}
						new_stack << stack[i];
					}
					--i;
					break;
				}
				case OPEN_BRACK: {
					break;
				}
				case CLOSE_BRACK: {
					break;
				}
				}
			}
		}
	}

	static bool correspond(char& ch, Node& node) {
		bool res = false;
		switch (node.essence) {
		case START: {
			return true;
		}
		case END: {
			return true;
		}
		case EMPTY: {
			return true;
		}
		case DOT: {
			return true;
		}
		case SINGLE: {
			res = node.values[0] == ch;
			break;
		}
		case ANY_DIGIT: {
			res = isdigit(ch);
			break;
		}
		case ANY_ALPHA: {
			res = isalpha(ch);
			break;
		}
		case SPACE: {
			res = ch == ' ';
			break;
		}
		case SQUR_BRACKETS: {
			if (find(node.values.begin(), node.values.end(), ch))
				res = true;
			break;
		}
		}
		return node.negation ? !res : res;
	}

	static bool dfs(vector<vector<int>>& graf, vector<Node>& nodes, int grafPos, string& target, int strPos) {
		
		if (!correspond(target[strPos], nodes[grafPos]))
			return false;

		if (nodes[grafPos].essence == END && (!end_flag || strPos == target.size()))
			return true;

		if (strPos == target.length() && nodes[grafPos].essence != END)
			return false;

		if (nodes[grafPos].essence == START || nodes[grafPos].essence == EMPTY) // crutch
			--strPos;

		for (int i = 0; i < graf[grafPos].size(); i++) {
			if (dfs(graf, nodes, graf[grafPos][i], target, strPos + 1))
				return true;
		}

		return false;
	}

	int match(string& target, string& regex, bool check = true) {
		vector<Node> nodes;
		vector<char> stack;

		if (check) {
			if (!regValidity(regex))
				return -2;
		}

		string preparedRegex = highLightBranches(regex);
		parser(preparedRegex, stack, nodes);
		vector<vector<int>> graf(nodes.size(), vector<int>());
		buildGraf(stack, graf);

		//for (int i = 0; i < graf.size(); i++) {          // print graf
		//	std::cout << i << '-' << ' ';
		//	for (int j = 0; j < graf[i].size(); j++) {
		//		if (j)
		//			std::cout << ' ';
		//		std::cout  << graf[i][j];
		//	}
		//	std::cout << '\n';
		//}

		
		for (int i = 0; i < target.size(); i++) {
			if (dfs(graf, nodes, 0, target, i))
				return i;

			if (start_flag)  // only one ckeck form begin
				break;
		}

		return -1;
	}
}