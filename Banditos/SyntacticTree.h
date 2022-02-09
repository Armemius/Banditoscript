#pragma once
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include "Token.h"

#define SYNTDBG

namespace bndts {
	namespace synt {
		// Bool for right-associated
		auto prio = std::map<std::string, std::pair<int, bool>> {
			{"++t", {0, false}},
			{"--t", {0, false}},
			{"++r", {1, true}},
			{"--r", {1, true}},
			{"-u", {1, true}},
			{"+u", {1, true}},
			{"!u", {1, true}},
			{"~u", {1, true}},
			{"&u", {1, true}},
			{"*u", {1, true}},
			{"*b", {2, false}},
			{"/b", {2, false}},
			{"%b", {2, false}},
			{"+b", {3, false}},
			{"-b", {3, false}},
			{"<<b", {4, false}},
			{">>b", {4, false}},
			{"<b", {5, false}},
			{">b", {5, false}},
			{"<=b", {5, false}},
			{">=b", {5, false}},
			{"==b", {6, false}},
			{"!=b", {6, false}},
			{"&b", {7, false}},
			{"^b", {8, false}},
			{"|b", {9, false}},
			{"&&b", {10, false}},
			{"||b", {11, false}},
			{"=b", {12, true}},
			{"*=b", {12, true}},
			{"/=b", {12, true}},
			{"%=b", {12, true}},
			{"+=b", {12, true}},
			{"-=b", {12, true}},
			{"<<=b", {12, true}},
			{">>=b", {12, true}},
			{"&=b", {12, true}},
			{"|=b", {12, true}},
			{"^=b", {12, true}},
			{"(", {13, false}},
			{")", {13, false}}
		};

		struct Node {
			std::list<Node*> nodes;
			std::string token = "";
		};

		void Err(const Token& tt, const std::string& orig, std::string exp = "") {
			int start = tt.pos - tt.col, fin = tt.pos - tt.col;
			while (orig[fin] != '\n' && fin < orig.size())
				fin++;
			std::string line = "\x1B[32m" + orig.substr(start, tt.col) + "\033[0m"
				+ "\x1B[31m" + orig.substr(tt.pos, tt.orig.size()) + "\033[0m"
				+ "\x1B[33m" + orig.substr(tt.pos + tt.orig.size(), (fin == tt.pos ? 0 :fin - tt.pos - tt.orig.size())) + "\033[0m";
			std::string arrow = "";
			for (int i = 0; i < tt.col; ++i)
				arrow += ' ';
			std::string err = "Unexpected token \"" + tt.value + ":" + tt.id + "\" at "
				+ tt.filename + ":" + std::to_string(tt.pos) + " (Col: " + std::to_string(tt.col)
				+ "; Line: " + std::to_string(tt.line) + ")" + (exp.size() ? "\n\rExpected: " + exp : "") + "\n\r"
				+ line + (fin == tt.pos ? "" : "\n\r") + arrow + "^\n\r";
			throw std::exception(err.c_str());
		}

		inline bool Check(const Token& token, const std::string& id, const std::string& value) {
			return token.id == id && token.value == value;
		}

		inline bool Check(const Token& token, const std::string& id) {
			return token.id == id;
		}

		Node* ParseBlock(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig);
		Node* ParseExpr(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig);

		Node* ParseCall(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseCall\n\r";
#endif 

			if (Check(tk[pos], "ID")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "function name");
			}
			if (Check(tk[pos], "BRACKETS", "(")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "opening bracket");
			}
			pos--;
			do {
				pos++;
				ParseExpr(list, tk, pos, orig);
			} while (Check(tk[pos], "GRAMMAR", ","));
			if (Check(tk[pos], "BRACKETS", ")")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "closing bracket");
			}
			return 0;
		}

		Node* ParseExpr(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseExpr\n\r";
			//std::cout << tk[pos].orig << "\n\r";
#endif 
			int origp = pos;
			static int brb = 0;
			auto exprtkn = std::vector<Token>();
			for (;;)
			if (Check(tk[pos], "VALUE")) {
				exprtkn.push_back(tk[pos]);
				pos++;
			} else if (Check(tk[pos], "KEYWORD", "true")) {
				exprtkn.push_back(tk[pos]);
				exprtkn.back().id = "VALUE";
				pos++;
			} else if (Check(tk[pos], "KEYWORD", "false")) {
				exprtkn.push_back(tk[pos]);
				exprtkn.back().id = "VALUE";
				pos++;
			} else if (Check(tk[pos], "KEYWORD", "false")) {
				exprtkn.push_back(tk[pos]);
				exprtkn.back().id = "VALUE";
				pos++;
			} else if (Check(tk[pos], "OPERATION")) {
				exprtkn.push_back(tk[pos]);
				pos++;
			}
			else if (Check(tk[pos], "ID") && Check(tk[pos + 1], "BRACKETS", "(")) {
				exprtkn.push_back(tk[pos]);
				ParseCall(list, tk, pos, orig);
			}
			else if (Check(tk[pos], "ID")) {
				exprtkn.push_back(tk[pos]);
				pos++;
			}
			else if (Check(tk[pos], "BRACKETS", "(")) {
				exprtkn.push_back(tk[pos]);
				pos++;
				brb++;
			}
			else if (Check(tk[pos], "BRACKETS", ")") && brb > 0) {
				exprtkn.push_back(tk[pos]);
				pos++;
				brb--;
			}
			else
				break;
			if (origp == pos)
				Err(tk[pos], orig, "expression");
			if (brb != 0) 
				Err(tk[pos], orig, "bracket");
			for (int it = 0; it < exprtkn.size(); ++it) {
				if (exprtkn[it].id == "OPERATION") {

					Token &curr = exprtkn[it], prev, next;
					if (it > 0) {
						prev = exprtkn[it - 1];
					}
					else {
						prev.id = "NULL";
						prev.value = "NULL";
					}
					if (it < exprtkn.size() - 1) {
						next = exprtkn[it + 1];
					}
					else {
						next.id = "NULL";
						next.value = "NULL";
					}


					if (curr.value == "++" || curr.value == "--") {
						if (prev.id == "BRACKETS" && prev.value == ")" || prev.id == "VALUE" || prev.id == "ID")
							curr.value += "t";
						else if (next.id == "BRACKETS" && next.value == "(" || next.id == "VALUE" || next.id == "ID")
							curr.value += "r";
						else
							Err(curr, orig, "expression for increment or decrement");
					}
					else if ((prev.id == "BRACKETS" && prev.value == ")" || prev.id == "VALUE" || prev.id == "ID" || prev.value == "++t" || prev.value == "--t")
						&& (next.id == "BRACKETS" && next.value == "(" || next.id == "VALUE" || next.id == "ID" || next.value == "-" || next.value == "--" 
							|| next.value == "++" || next.value == "*" || next.value == "&" || next.value == "!" || next.value == "~"))
						curr.value += 'b';
					else if (!(prev.id == "BRACKETS" && prev.value == ")" || prev.id == "VALUE" || prev.id == "ID")
							&& (next.id == "BRACKETS" && next.value == "(" || next.id == "VALUE" || next.id == "ID"))
							curr.value += 'u';
					else
						Err(exprtkn[it], orig, "expressions for binary operation");
				}
#ifdef SYNTDBG
				std::cout << exprtkn[it].value <<  " ";
#endif
			}
#ifdef SYNTDBG
			std::cout << "\n\r";
#endif
			auto prev = exprtkn[0];
			int st = 0;

			for (auto& it : exprtkn) {
#ifdef SYNTDBG
				std::cout << it.value << "(" << it.id << ") " << st << "\n\r";
#endif
				// 1 for bin op, 0 for value
				if (prev.value == "(" && it.value == ")")
					Err(it, orig, "expression");
				if (st == 0) {
					if (it.id == "VALUE" || it.id == "ID")
						st = 1;
					else if (it.id == "BRACKETS" && it.value == "(")
						st = 0;
					else if (it.id == "BRACKETS" && it.value == ")")
						st = 0;
					else if (it.id == "OPERATION" && it.value.back() == 'b')
						Err(it, orig, "value");
				} else {
					if (it.id == "BRACKETS" && it.value == ")")
						st = 1;
					if (it.id == "BRACKETS" && it.value == "(")
						st = 0;
					if (it.id == "OPERATION" && it.value.back() == 'b')
						st = 0;
					else if (it.id == "VALUE" || it.id == "ID")
						Err(it, orig, "binary operation");
				}
				prev = it;
			}
			auto stk = std::stack<Token>();
			auto res = std::vector<Token>();
			
			for (auto& tk : exprtkn) {
				for (auto& tk : res) {
					std::cout << tk.value << " ";
				}
				std::cout << "\n\r" << stk.size() << " " << res.size() << "\n\r";
				if (tk.id == "VALUE" || tk.id == "ID" || tk.id == "OPERATION" && tk.value.back() == 't') {
					res.push_back(tk);
				}
				else if (tk.id == "OPERATION" && (tk.value.back() == 'r' || tk.value.back() == 'u')) {
					stk.push(tk);
				}
				else if (Check(tk, "BRACKETS", "(")) {
					stk.push(tk);
				}
				else if (Check(tk, "BRACKETS", ")")) {
					if (stk.empty())
						Err(tk, orig, "bracket");
					while (!Check(stk.top(), "BRACKETS", "(")) {
						res.push_back(stk.top());
						stk.pop();
						if (stk.empty())
							Err(tk, orig, "bracket");
					}
					stk.pop();
				}
				else if (tk.id == "OPERATION" && (tk.value.back() == 'b')) {
					if (!stk.empty())
					while ((tk.value.back() == 'r' || tk.value.back() == 'u')
						|| ((prio[tk.value].first >= prio[stk.top().value].first)
						|| (prio[tk.value].first == prio[stk.top().value].first && prio[stk.top().value].second))) {
						std::cout << "PUSHEEN!!!!";
						std::cout << tk.value << "\n" << (tk.value.back() == 'r' || tk.value.back() == 'u') << (prio[tk.value].first >= prio[stk.top().value].first) << (prio[tk.value].first == prio[stk.top().value].first && prio[stk.top().value].second);
						res.push_back(stk.top());
						stk.pop();
						if (stk.empty())
							break;
					}
					stk.push(tk);
					
				}
			}
			while (!stk.empty()) {
				res.push_back(stk.top());
				stk.pop();
			}
#ifdef SYNTDBG
			for (auto& tk : res) {
				std::cout << tk.value << " ";
			}
			std::cout << "\n";
#endif
			return 0;
		}

		Node* ParseVar(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseVar\n\r";
#endif 
				if (Check(tk[pos], "MOD")) {
					if (Check(tk[pos], "MOD", "dim")) {
						while (Check(tk[pos], "MOD", "dim")) {
							pos++;
						}
						if (Check(tk[pos], "MOD", "array")) {
							pos++;
						}
						else {
							Err(tk[pos], orig, "array modifier");
						}
					}
					while (Check(tk[pos], "MOD")) {
						if (Check(tk[pos], "MOD", "dim"))
							Err(tk[pos], orig, "another modifier");
						pos++;
					}
				}
				if (Check(tk[pos], "TYPE") || Check(tk[pos], "ID")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "type name");
				}
				pos--;
				do {
					pos++;
					if (Check(tk[pos], "ID")) {
						pos++;
					}
					else {
						Err(tk[pos], orig, "variable name");
					}
					if (Check(tk[pos], "OPERATION", "=")) {
						pos++;
						ParseExpr(list, tk, pos, orig);
					}
				} while (Check(tk[pos], "GRAMMAR", ","));
				if (!Check(tk[pos], "ENDLINE")) {
					Err(tk[pos], orig, "end of line");
				}
			return 0;
		}

		Node* ParseStatement(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseStatement\n\r";
#endif 
			while (Check(tk[pos], "ENDLINE")) {
				pos++;
			}
			if (Check(tk[pos], "MOD") || Check(tk[pos], "TYPE") || (Check(tk[pos], "ID") && Check(tk[pos + 1], "ID"))) {
#ifdef SYNTDBG
				std::cout << "Sussik\n\r";
#endif 
				ParseVar(list, tk, pos, orig);
			}
			else if (Check(tk[pos], "KEYWORD", "delete")) {
					pos++;
					if (Check(tk[pos], "ID")) {
						pos++;
					}
					else {
						Err(tk[pos], orig, "variable name");
					}
					if (Check(tk[pos], "ENDLINE")) {
						pos++;
					}
					else {
						Err(tk[pos], orig, "end of line");
					}
			}
			else if (Check(tk[pos], "KEYWORD", "return")) {
				pos++;
				ParseExpr(list, tk, pos, orig);
				if (Check(tk[pos], "ENDLINE")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "end of line");
				}
			}
			else if (Check(tk[pos], "KEYWORD", "throw")) {
				pos++;
				if (Check(tk[pos], "STRING")) {
					pos++;
				}
				if (Check(tk[pos], "ENDLINE")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "end of line");
				}
			}
			else if (Check(tk[pos], "KEYWORD", "in")) {
				pos++;
				if (Check(tk[pos], "ID")) {
pos++;
				}
				if (Check(tk[pos], "ENDLINE")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "end of line");
				}
			}
			else if (Check(tk[pos], "KEYWORD", "out")) {
			pos++;
			if (Check(tk[pos], "KEYWORD", "outfirst")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "'za' keyword");
			}
			if (Check(tk[pos], "STRING")) {
				pos++;
			}
			else
				ParseExpr(list, tk, pos, orig);
			while (Check(tk[pos], "KEYWORD", "outelem")) {
				pos++;
				if (Check(tk[pos], "STRING")) {
					pos++;
				}
				else
					ParseExpr(list, tk, pos, orig);
			}
			if (Check(tk[pos], "ENDLINE")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "end of line");
			}
			}
			else if (Check(tk[pos], "KEYWORD", "if")) {
			pos++;
			ParseExpr(list, tk, pos, orig);
			if (Check(tk[pos], "KEYWORD", "then")) {
				pos++;
			}
			else
				Err(tk[pos], orig, "'then' keyword");
			ParseBlock(list, tk, pos, orig);
			if (Check(tk[pos], "KEYWORD", "else")) {
				pos++;
				ParseBlock(list, tk, pos, orig);
			}
			}
			else if (Check(tk[pos], "KEYWORD", "while")) {
			pos++;
			ParseExpr(list, tk, pos, orig);
			if (Check(tk[pos], "KEYWORD", "then")) {
				pos++;
				ParseBlock(list, tk, pos, orig);
			}
			else
				Err(tk[pos], orig, "'then' keyword");
			}
			else if (Check(tk[pos], "KEYWORD", "do")) {
			pos++;
			ParseExpr(list, tk, pos, orig);
			if (Check(tk[pos], "KEYWORD", "then")) {
				pos++;
				ParseBlock(list, tk, pos, orig);
			}
			else
				Err(tk[pos], orig, "'then' keyword");
			}
			else if (Check(tk[pos], "KEYWORD", "for")) {
			pos++;
			if (Check(tk[pos], "ID")) {
				pos++;
			}
			else
				Err(tk[pos], orig, "name");
			if (Check(tk[pos], "KEYWORD", "to")) {
				pos++;
			}
			else
				Err(tk[pos], orig, "'to' keyword");
			ParseExpr(list, tk, pos, orig);
			if (Check(tk[pos], "KEYWORD", "step")) {
				pos++;
				ParseExpr(list, tk, pos, orig);
			}
			ParseBlock(list, tk, pos, orig);
			}
			else if (Check(tk[pos], "KEYWORD", "foreach")) {
				pos++;
				if (Check(tk[pos], "ID")) {
				pos++;
				}
				else
					Err(tk[pos], orig, "name");
				if (Check(tk[pos], "KEYWORD", "step")) {
					pos++;
				}
				else
					Err(tk[pos], orig, "'step' keyword");
				ParseExpr(list, tk, pos, orig);
				ParseBlock(list, tk, pos, orig);
			}
			else {
				int prevpos = pos;
				ParseExpr(list, tk, pos, orig);
				if (pos == prevpos)
					Err(tk[pos], orig, "statement");
			}
			return 0;
		}

		Node* ParseBlock(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseBlock\n\r";
#endif 
			if (Check(tk[pos], "BLOCK", "Pognali")) {
				pos++;
				if (!Check(tk[pos], "ENDLINE")) {
					Err(tk[pos], orig, "end of line");
				}
				pos++;
				while (!Check(tk[pos], "BLOCK", "Shuher")) {
					if (Check(tk[pos], "ENDLINE")) {
						pos++;
						continue;
					}
					ParseStatement(list, tk, pos, orig);
				}
				pos++;
			}
			else {
				ParseStatement(list, tk, pos, orig);
			}
			return 0;
		}

		Node* ParseParams(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseParams\n\r";
#endif 
			pos--;
			do {
				pos++;
				if (Check(tk[pos], "MOD")) {
					if (Check(tk[pos], "MOD", "dim")) {
						while (Check(tk[pos], "MOD", "dim")) {
							pos++;
						}
						if (Check(tk[pos], "MOD", "array")) {
							pos++;
						}
						else {
							Err(tk[pos], orig, "array modifier");
						}
					}
					while (Check(tk[pos], "MOD")) {
						if (Check(tk[pos], "MOD", "dim"))
							Err(tk[pos], orig, "another modifier");
						pos++;
					}
				}
				if (Check(tk[pos], "TYPE") || Check(tk[pos], "ID")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "type name");
				}
				if (Check(tk[pos], "ID")) {
					pos++;
				}
				else {
					Err(tk[pos], orig, "variable name");
				}
				if (Check(tk[pos], "OPERATION", "=")) {
					pos++;
					if (Check(tk[pos], "VALUE")) {
						pos++;
					}
					else {
						Err(tk[pos], orig, "value");
					}
				}
			} 
			while (Check(tk[pos], "GRAMMAR", ","));
			
			return 0;
		}

		Node* ParseStruct(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseStruct\n\r";
#endif 
			++pos;
			if (Check(tk[pos], "ID")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "struct name");
			}
			if (Check(tk[pos], "BLOCK", "Pognali")) {
				pos++;
			}
			else {
				Err(tk[pos], orig, "block opening");
			}
			while (!Check(tk[pos], "BLOCK", "Shuher")) {
				if (Check(tk[pos], "ENDLINE"))
					pos++;
				else if (Check(tk[pos], "ID") || Check(tk[pos], "TYPE") || Check(tk[pos], "MOD"))
					ParseVar(list, tk, pos, orig);
				else if (Check(tk[pos], "BRACKETS", "(")) {
					++pos;
					ParseParams(list, tk, pos, orig);
					if (Check(tk[pos], "BRACKETS", ")")) {
						++pos;
					}
					else {
						Err(tk[pos], orig, "closing bracket");
					}
					ParseBlock(list, tk, pos, orig);
				}
				else 
					Err(tk[pos], orig, "variable or constructor initialisation");
			}
			pos++;
			return 0;
		}
		
		Node* ParseFunc(Node* list, const std::vector<Token>& tk, int& pos, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING ParseFunc\n\r";
#endif 
			++pos;
			if (Check(tk[pos], "ID")) {
				++pos;
			}
			else {
				Err(tk[pos], orig, "function name");
			}
			if (Check(tk[pos], "BRACKETS", "(")) {
				++pos;
			}
			else {
				Err(tk[pos], orig, "opening bracket");
			}
			if (Check(tk[pos], "BRACKETS", ")")) {
				++pos;
			}
			else {
				ParseParams(list, tk, pos, orig);
				if (Check(tk[pos], "BRACKETS", ")")) {
					++pos;
				}
				else {
					Err(tk[pos], orig, "closing bracket");
				}
			}
			ParseBlock(list, tk, pos, orig);
			return list;
		}

		Node* Analyze(const std::vector<Token>& tk, const std::string& orig) {
#ifdef SYNTDBG
			std::cout << "CALLING Analyze\n\r";
#endif 
			Node* list = new Node();
			int pos = 0;
			while (pos < tk.size() && !Check(tk[pos], "END")) {
				const Token& tt = tk[pos];
				if (Check(tt, "ID") || Check(tt, "TYPE") || Check(tt, "MOD")) {
					list->nodes.push_back(ParseVar(list, tk, pos, orig));
				} else if (Check(tt, "KEYWORD", "struct")) {
					list->nodes.push_back(ParseStruct(list, tk, pos, orig));
				} else if (Check(tt, "KEYWORD", "func")) {
					list->nodes.push_back(ParseFunc(list, tk, pos, orig));
				} else if (Check(tt, "ENDLINE")) {
					pos++;
				}
				else {
					Err(tt, orig, "function, struct or variable initialization");
				}
			}
			return list;
		}
	}
}