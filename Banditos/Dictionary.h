#pragma once
#include <vector>
#include <string>

namespace bndts {
	namespace lex {
        auto keywords = std::vector<std::string>{
            // Modifiers
            "чётк\\S\\S\\s",		// const
            "широк\\S\\S\\s",		// array
            "оч\\s",			// array dimension
            "блатн\\S\\S\\s",		// unsigned
            "посыльн\\S\\S\\s",	// pointer

            "лапша\\s",		// func
            "прогнать\\s",		// for
            "до\\s",			// for
            "шаг\\s",			// for
            "пока\\s",			// while
            "юмать\\s",		// do
            "пояснить\\s",		// output
            "за\\s",			// output
            "и\\s",			// output
            "алё\\s",			// input
            "атас\\s",			// throw
            "харэ\\s",			// break
            "сачковать\\s",	// continue
            "липа\\s",			// null
            "стрела\\s",		// if
            "забить\\s",		// then
            "жиган\\s",		// true
            "фраер\\s",		// false
            "забить\\s",		// then
            "хапнуть\\s",		// new
            "вальнуть\\s",		// delete
            "ласкать\\s",		// return
            "малина\\s"		// struct
        };
        auto systemTypes = std::vector<std::string>{
            "погоняло\\s",		// string
            "шифер\\s",		// int
            "колонна\\s",		// long
            "плавник\\s",		// float
            "двойник\\s",		// double
            "гудрон\\s",		// char
            "чубрик\\s",		// bool
        };
        auto operations = std::vector<std::string>{
            "внатуре\\s",
            "<=",
            ">=",
            ">",
            "<",
            "++",
            "--",
            "<<",
            ">>",
            "по масти\\s", // init
            "повесить\\s", // set
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