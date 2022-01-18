#pragma once
#include <string>

namespace bndts {
	namespace lex {
		class ITextProvider abstract {
		public:
			virtual std::string Get(const std::string&) = 0;
		};
	}
}

