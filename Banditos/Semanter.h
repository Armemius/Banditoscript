#pragma once
#include "SyntacticTree.h"
#include <set>
#include <unordered_map>
using namespace bndts;
using bndts::synt::Type;
using bndts::synt::Node;

namespace bndts {
    namespace smc {
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

        std::set<std::string> defaultTypes {
            "string",
            "int",
            "long",
            "float",
            "double",
            "char",
            "bool"
        };

        struct Var {
            synt::Type type;
            int lvl = 0;
            std::string id = "None";
        };

        struct Func {
            std::string name = "NULL";
            std::string trueName = "NULL";
            synt::Node* params = NULL;
            synt::Node* block = NULL;
            Type type;
        };

        struct Struct {
            std::string name = "@NONE";
            std::vector<Func> constructs;
            std::unordered_map<std::string, Func> funcs;
            std::unordered_map<std::string, Var> vars;
        };

        std::unordered_map<std::string, Struct> structs;
        std::unordered_map<std::string, std::vector<Func>> funcs;
        std::unordered_map<std::string, std::stack<Var>> vars;

        Type CheckExpr(synt::Node* node, synt::Node* prev, int lvl, std::set<std::string>& usedFuncs);

        Type GetOpType(const Type& op1, const Type& op2, const std::string& oper, Node* node) {
            if (oper == "->b") {
                std::cout << "SUS!!\n";
                if (op1.mods & ARR_MODIFIER) {
                    std::cout << "INDEXATOR!!\n";
                    if (op2.value == "int") {
                        std::cout << op1.mods << " " << op1.params << "\n\r";
                        if (op1.params == 0)
                            return Type{ op1.value, op1.mods & ~ARR_MODIFIER, 0};
                        else
                            return Type{ op1.value, op1.mods, op1.params - 1};
                    }
                    else 
                        Err(node->tk, "index is not INT");
                }
                else
                    if (defaultTypes.find(op1.value) != defaultTypes.end()) {
                        std::cout << "STRUCTER!!\n";
                        std::cout << op2.value << "\n";
                    }
                else 
                    Err(node->tk, "incorrect use of '->' operator");
            }
            if (op1 == op2 && !(op1.mods & (ARR_MODIFIER | PTR_MODIFIER)) && defaultTypes.find(op1.value) != defaultTypes.end()) {
                return op1;
            }
            Err(node->tk, "not able to use operation for these types(" 
                + op1.value + ":" + std::to_string(op1.mods) 
                + ":" + std::to_string(op1.params) + "/" 
                + op2.value + ":" + std::to_string(op2.mods)
                + ":" + std::to_string(op2.params) + + ")"); // Undefined operands
        }

        void ReloadVars(int lvl) {
            for (auto& it : vars) {
                while (it.second.top().lvl > lvl) {
                    it.second.pop();
                }
            }
        }

        Type CheckFuncType(synt::Node* node, int lvl, Type& type, bool& isFirst, const std::string& name) {
            bool isVoid = true;
            if (node->token == "STATEMENT" && node->value == "RETURN") {
                if (isFirst) {
                    isFirst = 0;
                    auto used = std::set<std::string>();
                    try {
                        type = CheckExpr(node->nodes[0], node, lvl, used);
                    }
                    catch (std::exception ex) {
                        if (used.find(name) != used.end())
                            std::rethrow_exception(std::current_exception());
                    }
                    
                } else {
                    auto used = std::set<std::string>();
                    if (type != CheckExpr(node->nodes[0], node, lvl, used)) {
                        Err(node->tk, "undefined return type"); // Undefined return type
                    }
                }
            }
            for (auto& it : node->nodes) {
                if (it != 0)
                    CheckFuncType(it, lvl, type, isFirst, name);
            }
            return Type();
        }

        void CheckFuncBlock(synt::Node* node, int lvl) {
            
        }

        Type CheckCall(synt::Node* node, int lvl, std::set<std::string> &used) {
            std::string& name = node->value;
            for (auto& it : funcs[name]) {
                if (it.params->nodes.size() == node->nodes.size()) {
                    for (int j = 0; j < node->nodes.size(); j++) {
                        if (CheckExpr(node->nodes[j], node, lvl, used) != it.params->nodes[j]->nodes[0]->type)
                            goto continuer;
                    }
                    name = it.trueName;
                    return it.type;
                }
            continuer:
                continue;
            }
            for (auto& it : structs[name].constructs) {
                if (it.params->nodes.size() == node->nodes.size()) {
                    for (int j = 0; j < node->nodes.size(); j++) {
                        if (CheckExpr(node->nodes[j], node, lvl, used) != it.params->nodes[j]->nodes[0]->type)
                            goto scontinuer;
                    }
                    name = it.trueName;
                    return it.type;
                }
            scontinuer:
                continue;
            }
            Err(node->tk, "not able to match function's parameters with call");
        }

        Type CheckExpr(synt::Node* node, synt::Node* prev, int lvl, std::set<std::string>& used) {
            if (node->token == "EXPRESSION")
                return CheckExpr(node->nodes[0], node, lvl, used);
            if (node->token == "VALUE") {
                if (node->value == "true" || node->value == "false")
                    return Type{ "bool" };
                if (node->value.find('.') != std::string::npos)
                    return Type{ "double" };
                else
                    return Type{ "int" };
            }
            else if (node->token == "STRING") {
                if (node->value.size() == 3 && node->value.find('\\') == std::string::npos
                    || node->value.size() == 4 && node->value.find('\\') != std::string::npos) {
                    return Type{ "char" };
                }
                else
                    return Type{ "string" };
            }
            else if (node->token == "ID") {
                if (!vars[node->value].empty()) {
                    return vars[node->value].top().type;
                }
                else if (prev->token == "OPERATION" && prev->value == "->b") {
                    return Type{ node->value, 0, 0};
                }
                else {
                    std::cout << node->prev->token << " " << node->prev->value << "\n\r";
                    Err(node->tk, "undefined variable"); // Undefined variable
                }
            }
            else if (node->token == "CALL") {
                if (!funcs[node->value].empty()) {
                    auto type = CheckCall(node, lvl, used);
                    if (type.value == "void")
                        Err(node->tk, "trying to get value from void-function");
                    return type;
                }
                else if (structs[node->value].name != "@NONE") {
                    return CheckCall(node, lvl, used);
                }
                else
                    Err(node->tk, "undefined call"); // Undefined function
            }
            else if (node->token == "OPERATION") {
                // Unary
                if (node->nodes.size() == 1) {
                    return CheckExpr(node->nodes[0], node, lvl, used);
                // Binary
                } else {
                    Type type1 = CheckExpr(node->nodes[0], node, lvl, used),
                         type2 = CheckExpr(node->nodes[1], node, lvl, used);
                    return GetOpType(type1, type2, node->value, node);
                }
            }
            return Type{"void"};
        }

        Var CheckVar(synt::Node* node, int lvl) {
            if (!vars[node->value].empty())
                if (vars[node->value].top().lvl == lvl)
                    Err(node->tk, "variable redefinition"); // Redefinition of var
            Var var = Var();
            var.id = node->value;
            var.type = node->nodes[0]->type;
            auto tmp = std::set<std::string>();
            if (node->nodes[1]->token != "DEFAULT" && var.type != CheckExpr(node->nodes[1], node, lvl, tmp))
                Err(node->tk, "type mismatch"); // Type mismatch
            vars[node->value].push(var);
            return var;
        }

        void CheckFunc(synt::Node* node, int lvl) {
            std::string name = node->nodes[0]->value;
            Node* params = node->nodes[1];
            Node* block = node->nodes[2];
            for (auto& jt : funcs[name]) {
                if (jt.params->nodes.size() == params->nodes.size()) {
                    for (int i = 0; i < params->nodes.size(); ++i) {
                        if (jt.params->nodes[i]->nodes[0]->type != params->nodes[i]->nodes[0]->type)
                            goto skipper;
                    }
                    Err(node->nodes[0]->tk, "function redefinition");
                }
            skipper:
                continue;
            }

            Type type = Type{"void"};
            bool tmp = true;
            funcs[name].push_back(Func{name, name + "@" + std::to_string(funcs[name].size()), params, block, type});
            CheckFuncType(block, lvl, funcs[name].back().type, tmp, name);
            std::cout << funcs[name].back().type.value << " " << funcs[name].back().trueName << "\n";
            CheckFuncBlock(block, lvl);
        }

        void CheckStruct(synt::Node* node, int lvl) {
            Struct str = Struct();
            str.name = node->nodes[0]->value;
            auto& stg = structs[str.name];
            for (auto& it : node->nodes[1]->nodes) {
                if (it == NULL)
                    continue;
                std::cout << it->token << "\n\r";
                if (it->token == "VARIABLE") {
                    auto var = CheckVar(it, lvl);
                    if (stg.vars.find(var.id) == stg.vars.end()) {
                        stg.vars.insert({ var.id, var });
                    } else
                        Err(it->tk, "struct variable redefinition");
                } else if (it->token == "CONSTRUCTOR") {
                    std::cout << "CONSTRUCTOR!!\n";
                    std::string name = str.name;
                    Node* params = it->nodes[0];
                    Node* block = it->nodes[1];

                    std::cout << name << " " << str.constructs.size() << "\n\r";

                    for (auto& jt : str.constructs) {
                        if (jt.params->nodes.size() == params->nodes.size()) {
                            for (int i = 0; i < params->nodes.size(); ++i) {
                                if (jt.params->nodes[i]->nodes[0]->type != params->nodes[i]->nodes[0]->type)
                                    goto skipper;
                            }
                            Err(node->nodes[0]->tk, "constructor redefinition");
                        }
                    skipper:
                        continue;
                    }

                    str.constructs.push_back(Func{ name, name + "@" + std::to_string(str.constructs.size()), params, block, name });
                }
            }
            if (structs[str.name].name == "@NONE")
                structs[str.name] = str;
            else
                Err(node->tk, "struct redefinition");
        }

        void Check(synt::Node* node, int lvl = 0) {
            if (node->token == "PROGRAM") {
                for (auto& it : node->nodes) {
                    if (it == NULL)
                        continue;
                    if (it->token == "STRUCT") {
                        CheckStruct(it, lvl);
                        ReloadVars(lvl);
                    }
                }
                for (auto& it : node->nodes) {
                    if (it == NULL || it->token == "STRUCT")
                        continue;
                    else if (it->token == "FUNCTION") {
                        CheckFunc(it, lvl);
                        ReloadVars(lvl);
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