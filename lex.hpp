#ifndef LEX_HPP
#define LEX_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <tuple>
#include <regex>

bool findStrInVector (std::vector<std::string> strVec, std::string str);

class lex
{
private:
    std::string input;
    bool valid = true;
    const char *current_input_pos;
    std::vector<std::string> RELOP = {"=", "<", "<=", ">", ">=", "<>"};
    std::vector<std::string> ADDOP = {"+", "-", "or"};
    std::vector<std::string> MULOP = {"*", "/", "div", "mod", "and"};
public:
    lex(const std::string user_input);
    std::vector<std::tuple<std::string, std::string>> parse_input();
    std::tuple<std::string, std::string> get_next_token();
    bool is_valid();
    bool isIdentifier (std::string token);
    bool isConstant (std::string token);
    bool isRELOP (std::string token);
    bool isADDOP (std::string token);
    bool isMULOP (std::string token);
    ~lex();
};

#endif /* LEX_HPP */