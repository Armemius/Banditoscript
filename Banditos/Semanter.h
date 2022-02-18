#pragma once
#include "SyntacticTree.h"
#include <set>
using namespace bndts;

namespace bndts {
	namespace smc {

		std::set<std::string> defaultTypes {
			"string",
			"int",
			"float",
			"double",
			"char",
			"bool"
		};

		struct Var {
			std::string type = "NULL";
			int mods = 0;
			int dims = 0;
			int lvl = 0;
		};

		struct Func {
			std::string name = "NULL";
			std::string type = "NULL";
			synt::Node* params = NULL;
			synt::Node* block = NULL;
		};

		struct Struct {
			std::string name;
			std::vector<Func> constructs;
			std::map<std::string, Func> funcs;
			std::map<std::string, Var> vars;
		};

		std::map<std::string, Struct> structs;
		std::map<std::string, std::vector<Func>> funcs;
		std::map<std::string, std::stack<Var>> vars;

		std::string orig;

		void Err(const Token& tt, std::string exp = "") {
			int start = tt.pos - tt.col, fin = tt.pos - tt.col;
			while (orig[fin] != '\n' && fin < orig.size())
				fin++;
			std::string line = "\x1B[32m" + orig.substr(start, tt.col) + "\033[0m"
				+ "\x1B[31m" + orig.substr(tt.pos, tt.orig.size()) + "\033[0m"
				+ "\x1B[32m" + orig.substr(tt.pos + tt.orig.size(), (fin == tt.pos ? 0 : fin - tt.pos - tt.orig.size())) + "\033[0m";
			std::string arrow = "";
			for (int i = 0; i < tt.col; ++i)
				arrow += ' ';
			std::string err = "Unexpected token \"" + tt.value + ":" + tt.id + "\" at "
				+ tt.filename + ":" + std::to_string(tt.pos) + " (Col: " + std::to_string(tt.col)
				+ "; Line: " + std::to_string(tt.line) + ")" + (exp.size() ? "\n\rWhat has gone wrong: " + exp : "") + "\n\r"
				+ line + (fin == tt.pos ? "" : "\n\r");
			throw std::exception(err.c_str());
		}

		std::string CheckExpr(synt::Node* node, int lvl) {
			if (node->token == "EXPRESSION")
				return CheckExpr(node->nodes[0], lvl);
			if (node->token == "VALUE") {
				if (node->value == "true" || node->value == "false")
					return "bool";
				if (node->value.find('.') != std::string::npos)
					return "double";
				else
					return "int";
			}
			else if (node->token == "STRING") {
				if (node->value.size() == 3 && node->value.find('\\') == std::string::npos
					|| node->value.size() == 4 && node->value.find('\\') != std::string::npos) {
					return "char";
				}
				else
					return "string";
			}
			else if (node->token == "ID") {
				if (!vars[node->value].empty()) {
					return vars[node->value].top().type;
				}
				else
					Err(node->tk, "undefined variable"); // Undefined variable
			}
			else if (node->token == "CALL") {
				if (!funcs[node->value].empty()) {
					return funcs[node->value][0].type;
				}
				else
					Err(node->tk, "undefined function"); // Undefined variable
			}
			else if (node->token == "OPERATION") {
				// Unary
				if (node->nodes.size() == 1) {
					return CheckExpr(node->nodes[0], lvl);
				// Binary
				} else {
					std::string type1 = CheckExpr(node->nodes[0], lvl),
								type2 = CheckExpr(node->nodes[1], lvl);
					if (type1 != type2)
						Err(node->tk, "incompatible types"); // Incompatible types
					return type1;
				}
			}
			return "void";
		}

		void CheckVar(synt::Node* node, int lvl) {
			if (!vars[node->value].empty())
				if (vars[node->value].top().lvl == lvl)
					Err(node->tk, "variable redefinition"); // Redefinition of var
			Var var = Var();
			var.type = node->nodes[0]->value;
			if (node->nodes[1]->token != "DEFAULT" && var.type != CheckExpr(node->nodes[1], lvl))
				Err(node->tk, "type mismatch"); // Type mismatch
			vars[node->value].push(var);
		}

		void CheckFunc(synt::Node* node, int lvl) {

		}

		void CheckStruct(synt::Node* node, int lvl) {
			Struct str = Struct();
			str.name = node->nodes[0]->value;
			for (auto& it : node->nodes[1]->nodes) {

			}
			structs[str.name] = str;
		}

		void Check(synt::Node* node, int lvl = 0) {
			if (node->token == "PROGRAM") {
				for (auto& it : node->nodes) {
					if (it == NULL)
						continue;
					if (it->token == "STRUCT") {
						CheckStruct(it, lvl);
					} else if (it->token == "FUNCTION") {
						CheckStruct(it, lvl);
					} else if (it->token == "VARIABLE") {
						CheckVar(it, lvl);
					}
					else
						throw; //SUSS
				}
			}
			return;
		}
	}
}