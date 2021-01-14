#ifndef SYNT_HPP
#define SYNT_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <queue>

class productions
{
private:
    std::vector<std::pair<std::string, std::string>> all_rules;
    std::vector<std::string> heads;
    std::vector<std::vector<std::string>> tails;
    std::vector<std::string> all_symbols;
    // rule A ("A -> b");
    std::map<std::string, std::vector<std::vector<std::string>>> H_T_map;
    bool created_using_operator = false;
public:
    const std::string delimiter = " -> ";
    productions();
    productions operator= (const productions& p);
    void add_production(std::string line);
    std::vector<std::vector<std::string>> get_tail(std::string head);
    std::vector<std::string> get_heads();
    std::vector<std::string> get_all_symbols();
    std::vector<std::pair<std::string, std::string>> get_all_rules();
    void print_all_productions();
    bool is_non_terminal(std::string symbol);
    ~productions();
};

class itemizer
{
private:

public:
    itemizer();
    // Gets a tail and itemizes it
    // Input: T -> a B c (vector of strings)
    // Output: T -> .a B c, T -> a. B c, T -> a B. c, T -> a B c. (vector of vecotrs of strings)
    std::vector<std::vector<std::string>> itemize(std::vector<std::string> a_production);
    ~itemizer();
};

class item
{
private:
    // i["S"] = {".", "expr_list"}
public:
    std::map<std::string, std::vector<std::string>> i;
    std::string head;
    item(std::string head, std::vector<std::string> itemized_tail);
    bool operator ==(const item &itm) const;
    std::map<std::string, std::vector<std::string>> get_map ();
    std::string get_head_of_item();
    std::vector<std::string> get_tail_of_item();
    ~item();
};

//bool operator== (const item& itm1, const item& itm2);

class state
{
private:
public:
    unsigned int id;
    std::vector<item> items_in_state;
    std::vector<std::string> symbols_that_must_be_checked;
    unsigned int from_id;

    state(unsigned int from_id, unsigned int id);
    state(std::vector<item> itms);
    //state operator= (const state &st);
    bool operator ==(const state &st) const;
    unsigned int get_self_id();
    std::vector<item> get_items ();
    void calculate_symbols_that_must_be_checked_for_this_state();
    void set_items(std::vector<item> itms);
    std::vector<item> get_all_items_with_dot_before_symbol_with_dot_moved_after(std::string symbol, unsigned int for_state, productions P);
    std::vector<std::string> get_state_symbols ();
    void closure (item I, productions P);
    ~state();
};

//bool operator== (state& st1, state& st2);

class Goto
{
private:
    item I;
    std::string symbol;
public:
    Goto();
    ~Goto();
};

class syntatic
{
private:
    //https://stackoverflow.com/questions/9247948/how-to-write-3d-mapping-in-c
    std::map<unsigned int, std::map<std::string, unsigned int>> GOTO_table;
    std::map<unsigned int, std::map<std::string, std::string>> ACTION_table;
    std::vector<unsigned int> state_stack;
public:
    syntatic(std::map<unsigned int, std::map<std::string, unsigned int>> GOTO_table, std::map<unsigned int, std::map<std::string, std::string>> ACTION_table);
    void parse_input(std::vector<std::tuple<std::string, std::string>> list_of_tokens, productions P);
    ~syntatic();
};

/* ACTION Table
|-------|----
| stack | terminals |
|       | 

*/

#endif /* SYNT_HPP */