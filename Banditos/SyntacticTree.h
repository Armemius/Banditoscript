#pragma once
#include <list>
#include <vector>
#include "Token.h"

namespace bndts {
	namespace synt {
		struct List {
			std::list<List*> nodes;
			std::string token = 0;
		};

		List* ParseStruct(List* list, const std::vector<Token>& tokens, int* pos) {

		}

		List* ParseSegment(List* list, const std::vector<Token>& tokens, int* pos) {
			if (tokens[*pos].id == "KEYWORD" && tokens[*pos].value == "struct")
				
		}

		List* Analyze(const std::vector<Token>& tokens) {
			List* list = 0;
			
		}
	}
}