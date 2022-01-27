#pragma once
#include <vector>
#include <string>

namespace bndts {
	namespace lex {
        auto keywords = std::vector<std::string>{
            // Modifiers
            "����\\S\\S\\s",		// const
            "�����\\S\\S\\s",		// array
            "��\\s",			// array dimension
            "�����\\S\\S\\s",		// unsigned
            "�������\\S\\S\\s",	// pointer

            "�����\\s",		// func
            "��������\\s",		// for
            "��\\s",			// for
            "���\\s",			// for
            "����\\s",			// while
            "�����\\s",		// do
            "��������\\s",		// output
            "��\\s",			// output
            "�\\s",			// output
            "��\\s",			// input
            "����\\s",			// throw
            "����\\s",			// break
            "���������\\s",	// continue
            "����\\s",			// null
            "������\\s",		// if
            "������\\s",		// then
            "�����\\s",		// true
            "�����\\s",		// false
            "������\\s",		// then
            "�������\\s",		// new
            "��������\\s",		// delete
            "�������\\s",		// return
            "������\\s"		// struct
        };
        auto systemTypes = std::vector<std::string>{
            "��������\\s",		// string
            "�����\\s",		// int
            "�������\\s",		// long
            "�������\\s",		// float
            "�������\\s",		// double
            "������\\s",		// char
            "������\\s",		// bool
        };
        auto operations = std::vector<std::string>{
            "�������\\s",
            "<=",
            ">=",
            ">",
            "<",
            "++",
            "--",
            "<<",
            ">>",
            "�� �����\\s", // init
            "��������\\s", // set
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