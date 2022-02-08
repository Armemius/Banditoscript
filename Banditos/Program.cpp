#include <iostream>

#include "Lexer.h"
#include "FileTextProvider.h"
#include "RawTextProvider.h"
#include "SyntacticTree.h"

using namespace bndts;

int main(int argc, char* argv[]) {
	/*
	printf("\x1B[31mTexting\033[0m\t\t"); // Red
	printf("\x1B[32mTexting\033[0m\t\t"); // Green
	printf("\x1B[33mTexting\033[0m\t\t"); // Yellow
	*/
	setlocale(LC_ALL, "rus");
	try {
		auto path = R"(D:\Le projects\Sussy\Thulan\Translator\Source.txt)";
		auto txt = lex::FileTextProvider();
		lex::Lexer lexa(new lex::RawTextProvider());
		auto raw = txt.Get(path);
		auto tmp = lexa.Get(raw, path);
		std::vector<Token> clear = std::vector<Token>();
		for (auto& token : tmp) {
			if (token.id != "SPACE" && token.id != "COMMENT") {
				std::cout << token.id << " " << token.value << "\n\r";
				clear.push_back(token);
			}
		}
		clear.push_back(Token{path, "END", "END", "END"});
		auto SyntTree = synt::Analyze(clear, raw);
	}
	catch (std::exception ex) {
		std::cout << "Critical error! Unable to continue\n\r" << ex.what();
	}
}
