#pragma once
#include <vector>
#include <string>

namespace bndts {
	namespace lex {
        auto keywords = std::vector<std::string>{
            // Modifiers
            "\\b(chotkiy|chotkaya|chotkoe)\\b",		// const
            "\\b(shirokiy|shirokaya|shirokoe)\\b",		// array
            "\\boch\\b",			// array dimension
            "\\b(blatnoy|blatnaya|blatnoe)\\b",		// unsigned
            "\\b(posilniy|posilnaya|posilnoe)\\b",	// pointer

            "\\blapsha\\b",		// func
            "\\bprognat\\b",		// for
            "\\bdo\\b",			// for
            "\\bshag\\b",			// for
            "\\bpoka\\b",			// while
            "\\byumat\\b",		// do
            "\\bpoyasnit\\b",		// output
            "\\bza\\b",			// output
            "\\bi\\b",			// output
            "\\balyo\\b",			// input
            "\\batas\\b",			// throw
            "\\bhare\\b",			// break
            "\\bsachkovat\\b",	// continue
            "\\blipa\\b",			// null
            "\\bstrela\\b",		// if
            "\\bzabit\\b",		// then
            "\\bzhigan\\b",		// true
            "\\bfraer\\b",		// false
            "\\bzabit\\b",		// then
            "\\bhapnut\\b",		// new
            "\\bvalnut\\b",		// delete
            "\\blaskat\\b",		// return
            "\\bmalina\\b"		// struct
        };
        auto systemTypes = std::vector<std::string>{
            "\\bpogonyalo\\b",		// string
            "\\bshifer\\b",		// int
            "\\bcolonna\\b",		// long
            "\\bplavnik\\b",		// float
            "\\bdvoynik\\b",		// double
            "\\bgudron\\b",		// char
            "\\bchubrik\\b",		// bool
        };
        auto operations = std::vector<std::string> {
            "==",
            "<=",
            ">=",
            ">",
            "<",
            "++",
            "--",
            "<<",
            ">>",
            "+=",
            "-=",
            "*=",
            "/=",
            "=",
            "-",
            "/",
            "*",
            "%",
            "&",
            "!",
            "+"
        };
        auto grammar = std::vector<std::string>{
            ":",
            ","
        };
	}
}