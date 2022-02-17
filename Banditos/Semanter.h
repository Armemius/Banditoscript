#pragma once
#include "SyntacticTree.h"

using namespace bndts;

namespace bndts {
	namespace smc {
		struct Var {
			std::string type = "NULL";
			int mods = 0;
			int dims = 0;
			int level = 0;
		};

		struct Func {
			std::string name = "NULL";
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
		std::map<std::string, Func> funcs;
		std::map<std::string, std::stack<Var>> vars;

		std::string CheckExpr(synt::Node* node, int lvl) {
			return "VOID";
		}

		void CheckVar(synt::Node* node, int lvl) {
			if (!vars[node->value].empty())
				if (vars[node->value].top().level == lvl)
					throw; // Redefinition of var
			Var var = Var();
			var.type = node->nodes[0]->value;
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