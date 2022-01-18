#pragma once
#include <vector>
#include <string>

namespace bndts {
	namespace lex {
        auto keywords = std::vector<std::string>{
            // Modifiers
            "����\\S\\S",		// const
            "�����\\S\\S",		// array
            "��",			// array dimension
            "�����\\S\\S",		// unsigned
            "�������\\S\\S",	// pointer

            "�����",		// func
            "��������",		// for
            "��",			// for
            "���",			// for
            "����",			// while
            "�����",		// do
            "��������",		// output
            "��",			// output
            "�",			// output
            "��",			// input
            "����",			// throw
            "����",			// break
            "���������",	// continue
            "����",			// null
            "������",		// if
            "������",		// then
            "�����",		// true
            "�����",		// false
            "������",		// then
            "�������",		// new
            "��������",		// delete
            "�������",		// return
            "������"		// struct
        };
        auto systemTypes = std::vector<std::string>{
            "��������",		// string
            "�����",		// int
            "�������",		// long
            "�������",		// float
            "�������",		// double
            "������",		// char
            "������",		// bool
        };
        auto operations = std::vector<std::string>{
            "�������",
            "<=",
            ">=",
            ">",
            "<",
            "++",
            "--",
            "<<",
            ">>",
            "�� �����", // init
            "��������", // set
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