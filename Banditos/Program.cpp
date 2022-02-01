#include <iostream>

#include "Lexer.h"
#include "FileTextProvider.h"
#include "RawTextProvider.h"
#include "SyntacticTree.h"

using namespace bndts;

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "rus");
	try {
		lex::Lexer lexa(new lex::FileTextProvider());
		auto tmp = lexa.Get(R"(D:\Le projects\Sussy\Thulan\Translator\Source.txt)");
		std::cout << std::endl;
		std::vector<Token> clear = std::vector<Token>();
		for (auto& token : tmp) {
			if (token.id != "SPACE" && token.id != "COMMENT") {
				std::cout << token.id << " " << token.value << "\n\r";
				clear.push_back(token);
			}
		}
		auto SyntTree = synt::Analyze(clear);
	}
	catch (std::exception ex) {
		std::cout << "Critical error! Unable to continue\n\r" << ex.what();
	}
}
