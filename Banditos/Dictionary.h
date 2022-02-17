#pragma once
#include <vector>
#include <string>

namespace bndts {
    namespace lex {
        auto keywords = std::vector<std::string>{
            "\\blapsha\\b",		    // func
            "\\bproehat\\b",		// for
            "\\bprognat\\b",		// for
            "\\bdo\\b",			    // for
            "\\bpo\\b",			    // for
            "\\bshag\\b",			// for
            "\\bpoka\\b",			// while
            "\\byumat\\b",		    // do
            "\\bpoyasnit\\b",		// output
            "\\bza\\b",		    	// output
            "\\bi\\b",			    // output
            "\\balyo\\b",			// input
            "\\batas\\b",			// throw
            "\\bhare\\b",			// break
            "\\bsachkovat\\b",	    // continue
            "\\blipa\\b",			// null
            "\\bstrela\\b",		    // if
            "\\bzabit\\b",		    // then
            "\\binache\\b",		    // else
            "\\bzhigan\\b",		    // true
            "\\bfraer\\b",		    // false
            "\\bzabit\\b",		    // then
            "\\bvalnut\\b",		    // delete
            "\\blaskat\\b",		    // return
            "\\bmalina\\b"		    // struct
        };

        auto mods = std::vector<std::string>{
            "\\b(chotkiy|chotkaya|chotkoe)\\b",		// const
            "\\b(shirokiy|shirokaya|shirokoe)\\b",	// array
            "\\boch\\b",			                // array dimension
            "\\b(blatnoy|blatnaya|blatnoe)\\b",		// unsigned
            "\\b(posilniy|posilnaya|posilnoe)\\b",	// pointer
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
            "||",
            "&&",
            "->",
            "=",
            "-",
            "/",
            "*",
            "%",
            "&",
            "|",
            "!",
            "+"
        };
        auto grammar = std::vector<std::string>{
            ":",
            ","
        };
    }
}