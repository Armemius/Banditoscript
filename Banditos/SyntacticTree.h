#pragma once
#include <list>
#include <vector>
#include "Token.h"

namespace bndts {
	namespace synt {
		struct Node {
			std::list<Node*> nodes;
			std::string token = 0;
		};

		Node* ParseStruct(Node* list, const std::vector<Token>& tokens, int* pos) {
			return 0;
		}

		Node* ParseSegment(Node* list, const std::vector<Token>& tokens, int* pos) {
			return 0;
		}

		Node* Analyze(const std::vector<Token>& tokens) {
			Node* list = 0;
			return list;
		}
	}
}