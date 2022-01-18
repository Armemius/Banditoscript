#pragma once
#include "ITextProvider.h"

namespace bndts {
	namespace lex {
		class RawTextProvider : public ITextProvider {
			std::string Get(const std::string& text) override;
		};

		std::string RawTextProvider::Get(const std::string& text) {
			return text;
		}
	}
}