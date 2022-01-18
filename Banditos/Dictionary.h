#pragma once
#include <vector>
#include <string>

namespace bndts {
	namespace lex {
        auto keywords = std::vector<std::string>{
            // Modifiers
            "чётк\\S\\S",		// const
            "широк\\S\\S",		// array
            "оч",			// array dimension
            "блатн\\S\\S",		// unsigned
            "посыльн\\S\\S",	// pointer

            "лапша",		// func
            "прогнать",		// for
            "до",			// for
            "шаг",			// for
            "пока",			// while
            "юмать",		// do
            "пояснить",		// output
            "за",			// output
            "и",			// output
            "алё",			// input
            "атас",			// throw
            "харэ",			// break
            "сачковать",	// continue
            "липа",			// null
            "стрела",		// if
            "забить",		// then
            "жиган",		// true
            "фраер",		// false
            "забить",		// then
            "хапнуть",		// new
            "вальнуть",		// delete
            "ласкать",		// return
            "малина"		// struct
        };
        auto systemTypes = std::vector<std::string>{
            "погоняло",		// string
            "шифер",		// int
            "колонна",		// long
            "плавник",		// float
            "двойник",		// double
            "гудрон",		// char
            "чубрик",		// bool
        };
        auto operations = std::vector<std::string>{
            "внатуре",
            "<=",
            ">=",
            ">",
            "<",
            "++",
            "--",
            "<<",
            ">>",
            "по масти", // init
            "повесить", // set
            "-",
            "/",
            "*",
            "%",
            "&",
            "!",
            "+",
            "+=",
            "-=",
            "*=",
            "/="
        };
        auto grammar = std::vector<std::string>{
            ":",
            ","
        };
	}
}