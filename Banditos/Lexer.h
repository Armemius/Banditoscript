#pragma once
#include <vector>
#include <regex>
#include "Utility.h"
#include "Token.h"
#include "Dictionary.h"
#include "Token.h"
#include "FileTextProvider.h"

namespace bndts {
	namespace lex {
		class Lexer {
		private:
			ITextProvider* _textProvider;
		public:
			Lexer(ITextProvider *textProvider);
			~Lexer();
			std::vector<Token> Parse(const std::string& raw, const std::string& file);
			std::vector<Token> Get(const std::string& raw, const std::string& name);
            void Normalize(std::vector<Token>* tokens);
		};

		Lexer::Lexer(ITextProvider* textProvider) :
			_textProvider(textProvider) {}

		Lexer::~Lexer() {
			delete _textProvider;
		}

        std::vector<Token> Lexer::Parse(const std::string& raw, const std::string& file = "NONE") {
            setlocale(LC_ALL, "rus");
            std::string filename = file;
            std::string txt = _textProvider->Get(raw);
            std::cout << txt;
            auto Join = [](const std::vector<std::string>& dict) {
                std::string joined = "";
                for (int i = 0; i < dict.size(); ++i) {
                    joined += "(" + dict[i] + ")" + (i == dict.size() - 1 ? "" : "|");
                }
                return joined;
            };
            // Join with shielding several strings to use as regex
            auto SJoin = [](const std::vector<std::string>& dict) {
                auto Shield = [](const std::string& line) {
                    std::string shielded = "";
                    for (auto& it : line) {
                        shielded = shielded + "\\" + it;
                    }
                    return shielded;
                };
                std::string joined = "";
                for (int i = 0; i < dict.size(); ++i) {
                    joined += "(" + Shield(dict[i]) + ")" + (i == dict.size() - 1 ? "" : "|");
                }
                return joined;
            };
            auto expr = std::vector<std::pair<std::string, std::string>>{
                {"VALUE", R"(\d+\.\d*|\d+)"},
                {"BRACKETS", R"(\(|\))"},
                {"BLOCK", R"(\bPognali\b|\bShuher\b)"},
                {"COMMENT", R"(\/\/.*|\/\*[\s\S]*?\*\/)"},
                {"STRING", R"(\'(\\.|[^'\\])*\'|\"(\\.|[^"\\])*\")"},
                {"KEYWORD", Join(keywords)},
                {"MOD", Join(mods)},
                {"TYPE", Join(systemTypes)},
                {"OPERATION", SJoin(operations)},
                {"GRAMMAR", SJoin(grammar)},
                {"SPACE", R"([\ \t]+)"},
                {"ENDLINE", R"(\n|\n\r)"},
                {"ID", R"([_a-zA-Zà-ÿÀ-ß][_a-zA-Zà-ÿÀ-ß0-9]*)"}
            };
            std::string joined = "";
            for (int i = 0; i < expr.size(); ++i) {
                joined += "(" + expr[i].second + ")" + (i == expr.size() - 1 ? "" : "|");
            }
            std::regex joinedRegex(joined);
            auto words_begin =
                std::sregex_iterator(txt.begin(), txt.end(), joinedRegex);
            auto words_end = std::sregex_iterator();

            std::vector<Token> tokens = std::vector<Token>();
            auto getCol = [](const std::string& str, int pos) {
                int ctr = 0;
                while (pos > 0 && str[pos - 1] != '\n' && str[pos - 1] != '\r') {
                    --pos;
                    ++ctr;
                }
                return ctr;
            };
            auto getStr = [](const std::string& str, int pos) {
                int ctr = 0;
                while (pos >= 0) {
                    if (str[pos] == '\n')
                        ++ctr;
                    --pos;
                }
                return ctr;
            };
            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;
                for (auto& it : expr) {
                    if (std::regex_match(match.str(), std::regex(it.second))) {
                        tokens.push_back(Token{filename, it.first, match.str(), match.str(), match.position(), getCol(txt, match.position()), getStr(txt, match.position())});
                        break;
                    }
                }
            }
            if (std::regex_replace(txt, joinedRegex, "").size() > 0) {
                std::cout << std::regex_replace(txt, joinedRegex, "");
                throw std::exception("Invalid tokens: unable to parse");
            }
            else
                return tokens;
        }

        std::vector<Token> Lexer::Get(const std::string& raw, const std::string& name) {
            auto tokens = Parse(raw, name);
            Normalize(&tokens);
            return tokens;
        }

        void Lexer::Normalize(std::vector<Token>* tokens) {
            auto replacesKeywords = std::vector<std::pair<std::string, std::string>>{
                {"(chotkiy|chotkaya|chotkoe)", "const"},
                {"(shirokiy|shirokaya|shirokoe)", "array"},
                {"och", "dim"},
                {"(blatnoy|blatnaya|blatnoe)", "unsigned"},
                {"(posilniy|posilnaya|posilnoe)", "ptr"},

                {"lapsha", "func"},
                {"proehat", "foreach"},
                {"prognat", "for"},
                {"do", "to"},
                {"po", "step"},
                {"poka", "while"},
                {"yumat", "do"},
                {"poyasnit", "out"},
                {"za", "outfirst"},
                {"i", "outelem"},
                {"alyo", "in"},
                {"atas", "throw"},
                {"hare", "break"},
                {"sachkovat", "continue"},
                {"lipa", "null"},
                {"strela", "if"},
                {"zabit", "then"},
                {"inache", "else"},
                {"zhigan", "true"},
                {"fraer", "false"},
                {"hapnut", "new"},
                {"valnut", "delete"},
                {"laskat", "return"},
                {"malina", "struct"}
            };
            auto replacesTypes = std::vector<std::pair<std::string, std::string>>{
                {"pogonyalo", "string"},
                {"shifer", "int"},
                {"colonna", "long"},
                {"plavnik", "float"},
                {"dvoynik", "double"},
                {"gudron", "char"},
                {"chubrik", "bool"},
            };
            auto replacesOperations = std::vector<std::pair<std::string, std::string>>{
                {"vnature", "equal"},
                {"po masti", "init"},
                {"povesit", "="},
                {"pobolee", ">"},
                {"pomenee", "<"},
                {"polevee", "<<"},
                {"popravee", ">>"},
            };
            for (int it = 0; it < tokens->size(); ++it) {
                auto& elem = tokens->at(it);
                std::vector<std::pair<std::string, std::string>>* replaces = nullptr;
                if (tokens->at(it).id == "OPERATION")
                    replaces = &replacesOperations;
                else if (tokens->at(it).id == "TYPE")
                    replaces = &replacesTypes;
                else if (tokens->at(it).id == "KEYWORD" || tokens->at(it).id == "MOD")
                    replaces = &replacesKeywords;
                else if (replaces == nullptr)
                    continue;
                for (auto& rep : *replaces) {
                    if (std::regex_match(elem.value, std::regex(rep.first))) {
                        //std::cout << elem.value << " <- " << rep.second << "\n";
                        elem.value = rep.second;
                        break;
                    }

                }
            }
        }
	}
}