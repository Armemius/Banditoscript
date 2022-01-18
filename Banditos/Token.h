#pragma once
#include <string>

struct Token {
	std::string filename = "";
	std::string id = "";
	std::string orig = "";
	std::string value = "";
	long long pos = 0, col = 0, line = 0;
};