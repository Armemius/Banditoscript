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
			std::vector<Token> Get(const std::string& raw);
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
                {"BLOCK", R"(Погнали\\s|Шухер\\s)"},
                {"COMMENT", R"(\/\/.*|\/\*[\s\S]*?\*\/)"},
                {"STRING", R"(\'(\\.|[^'\\])*\'|\"(\\.|[^"\\])*\")"},
                {"KEYWORD", Join(keywords)},
                {"TYPE", Join(systemTypes)},
                {"OPERATION", SJoin(operations)},
                {"GRAMMAR", SJoin(grammar)},
                {"SPACE", R"([\ \t]+)"},
                {"ENDLINE", R"(\n|\n\r)"},
                {"ID", R"([_a-zA-Zа-яА-Я][_a-zA-Zа-яА-Я0-9]*)"}
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
            if (std::regex_replace(txt, joinedRegex, "").size() > 0)
                throw std::exception("Invalid tokens: unable to parse");
            else
                return tokens;
        }

        std::vector<Token> Lexer::Get(const std::string& raw) {
            auto tokens = Parse(raw);
            Normalize(&tokens);
            return tokens;
        }

        void Lexer::Normalize(std::vector<Token>* tokens) {
            auto replacesKeywords = std::vector<std::pair<std::string, std::string>>{
                {"чётк\\S\\S\\s", "const"},
                {"широк\\S\\S\\s", "array"},
                {"оч\\s", "dim"},
                {"блатн\\S\\S\\s", "unsigned"},
                {"посыльн\\S\\S\\s", "ptr"},

                {"лапша\\s", "func"},
                {"прогнать\\s", "for"},
                {"до\\s", "to"},
                {"шаг\\s", "step"},
                {"пока\\s", "while"},
                {"юмать\\s", "do"},
                {"пояснить\\s", "out"},
                {"за\\s", "outfirst"},
                {"и\\s", "outelem"},
                {"алё\\s", "in"},
                {"атас\\s", "throw"},
                {"харэ\\s", "break"},
                {"сачковать\\s", "continue"},
                {"липа\\s", "null"},
                {"стрела\\s", "if"},
                {"забить\\s", "then"},
                {"жиган\\s", "true"},
                {"фраер\\s", "false"},
                {"хапнуть\\s", "new"},
                {"вальнуть\\s", "delete"},
                {"ласкать\\s", "return"},
                {"малина\\s", "struct"}
            };
            auto replacesTypes = std::vector<std::pair<std::string, std::string>>{
                {"погоняло\\s", "string"},
                {"шифер\\s", "int"},
                {"колонна\\s", "long"},
                {"плавник\\s", "float"},
                {"двойник\\s", "double"},
                {"гудрон\\s", "char"},
                {"чубрик\\s", "bool"},
            };
            auto replacesOperations = std::vector<std::pair<std::string, std::string>>{
                {"внатуре\\s", "equal"},
                {"по масти\\s", "init"},
                {"повесить\\s", "="},
                {"поболее\\s", ">"},
                {"поменее\\s", "<"},
                {"полевее\\s", "<<"},
                {"поправее\\s", ">>"},
            };
            for (int it = 0; it < tokens->size(); ++it) {
                auto& elem = tokens->at(it);
                std::vector<std::pair<std::string, std::string>>* replaces = nullptr;
                if (tokens->at(it).id == "OPERATION")
                    replaces = &replacesOperations;
                else if (tokens->at(it).id == "TYPE")
                    replaces = &replacesTypes;
                else if (tokens->at(it).id == "KEYWORD")
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