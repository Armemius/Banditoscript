#pragma once
#include <fstream>
#include <sstream>
#include "ITextProvider.h"

namespace bndts {
	namespace lex {
		class FileTextProvider : public ITextProvider {
			std::string Get(const std::string& text) override;
		};

		std::string FileTextProvider::Get(const std::string& path) {
			std::ifstream file(path);
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			return buffer.str();
		}
	}
}