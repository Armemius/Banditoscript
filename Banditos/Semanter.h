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

        auto renames = std::stack<std::pair<std::string&, std::string>>();

        std::set<std::string> compOps{
            "==b",
            ">=b",
            "<=b",
            "<b",
            ">b",
            "!=b",
        };

        std::set<std::string> btwOps{
            "&b",
            "|b",
            "<<b",
            ">>b",
        };

        std::set<std::string> boolOps{
            "&&b",
            "||b",
        };

        std::set<std::string> setOps{
            "=b",
            "+=b",
            "-=b",
            "/=b",
            "*=b",
            "%=b",
            "~=b",
            "<<=b",
            ">>=b",
            "&=b",
            "|=b",
        };

        std::set<std::string> defaultTypes {
            "string",
            "int",
            "long",
            "float",
            "double",
            "char",
            "bool"
        };

        std::set<std::string> bitwiseTypes{
            "int",
            "long",
            "float",
            "double",
        };

        struct Var {
            synt::Type type = Type();
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

        Var CheckVar(synt::Node* node, int lvl);
        Type CheckExpr(synt::Node* node, synt::Node* prev, int lvl, std::set<std::string>& usedFuncs);

        Type GetOpType(const Type& op1, const Type& op2, const std::string& oper, Node* node) {
            if (op1 == op2 && setOps.find(oper) != setOps.end()) {
                if (op1.mods & CONST_MODIFIER)
                    Err(node->tk, "trying to change const value");
                return op1;
            }
            if (op1 == op2 && btwOps.find(oper) != btwOps.end()) {
                return op1;
            }
            if (boolOps.find(oper) != boolOps.end()) {
                if ((op1.mods & ARR_MODIFIER) != 0 || (op1.mods & PTR_MODIFIER) != 0)
                    Err(node->tk, "invalid modifiers");
                if (op1.value != "bool")
                    Err(node->tk, "variables are not boolean");
                return op1;
            }
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
            if (op1 == op2 && compOps.find(oper) != compOps.end()) {
                return Type{"bool", 0, 0};
            }
            if (op1 == op2 && !(op1.mods & (ARR_MODIFIER | PTR_MODIFIER)) 
                && defaultTypes.find(op1.value) != defaultTypes.end()
                && btwOps.find(oper) == btwOps.end()) {
                return op1;
            }
            Err(node->tk, "not able to use operation '" + oper + "' for these types(" 
                + op1.value + ":" + std::to_string(op1.mods) 
                + ":" + std::to_string(op1.params) + "/" 
                + op2.value + ":" + std::to_string(op2.mods)
                + ":" + std::to_string(op2.params) + + ")"); // Undefined operands
        }

        void ReloadVars(int lvl) {
            for (auto& it : vars) {
                if (it.second.size() == 0)
                    continue;
                while (it.second.top().lvl > lvl) {
                    it.second.pop();
                    if (it.second.empty())
                        break;
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

        void ParseStatement(synt::Node* node, int lvl, bool noReturn = false, bool isLoop = false, Type* tp = 0) {
            std::cout << "ABIBUS!!!\n\r";
            auto tmp = std::set<std::string>();
            if (node->value == "VAR") {
                CheckVar(node->nodes[0], lvl);
            } else if (node->value == "EXPR") {
                auto used = std::set<std::string>();
                CheckExpr(node->nodes[0], node, lvl, used);
            } else if (node->value == "CONTINUE") {
                if (!isLoop)
                    Err(node->tk, "continue is within loop");
            } else if (node->value == "BREAK") {
                if (!isLoop)
                    Err(node->tk, "break is within loop");
            } else if (node->value == "DELETE") {
                auto& id = node->nodes[0]->value;
                if (vars[id].empty())
                    Err(node->tk, "no variable matching this name");
                if ((vars[id].top().type.mods & PTR_MODIFIER) == 0)
                    Err(node->tk, "variable is not ptr");
            } else if (node->value == "RETURN") {
                if (noReturn)
                    Err(node->tk, "return is not allowed here");
                if (tp == 0 && node->nodes[0]->token == "VOID")
                    return;
                else {
                    auto type = CheckExpr(node->nodes[0], node, lvl, tmp);
                    if (type != *tp)
                        Err(node->tk, "return type mismatch");
                }
            } else if (node->value == "THROW") {
                if (node->nodes[0]->token == "VOID")
                    return;
                auto type = CheckExpr(node->nodes[0], node, lvl, tmp);
                if (type.mods != 0 || type.params != 0 || type.value != "string")
                    Err(node->tk, "throw message is not string");
            } else if (node->value == "INPUT") {
                if (vars[node->nodes[0]->value].empty())
                    Err(node->tk, "no variable matching this name");
            } else if (node->value == "OUTPUT") {
                for (auto& it : node->nodes) {
                    auto type = CheckExpr(it, node, lvl, tmp);
                    if (type.mods != 0 || type.params != 0 || defaultTypes.find(type.value) == defaultTypes.end())
                        Err(it->tk, "output element's type is not default");
                }
            } else if (node->value == "IF") {
                auto type = CheckExpr(node->nodes[0], node, lvl, tmp);
                if (type.mods != 0 || type.params != 0 || type.value != "bool")
                    Err(node->tk, "expression result is not boolean");
                for (auto& it : node->nodes[1]->nodes) {
                    ParseStatement(it, lvl + 1, noReturn, isLoop, tp);
                }
                ReloadVars(lvl);
                if (node->nodes.size() > 2) {
                    for (auto& it : node->nodes[2]->nodes) {
                        ParseStatement(it, lvl + 1, noReturn, isLoop, tp);
                    }
                    ReloadVars(lvl);
                }
            } else if (node->value == "WHILE") {
                auto type = CheckExpr(node->nodes[0], node, lvl, tmp);
                if (type.mods != 0 || type.params != 0 || type.value != "bool")
                    Err(node->tk, "expression result is not boolean");
                for (auto& it : node->nodes[1]->nodes) {
                    ParseStatement(it, lvl + 1, noReturn, true, tp);
                }
                ReloadVars(lvl);
            } else if (node->value == "DOWHILE") {
                auto type = CheckExpr(node->nodes[0], node, lvl, tmp);
                if (type.mods != 0 || type.params != 0 || type.value != "bool")
                    Err(node->tk, "expression result is not boolean");
                for (auto& it : node->nodes[1]->nodes) {
                    ParseStatement(it, lvl + 1, noReturn, true, tp);
                }
                ReloadVars(lvl);
            } else if (node->value == "FOR") {
                auto& id = node->nodes[0]->value;
                if (vars[id].empty())
                    Err(node->tk, "no variable matching this name");
                auto& type0 = vars[id].top().type;
                if (type0.mods != 0 || type0.params != 0 || bitwiseTypes.find(type0.value) == bitwiseTypes.end())
                    Err(node->tk, "variable type is not allowed");
                auto type1 = CheckExpr(node->nodes[1], node, lvl, tmp);
                if (type1.mods != 0 || type1.params != 0 || bitwiseTypes.find(type1.value) == bitwiseTypes.end())
                    Err(node->tk, "destination type is not allowed");
                if (type0 != type1)
                    Err(node->tk, "initial and destination type mismatch");
                if (node->nodes.size() == 4) {
                    auto type2 = CheckExpr(node->nodes[2], node, lvl, tmp);
                    if (type1 != type2) {
                        Err(node->tk, "destination and step type mismatch");
                    } 
                    for (auto& it : node->nodes[3]->nodes) {
                        ParseStatement(it, lvl + 1, noReturn, true, tp);
                    }
                    ReloadVars(lvl);
                }
                else {
                    for (auto& it : node->nodes[2]->nodes) {
                        ParseStatement(it, lvl + 1, noReturn, true, tp);
                    }
                    ReloadVars(lvl);
                }
            } else if (node->value == "FOREACH") {
                auto& id = node->nodes[0]->value;
                if (vars[id].empty())
                    Err(node->tk, "no variable matching this name");
                auto& type0 = vars[id].top().type;
                if ((type0.mods & (CONST_MODIFIER)) != 0)
                    Err(node->tk, "invalid variable modifiers");
                auto type1 = CheckExpr(node->nodes[1], node, lvl, tmp);
                if ((type1.mods & ARR_MODIFIER) == 0)
                    Err(node->tk, "variable is not array");
                if (type0.value != type1.value)
                    Err(node->tk, "can't match types");
                Type dtp = (type1.params == 0 ? Type{ type1.value, type1.mods & ~ARR_MODIFIER, type1.params } : Type{ type1.value, type1.mods, type1.params - 1});
                std::cout << (type1.params - type0.params != 1) << " " << (type1.params == 0 && (type0.mods & ARR_MODIFIER) == 0) << "\n\r";
                std::cout << type0.value << " " << type1.value << "\n\r";
                std::cout << type0.mods << " " << type1.mods << "\n\r";
                std::cout << type0.params << " " << type1.params << "\n\r";
                bool sc1 = type1.params - type0.params == 1;
                bool sc2 = type1.params == 0 && (type0.mods & ARR_MODIFIER) == 0;
                if (type0 != dtp)
                    Err(node->tk, "not able to match variables");
                for (auto& it : node->nodes[2]->nodes) {
                    ParseStatement(it, lvl + 1, noReturn, true, tp);
                }
                ReloadVars(lvl);
            }       
            else {
                throw std::exception("Nepon vtf");
            }
        }

        void CheckConstr(synt::Node* node, int lvl) {
            for (auto& it : node->nodes[0]->nodes) {
                if (!vars[it->value].empty())
                    if (vars[it->value].top().lvl == lvl)
                        Err(it->tk, "variable redefinition"); // Redefinition of var
                std::cout << "SUSABIBAS " << (!vars[it->value].empty() ? vars[it->value].top().lvl : -1) << " " << lvl << "\n\r";
                Var var = Var();
                var.id = it->value;
                var.type = it->nodes[0]->type;
                var.lvl = lvl;
                auto tmp = std::set<std::string>();
                if (it->nodes[1]->token != "DEFAULT")
                    Err(node->tk, "default values is not supported yet"); // Sussy
                std::cout << "BIBASUSUS " << it->value << " " << vars[it->value].size() << "\n\r";
                vars[it->value].push(var);
            }

            for (auto& it : node->nodes[1]->nodes) {
                ParseStatement(it, lvl, true, false);
            }
        }

        Type CheckCall(synt::Node* node, int lvl, std::set<std::string> &used) {
            std::string& name = node->value;
            for (auto& it : funcs[name]) {
                if (it.params->nodes.size() == node->nodes.size()) {
                    for (int j = 0; j < node->nodes.size(); j++) {
                        if (CheckExpr(node->nodes[j], node, lvl, used) != it.params->nodes[j]->nodes[0]->type)
                            goto continuer;
                    }
                    renames.push({name, it.trueName});
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
                    auto type = CheckExpr(node->nodes[0], node, lvl, used);
                    auto& oper = node->value;
                    if (oper == "&u") {
                        if (node->nodes[0]->token != "ID")
                            Err(node->tk, "operation only works with variables");
                        if ((type.mods & PTR_MODIFIER) == 0)
                            return Type{ type.value, type.mods | PTR_MODIFIER, type.params };
                        else
                            Err(node->tk, "operation only works with non-ptr variables");
                    } else if (oper == "*u") {
                        if (node->nodes[0]->token != "ID")
                            Err(node->tk, "operation only works with variables");
                        if ((type.mods & PTR_MODIFIER) != 0)
                            return Type{ type.value, type.mods & ~PTR_MODIFIER, type.params };
                        else
                            Err(node->tk, "operation only works with ptr variables");
                    }
                    else if (oper.back() != 'u') {
                        if (node->nodes[0]->token != "ID")
                            Err(node->tk, "this oneration only works with variables");
                        if (type.mods & CONST_MODIFIER)
                            Err(node->tk, "this oneration doesn't works with constants");
                        if (type.mods & PTR_MODIFIER)
                            Err(node->tk, "this oneration doesn't works with pointers");
                        if (type.mods & ARR_MODIFIER)
                            Err(node->tk, "this oneration doesn't works with arrays");
                        if (bitwiseTypes.find(type.value) == bitwiseTypes.end())
                            Err(node->tk, "this oneration doesn't works with this type");
                    }
                    else if (bitwiseTypes.find(type.value) == bitwiseTypes.end()) {
                        //std::cout << type.value << "\n\r";
                        Err(node->tk, "this oneration doesn't work with this type");
                    }
                    return type;
                // Binary
                } else {
                    if (node->value == "=b" && node->nodes[0]->token != "ID")
                        Err(node->tk, "trying to change expression"); // Undefined function
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
            var.lvl = lvl;
            auto tmp = std::set<std::string>();
            if (defaultTypes.find(var.type.value) == defaultTypes.end() && structs.find(var.type.value) == structs.end())
                Err(node->tk, "undefined type"); // Type mismatch
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
            for (auto& it : block->nodes) {
                ParseStatement(it, lvl, false, false, &funcs[name].back().type);
            }
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
                    CheckConstr(it, lvl + 1);
                    ReloadVars(lvl);

                    
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
            while (!renames.empty()) {
                auto& tmp = renames.top();
                tmp.first = tmp.second;
                renames.pop();
            }
            return;
        }
    }
}