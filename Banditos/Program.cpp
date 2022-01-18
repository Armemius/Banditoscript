#include <iostream>

#include "Lexer.h"
#include "FileTextProvider.h"
#include "RawTextProvider.h"

using namespace bndts;

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "rus");
	lex::Lexer lexa(new lex::FileTextProvider());
	
	for (auto sus : lexa.Get(R"(D:\Le projects\Sussy\Thulan\Translator\Source.txt)")) {
		std::cout << sus.filename << " " << sus.id << " " << sus.orig << " " << sus.value << " " << sus.pos << "\n\r";
	}
}
