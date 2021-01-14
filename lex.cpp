#include "lex.hpp"

bool findStrInVector (std::vector<std::string> strVec, std::string str) {
    return std::find(strVec.begin(), strVec.end(), str) != strVec.end();
}

lex::lex(const std::string user_input) {
    input = user_input;
    if (user_input == "quit") {
        exit (0);
    }
    current_input_pos = &input[0];
}

lex::~lex() {}

bool lex::is_valid() {
    return valid;
}

bool lex::isIdentifier (std::string token) {
    std::regex id ("[a-z]([a-z]|[0-9])*");
    std::smatch match;
    if (std::regex_match(token, match, id)) {
        return true;
    } else {
        return false;
    }
}

bool lex::isConstant (std::string token) {
    //              sign? (unsigned int) | 
    std::regex constant ("(\\+|\\-)?([0-9]+|[0-9]+(\\.[0-9]*)?((E|e)(\\+|\\-)[0-9]+))");
    std::smatch match;
    if (std::regex_match(token, match, constant)) {
        return true;
    } else {
        return false;
    }
}

bool lex::isRELOP (std::string token) {
   return findStrInVector(RELOP, token);
}

bool lex::isADDOP (std::string token) {
    return findStrInVector(ADDOP, token);
}

bool lex::isMULOP (std::string token) {
    return findStrInVector(MULOP, token);
}

std::tuple<std::string, std::string> lex::get_next_token() {
    std::string value = "";
    std::tuple<std::string, std::string> token;
    static bool maybeSign = false;
    auto new_input_pos = &current_input_pos[0];
    for (const char *c = current_input_pos; *c != '\0'; c++) {
        std::string cAsStr = "";
        cAsStr.push_back(*c);
        if (*c == ' ') {
            value.push_back(*c);
            break;
        } else if (*c == '(') {
            value.push_back(*c);
            maybeSign = true;
            break;
        } else if (*c == ')') {
            value.push_back(*c);
            break;
        } else if (*c == '<' || *c == '>'){
            value.push_back(*c);
            maybeSign = true;
            if (*(c+1) == '=') {
                c++;
                new_input_pos++;
                value.push_back(*c);
            }
            break;
        } else if (isRELOP(cAsStr) || isMULOP(cAsStr)) {
            value.push_back(*c);
            maybeSign = true;
            break;
        } else if (maybeSign && isADDOP(cAsStr)) {
            maybeSign = false;
            const char *aux = c + 1;
            while (*aux == ' ') {
                aux++;
            }
            std::string auxAsStr = "";
            auxAsStr.push_back(*aux);
            if (*aux != '\0' && *aux != ' ' && *aux != '(' && *aux != ')' && !isRELOP(auxAsStr) && !isMULOP(auxAsStr) && !isADDOP(auxAsStr)) {
                value.push_back(*c);
                new_input_pos+=(aux - c);
                c = aux - 1;
            }
        } else if (isADDOP(cAsStr)) {
            value.push_back(*c);
            maybeSign = false;
            break;
        } else {
            maybeSign = false;
            while (*c != '\0' && *c != ' ' && *c != '(' && *c != ')' && !isRELOP(cAsStr) && !isMULOP(cAsStr) && !isADDOP(cAsStr)) {
                value.push_back(*c);
                c++;
                new_input_pos++;
                if (tolower(*c) == 'e') {
                    value.push_back(*c);
                    c++;
                    new_input_pos++;
                    value.push_back(*c);
                    c++;
                    new_input_pos++;
                }
                cAsStr.pop_back();
                cAsStr.push_back(*c);
            }
            c--;
            new_input_pos--;
            break;
        }
    }
    current_input_pos = *new_input_pos != '\0' ? new_input_pos + 1 : new_input_pos;
    token = {value,value};
    return token;
}

std::vector<std::tuple<std::string, std::string>> lex::parse_input() {
    std::vector<std::tuple<std::string, std::string>> list_of_tokens;
    std::tuple<std::string, std::string> current_token;
    
    while (1) {
        current_token = get_next_token();
        std::string token_value = std::get<1>(current_token);
        if (token_value == "") {
            current_token = std::make_tuple("$","");
            list_of_tokens.push_back(current_token);
            return list_of_tokens;
        }
        
        if (isADDOP(token_value)) {
            current_token = std::make_tuple("ADDOP", token_value);
        } else if (isConstant(token_value)) {
            current_token = std::make_tuple("constant", token_value);
        } else if (isRELOP(token_value)) {
            current_token = std::make_tuple("RELOP", token_value);
        } else if (isMULOP(token_value)) {
            current_token = std::make_tuple("MULOP", token_value);
        } else if (isIdentifier(token_value)) {
            current_token = std::make_tuple("identifier", token_value);
        } else {
            current_token = current_token;
        }
        if (token_value != " ") {
            list_of_tokens.push_back(current_token);
        }
    }
}